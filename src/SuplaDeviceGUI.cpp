/*
  Copyright (C) krycha88

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include "SuplaDeviceGUI.h"
#include "SuplaConfigManager.h"
#include <supla/network/SuplaGuiWiFi.h>

#define TIME_SAVE_PERIOD_SEK                 30   // the time is given in seconds
#define TIME_SAVE_PERIOD_IMPULSE_COUNTER_SEK 600  // 10min
#define STORAGE_OFFSET                       0
#include <supla/storage/eeprom.h>
Supla::Eeprom eeprom(STORAGE_OFFSET);

Supla::GUIESPWifi *wifi = nullptr;

namespace Supla {
namespace GUI {
void begin() {
  setupWifi();
  enableWifiSSL(ConfigESP->checkSSL());

  SuplaDevice.setName(ConfigManager->get(KEY_HOST_NAME)->getValue());

#ifdef BUILD_VERSION
  SuplaDevice.setSwVersion(BUILD_VERSION);
#endif

  String server = ConfigManager->get(KEY_SUPLA_SERVER)->getValue();
  auto npos = server.indexOf(":");
  String suplaServer = server.substring(0, npos);

  SuplaDevice.begin((char *)ConfigManager->get(KEY_SUPLA_GUID)->getValue(),      // Global Unique Identifier
                    suplaServer.c_str(),                                         // SUPLA server address
                    ConfigManager->get(KEY_SUPLA_EMAIL)->getValue(),             // Email address used to login to Supla Cloud
                    (char *)ConfigManager->get(KEY_SUPLA_AUTHKEY)->getValue());  // Authorization key

  if (getCountChannels() == 0)
    ConfigESP->configModeInit();

  if (ConfigManager->get(KEY_ENABLE_GUI)->getValueInt())
    crateWebServer();
}

void setupWifi() {
  if (wifi) {
    delete wifi;
    wifi = nullptr;
  }

  wifi = new Supla::GUIESPWifi(ConfigManager->get(KEY_WIFI_SSID)->getValue(), ConfigManager->get(KEY_WIFI_PASS)->getValue());

  String suplaHostname = ConfigManager->get(KEY_HOST_NAME)->getValue();
  suplaHostname.replace(" ", "_");
  wifi->setHostName(suplaHostname.c_str());
}

void enableWifiSSL(bool value) {
  if (wifi) {
    if (ConfigESP->configModeESP == CONFIG_MODE) {
      wifi->enableSSL(false);
    }
    else {
      wifi->enableSSL(value);
    }
  }
}

void crateWebServer() {
  if (WebServer == NULL) {
    WebServer = new SuplaWebServer();
    WebServer->begin();
  }
}

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
void addRelayButton(uint8_t nr) {
  uint8_t pinRelay, pinButton, pinLED;
  bool highIsOn, levelLed;

  pinRelay = ConfigESP->getGpio(nr, FUNCTION_RELAY);
  pinButton = ConfigESP->getGpio(nr, FUNCTION_BUTTON);
  pinLED = ConfigESP->getGpio(nr, FUNCTION_LED);
  highIsOn = ConfigESP->getLevel(pinRelay);
  levelLed = ConfigESP->getInversed(pinLED);

  if (pinRelay != OFF_GPIO) {
    relay.push_back(new Supla::Control::Relay(pinRelay, highIsOn));

    int size = relay.size() - 1;

    switch (ConfigESP->getMemory(pinRelay)) {
      case MEMORY_RELAY_OFF:
        relay[size]->setDefaultStateOff();
        break;
      case MEMORY_RELAY_ON:
        relay[size]->setDefaultStateOn();
        break;
      case MEMORY_RELAY_RESTORE:
        relay[size]->setDefaultStateRestore();
        break;
    }

    relay[size]->keepTurnOnDuration();
    relay[size]->getChannel()->setDefault(SUPLA_CHANNELFNC_POWERSWITCH);

    if (pinButton != OFF_GPIO) {
      auto button = new Supla::Control::Button(pinButton, ConfigESP->getPullUp(pinButton), ConfigESP->getInversed(pinButton));

      button->addAction(ConfigESP->getAction(pinButton), *relay[size], ConfigESP->getEvent(pinButton));
      button->setSwNoiseFilterDelay(100);
    }

    if (pinLED != OFF_GPIO) {
      new Supla::Control::PinStatusLed(pinRelay, pinLED, !levelLed);
    }

#if defined(SUPLA_PUSHOVER)
    if (size <= MAX_PUSHOVER_MESSAGE) {
      if (strcmp(ConfigManager->get(KEY_PUSHOVER_MASSAGE)->getElement(size).c_str(), "") != 0 &&
          strcmp(ConfigManager->get(KEY_PUSHOVER_TOKEN)->getValue(), "") != 0 && strcmp(ConfigManager->get(KEY_PUSHOVER_USER)->getValue(), "") != 0) {
        auto pushover =
            new Supla::Control::Pushover(ConfigManager->get(KEY_PUSHOVER_TOKEN)->getValue(), ConfigManager->get(KEY_PUSHOVER_USER)->getValue(), true);

        pushover->setTitle(ConfigManager->get(KEY_HOST_NAME)->getValue());
        pushover->setMessage(ConfigManager->get(KEY_PUSHOVER_MASSAGE)->getElement(size).c_str());
        relay[size]->addAction(Pushover::SEND_NOTIF_1, pushover, Supla::ON_TURN_ON);
      }
    }
#endif

#if defined(SUPLA_DIRECT_LINKS)
    if (size <= MAX_DIRECT_LINK) {
      if (strcmp(ConfigManager->get(KEY_DIRECT_LINKS_ON)->getElement(size).c_str(), "") != 0 ||
          strcmp(ConfigManager->get(KEY_DIRECT_LINKS_OFF)->getElement(size).c_str(), "") != 0) {
        auto directLink = new Supla::Control::DirectLinks(ConfigManager->get(KEY_SUPLA_SERVER)->getValue());

        if (strcmp(ConfigManager->get(KEY_DIRECT_LINKS_ON)->getElement(size).c_str(), "") != 0) {
          directLink->setUrlON(ConfigManager->get(KEY_DIRECT_LINKS_ON)->getElement(size).c_str());
          relay[size]->addAction(DirectLinks::SEND_DIRECT_LINKS_ON, directLink, Supla::ON_TURN_ON);
        }
        if (strcmp(ConfigManager->get(KEY_DIRECT_LINKS_OFF)->getElement(size).c_str(), "") != 0) {
          directLink->setUrlOFF(ConfigManager->get(KEY_DIRECT_LINKS_OFF)->getElement(size).c_str());
          relay[size]->addAction(DirectLinks::SEND_DIRECT_LINKS_OFF, directLink, Supla::ON_TURN_OFF);
        }
      }
    }
#endif
  }
  delay(0);
}
#endif

#ifdef SUPLA_DS18B20
void addDS18B20MultiThermometer(int pinNumber) {
  if (ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt() > 1) {
    for (int i = 0; i < ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt(); ++i) {
      sensorDS.push_back(new DS18B20(pinNumber, HexToBytes(ConfigManager->get(KEY_ADDR_DS18B20)->getElement(i))));
      supla_log(LOG_DEBUG, "Index %d - address %s", i, ConfigManager->get(KEY_ADDR_DS18B20)->getElement(i).c_str());
    }
  }
  else {
    sensorDS.push_back(new DS18B20(ConfigESP->getGpio(FUNCTION_DS18B20)));
  }

  Supla::GUI::addConditionsTurnON(SENSOR_DS18B20, sensorDS[0]);
  Supla::GUI::addConditionsTurnOFF(SENSOR_DS18B20, sensorDS[0]);
}
#endif

#ifdef SUPLA_CONFIG
void addConfigESP(int pinNumberConfig, int pinLedConfig) {
  ConfigESP->addConfigESP(pinNumberConfig, pinLedConfig);
}
#endif

#ifdef SUPLA_ROLLERSHUTTER
void addRolleShutter(uint8_t nr) {
  int pinRelayUp, pinRelayDown, pinButtonUp, pinButtonDown, pullupButtonUp, pullupButtonDown, inversedButtonUp, inversedButtonDown, pinLedUp,
      pinLedDown, actionButtonUp, actionButtonDown = Supla::Action::MOVE_DOWN_OR_STOP, eventButtonUp;
  bool highIsOn, levelLedUp, levelLedDown;

  pinRelayUp = ConfigESP->getGpio(nr, FUNCTION_RELAY);
  pinRelayDown = ConfigESP->getGpio(nr + 1, FUNCTION_RELAY);

  pinButtonUp = ConfigESP->getGpio(nr, FUNCTION_BUTTON);
  pinButtonDown = ConfigESP->getGpio(nr + 1, FUNCTION_BUTTON);

  pullupButtonUp = ConfigESP->getPullUp(pinButtonUp);
  pullupButtonDown = ConfigESP->getPullUp(pinButtonDown);

  inversedButtonUp = ConfigESP->getInversed(pinButtonUp);
  inversedButtonDown = ConfigESP->getInversed(pinButtonDown);

  actionButtonUp = ConfigESP->getAction(pinButtonUp);
  eventButtonUp = ConfigESP->getEvent(pinButtonUp);

  pinLedUp = ConfigESP->getGpio(nr, FUNCTION_LED);
  pinLedDown = ConfigESP->getGpio(nr + 1, FUNCTION_LED);

  levelLedUp = ConfigESP->getInversed(pinLedUp);
  levelLedDown = ConfigESP->getInversed(pinLedDown);

  highIsOn = ConfigESP->getLevel(pinRelayUp);

  switch (actionButtonUp) {
    case Supla::GUI::ActionRolleShutter::OPEN_OR_CLOSE:
      actionButtonUp = Supla::Action::OPEN_OR_STOP;
      actionButtonDown = Supla::Action::CLOSE_OR_STOP;
      break;
    case Supla::GUI::ActionRolleShutter::MOVE_UP_OR_MOVE_DOWN:
      actionButtonUp = Supla::Action::MOVE_UP_OR_STOP;
      actionButtonDown = Supla::Action::MOVE_DOWN_OR_STOP;
      break;
    case Supla::GUI::ActionRolleShutter::STEP_BY_STEP:
      actionButtonUp = Supla::Action::STEP_BY_STEP;
      break;
  }

  auto RollerShutterRelay = new Supla::Control::RollerShutter(pinRelayUp, pinRelayDown, highIsOn);

  if (pinButtonUp != OFF_GPIO && actionButtonUp == Supla::Action::STEP_BY_STEP) {
    auto RollerShutterButtonOpen = new Supla::Control::Button(pinButtonUp, pullupButtonUp, inversedButtonUp);

    RollerShutterButtonOpen->addAction(actionButtonUp, RollerShutterRelay, eventButtonUp);
  }
  else if (pinButtonUp != OFF_GPIO && pinButtonDown != OFF_GPIO) {
    auto RollerShutterButtonOpen = new Supla::Control::Button(pinButtonUp, pullupButtonUp, inversedButtonUp);
    auto RollerShutterButtonClose = new Supla::Control::Button(pinButtonDown, pullupButtonDown, inversedButtonDown);

    if (eventButtonUp == Supla::Event::ON_CHANGE) {
      RollerShutterButtonOpen->addAction(actionButtonUp, RollerShutterRelay, Supla::Event::ON_PRESS);
      RollerShutterButtonOpen->addAction(Supla::Action::STOP, RollerShutterRelay, Supla::Event::ON_RELEASE);

      RollerShutterButtonClose->addAction(actionButtonDown, RollerShutterRelay, Supla::Event::ON_PRESS);
      RollerShutterButtonClose->addAction(Supla::Action::STOP, RollerShutterRelay, Supla::Event::ON_RELEASE);
    }
    else {
      RollerShutterButtonOpen->addAction(actionButtonUp, RollerShutterRelay, eventButtonUp);
      RollerShutterButtonClose->addAction(actionButtonDown, RollerShutterRelay, eventButtonUp);
    }
  }

  if (pinLedUp != OFF_GPIO) {
    new Supla::Control::PinStatusLed(pinRelayUp, pinLedUp, !levelLedUp);
  }
  if (pinLedDown != OFF_GPIO) {
    new Supla::Control::PinStatusLed(pinRelayDown, pinLedDown, !levelLedDown);
  }
  delay(0);
}
#endif

#ifdef SUPLA_IMPULSE_COUNTER
std::vector<Supla::Sensor::ImpulseCounter *> impulseCounter;

void addImpulseCounter(int pin, bool lowToHigh, bool inputPullup, unsigned int debounceDelay) {
  impulseCounter.push_back(new Supla::Sensor::ImpulseCounter(pin, lowToHigh, inputPullup, debounceDelay));
  eeprom.setStateSavePeriod(TIME_SAVE_PERIOD_IMPULSE_COUNTER_SEK * 1000);
}
#endif

#ifdef SUPLA_RGBW
void addRGBWLeds(uint8_t nr) {
  int redPin = ConfigESP->getGpio(nr, FUNCTION_RGBW_RED);
  int greenPin = ConfigESP->getGpio(nr, FUNCTION_RGBW_GREEN);
  int bluePin = ConfigESP->getGpio(nr, FUNCTION_RGBW_BLUE);
  int brightnessPin = ConfigESP->getGpio(nr, FUNCTION_RGBW_BRIGHTNESS);

  int buttonPin = ConfigESP->getGpio(nr, FUNCTION_BUTTON);
  int pullupButton = ConfigESP->getPullUp(buttonPin);
  int inversedButton = ConfigESP->getInversed(buttonPin);

  if (redPin != OFF_GPIO && greenPin != OFF_GPIO && bluePin != OFF_GPIO && brightnessPin != OFF_GPIO) {
    auto rgbw = new Supla::Control::RGBWLeds(redPin, greenPin, bluePin, brightnessPin);

    if (buttonPin != OFF_GPIO) {
      auto button = new Supla::Control::Button(buttonPin, pullupButton, inversedButton);
      button->setMulticlickTime(200);
      button->setHoldTime(400);
      button->repeatOnHoldEvery(200);

      button->addAction(Supla::ITERATE_DIM_ALL, rgbw, Supla::ON_HOLD);
      button->addAction(Supla::TOGGLE, rgbw, Supla::ON_CLICK_1);
    }
  }
  else if (redPin != OFF_GPIO && greenPin != OFF_GPIO && bluePin != OFF_GPIO) {
    auto rgbw = new Supla::Control::RGBLeds(redPin, greenPin, bluePin);

    if (buttonPin != OFF_GPIO) {
      auto button = new Supla::Control::Button(buttonPin, pullupButton, inversedButton);
      button->setMulticlickTime(200);
      button->setHoldTime(400);
      button->repeatOnHoldEvery(200);

      button->addAction(Supla::ITERATE_DIM_ALL, rgbw, Supla::ON_HOLD);
      button->addAction(Supla::TOGGLE, rgbw, Supla::ON_CLICK_1);
    }
  }
  else if (brightnessPin != OFF_GPIO) {
    auto rgbw = new Supla::Control::DimmerLeds(brightnessPin);

    if (buttonPin != OFF_GPIO) {
      auto button = new Supla::Control::Button(buttonPin, pullupButton, inversedButton);
      button->setMulticlickTime(200);
      button->setHoldTime(400);
      button->repeatOnHoldEvery(200);

      button->addAction(Supla::ITERATE_DIM_ALL, rgbw, Supla::ON_HOLD);
      button->addAction(Supla::TOGGLE, rgbw, Supla::ON_CLICK_1);
    }
  }
}
#endif

void addConditionsTurnON(int function, Supla::ChannelElement *client) {
#if defined(SUPLA_RELAY)
  if (Supla::GUI::relay.size() == 0)
    return;

  for (uint8_t nr = 0; nr <= OFF_GPIO; nr++) {
    if (ConfigManager->get(KEY_CONDITIONS_SENSOR_TYPE)->getElement(nr).toInt() == function &&
        strcmp(ConfigManager->get(KEY_CONDITIONS_MIN)->getElement(nr).c_str(), "") != 0) {
      double threshold = ConfigManager->get(KEY_CONDITIONS_MIN)->getElement(nr).toDouble();

      client->addAction(Supla::TURN_OFF, Supla::GUI::relay[nr], OnInvalid());

      switch (ConfigManager->get(KEY_CONDITIONS_TYPE)->getElement(nr).toInt()) {
        case HEATING:
          client->addAction(Supla::TURN_ON, Supla::GUI::relay[nr], OnLess(threshold));
          break;

        case COOLING:
          client->addAction(Supla::TURN_ON, Supla::GUI::relay[nr], OnGreater(threshold));
          break;

        case MOISTURIZING:
          client->addAction(Supla::TURN_ON, Supla::GUI::relay[nr], OnLess(threshold, true));
          break;

        case DRAINGE:
          client->addAction(Supla::TURN_ON, Supla::GUI::relay[nr], OnGreater(threshold, true));
          break;
      }
    }
  }
#endif
}

void addConditionsTurnOFF(int function, Supla::ChannelElement *client) {
#if defined(SUPLA_RELAY)
  if (Supla::GUI::relay.size() == 0)
    return;

  for (uint8_t nr = 0; nr <= OFF_GPIO; nr++) {
    if (ConfigManager->get(KEY_CONDITIONS_SENSOR_TYPE)->getElement(nr).toInt() == function &&
        strcmp(ConfigManager->get(KEY_CONDITIONS_MAX)->getElement(nr).c_str(), "") != 0) {
      double threshold = ConfigManager->get(KEY_CONDITIONS_MAX)->getElement(nr).toDouble();

      client->addAction(Supla::TURN_OFF, Supla::GUI::relay[nr], OnInvalid());

      switch (ConfigManager->get(KEY_CONDITIONS_TYPE)->getElement(nr).toInt()) {
        case HEATING:
          client->addAction(Supla::TURN_OFF, Supla::GUI::relay[nr], OnGreater(threshold));
          break;

        case COOLING:
          client->addAction(Supla::TURN_OFF, Supla::GUI::relay[nr], OnLess(threshold));
          break;

        case MOISTURIZING:
          client->addAction(Supla::TURN_OFF, Supla::GUI::relay[nr], OnGreater(threshold, true));
          break;

        case DRAINGE:
          client->addAction(Supla::TURN_OFF, Supla::GUI::relay[nr], OnLess(threshold, true));
          break;
      }
    }
  }
#endif
}

void addCorrectionSensor() {
  double correction;

  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_THERMOMETER) {
        correction = ConfigManager->get(KEY_CORRECTION_TEMP)->getElement(channel->getChannelNumber()).toDouble();
        Supla::Correction::add(channel->getChannelNumber(), correction);
      }

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {
        correction = ConfigManager->get(KEY_CORRECTION_TEMP)->getElement(channel->getChannelNumber()).toDouble();
        Supla::Correction::add(channel->getChannelNumber(), correction);

        correction = ConfigManager->get(KEY_CORRECTION_HUMIDITY)->getElement(channel->getChannelNumber()).toDouble();
        Supla::Correction::add(channel->getChannelNumber(), correction, true);
      }
    }
  }
}

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER) || defined(SUPLA_PUSHOVER)
std::vector<Supla::Control::Relay *> relay;
#endif

#ifdef SUPLA_DS18B20
std::vector<DS18B20 *> sensorDS;
#endif

#ifdef SUPLA_HLW8012
Supla::Sensor::HLW_8012 *counterHLW8012 = nullptr;

void addHLW8012(int8_t pinCF, int8_t pinCF1, int8_t pinSEL) {
  if (counterHLW8012 == NULL && pinCF != OFF_GPIO && pinCF1 != OFF_GPIO && pinSEL != OFF_GPIO) {
    counterHLW8012 = new Supla::Sensor::HLW_8012(pinCF, pinCF1, pinSEL);
  }
  eeprom.setStateSavePeriod(TIME_SAVE_PERIOD_IMPULSE_COUNTER_SEK * 1000);
}
#endif

#ifdef SUPLA_CSE7766
Supla::Sensor::CSE_7766 *counterCSE7766 = nullptr;

void addCSE7766(int8_t pinRX) {
  if (counterCSE7766 == NULL && pinRX != OFF_GPIO) {
    counterCSE7766 = new Supla::Sensor::CSE_7766(pinRX);
  }
  eeprom.setStateSavePeriod(TIME_SAVE_PERIOD_IMPULSE_COUNTER_SEK * 1000);
}
#endif

#ifdef SUPLA_MPX_5XXX
Supla::Sensor::MPX_5XXX *mpx = nullptr;
#endif

#ifdef SUPLA_ANALOG_READING_MAP
Supla::Sensor::AnalogRedingMap *analog = nullptr;
#endif

}  // namespace GUI
}  // namespace Supla

SuplaConfigManager *ConfigManager = nullptr;
SuplaConfigESP *ConfigESP = nullptr;
SuplaWebServer *WebServer = nullptr;
