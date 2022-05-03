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

#define TIME_SAVE_PERIOD_SEK                 30   // the time is given in seconds
#define TIME_SAVE_PERIOD_IMPULSE_COUNTER_SEK 600  // 10min
#define STORAGE_OFFSET                       0
#include <supla/storage/eeprom.h>

Supla::Eeprom eeprom(STORAGE_OFFSET);

namespace Supla {
namespace GUI {
void begin() {
  setupConnection();
  enableConnectionSSL(ConfigESP->checkSSL());

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
    ConfigESP->configModeInit(WIFI_AP_STA);

  if (ConfigManager->get(KEY_ENABLE_GUI)->getValueInt())
    crateWebServer();
}

void setupConnection() {
#ifdef SUPLA_WT32_ETH01_LAN8720
  if (eth == nullptr) {
    eth = new Supla::WT32_ETH01(1);  // uint_t ETH_ADDR = I²C-address of Ethernet PHY (0 or 1)
  }
#else
  if (wifi) {
    wifi->setSsid(ConfigManager->get(KEY_WIFI_SSID)->getValue());
    wifi->setPassword(ConfigManager->get(KEY_WIFI_PASS)->getValue());
    Supla::Network::Setup();
  }
  else {
    wifi = new Supla::GUIESPWifi(ConfigManager->get(KEY_WIFI_SSID)->getValue(), ConfigManager->get(KEY_WIFI_PASS)->getValue());
  }

  String suplaHostname = ConfigManager->get(KEY_HOST_NAME)->getValue();
  suplaHostname.replace(" ", "_");
  wifi->setHostName(suplaHostname.c_str());

#endif
}

void enableConnectionSSL(bool value) {
#ifdef SUPLA_WT32_ETH01_LAN8720

  if (eth) {
    if (ConfigESP->configModeESP == CONFIG_MODE) {
      eth->enableSSL(false);
    }
    else {
      eth->enableSSL(value);
    }
  }

#else
  if (wifi) {
    if (ConfigESP->configModeESP == CONFIG_MODE) {
      wifi->enableSSL(false);
    }
    else {
      wifi->enableSSL(value);
    }
  }
#endif
}

void crateWebServer() {
  if (WebServer == NULL) {
    WebServer = new SuplaWebServer();
    WebServer->begin();
  }
}

#if defined(SUPLA_RELAY)
void addRelay(uint8_t nr) {
  uint8_t pinRelay, pinLED;
  bool highIsOn, levelLed;

  pinRelay = ConfigESP->getGpio(nr, FUNCTION_RELAY);
  pinLED = ConfigESP->getGpio(nr, FUNCTION_LED);
  levelLed = ConfigESP->getInversed(pinLED);

  if (pinRelay != OFF_GPIO) {
    if (pinRelay == GPIO_VIRTUAL_RELAY) {
      relay.push_back(new Supla::Control::VirtualRelay());
    }
    else {
      highIsOn = ConfigESP->getLevel(pinRelay);
      relay.push_back(new Supla::Control::Relay(pinRelay, highIsOn));
    }

    switch (ConfigESP->getMemory(pinRelay, nr)) {
      case MEMORY_OFF:
        relay[nr]->setDefaultStateOff();
        break;
      case MEMORY_ON:
        relay[nr]->setDefaultStateOn();
        break;
      case MEMORY_RESTORE:
        relay[nr]->setDefaultStateRestore();
        break;
    }

    relay[nr]->keepTurnOnDuration();
    relay[nr]->getChannel()->setDefault(SUPLA_CHANNELFNC_POWERSWITCH);

    if (pinLED != OFF_GPIO) {
      new Supla::Control::PinStatusLed(pinRelay, pinLED, !levelLed);
    }
  }
  else {
    relay.push_back(nullptr);
  }
  delay(0);
}

#ifdef SUPLA_BUTTON
void addButtonToRelay(uint8_t nr) {
  uint8_t pinButton, nrButton, pinRelay;

  nrButton = ConfigESP->getNumberButton(nr);
  pinButton = ConfigESP->getGpio(nr, FUNCTION_BUTTON);
  pinRelay = ConfigESP->getGpio(nrButton, FUNCTION_RELAY);

  if (pinButton != OFF_GPIO && pinRelay != OFF_GPIO) {
    Supla::Control::Button *button;

    if (pinButton == A0) {
      button = new Supla::Control::ButtonAnalog(A0, ConfigManager->get(KEY_ANALOG_INPUT_EXPECTED)->getElement(nr).toInt());
    }
    else {
      button = new Supla::Control::Button(pinButton, ConfigESP->getPullUp(pinButton), ConfigESP->getInversed(pinButton));
      button->setSwNoiseFilterDelay(50);
    }

    if (ConfigESP->getEvent(pinButton) == Supla::ON_HOLD) {
      int holdTimeMs = String(ConfigManager->get(KEY_AT_HOLD_TIME)->getValue()).toDouble() * 1000;
      button->setHoldTime(holdTimeMs);
      button->repeatOnHoldEvery(2000);
    }

    button->addAction(ConfigESP->getAction(pinButton), relay[nrButton], ConfigESP->getEvent(pinButton));

#ifdef SUPLA_ACTION_TRIGGER
    addActionTriggerRelatedChannel(button, ConfigESP->getEvent(pinButton), relay[nrButton]);
#endif
  }
  else {
#ifdef SUPLA_ACTION_TRIGGER
    Supla::GUI::addButtonActionTrigger(nr);
#endif
  }
  delay(0);
}
#endif
#endif

#ifdef SUPLA_ACTION_TRIGGER
void addButtonActionTrigger(uint8_t nr) {
  uint8_t pinButton = ConfigESP->getGpio(nr, FUNCTION_BUTTON);

  if (pinButton != OFF_GPIO) {
    auto button = new Supla::Control::Button(pinButton, ConfigESP->getPullUp(pinButton), ConfigESP->getInversed(pinButton));
    button->setSwNoiseFilterDelay(100);
    auto at = new Supla::Control::ActionTrigger();

    button->addAction(ConfigESP->getAction(pinButton), at, ConfigESP->getEvent(pinButton));

    int muliclickTimeMs = String(ConfigManager->get(KEY_AT_MULTICLICK_TIME)->getValue()).toDouble() * 1000;
    int holdTimeMs = String(ConfigManager->get(KEY_AT_HOLD_TIME)->getValue()).toDouble() * 1000;

    if (ConfigESP->getEvent(pinButton) == Supla::ON_CHANGE) {
      button->setMulticlickTime(muliclickTimeMs, true);
    }
    else {
      button->setMulticlickTime(muliclickTimeMs);
      button->setHoldTime(holdTimeMs);
    }

    at->attach(button);
  }
}
#endif

#ifdef SUPLA_ACTION_TRIGGER
void addActionTriggerRelatedChannel(Supla::Control::Button *button, int eventButton, Supla::Element *element) {
  auto at = new Supla::Control::ActionTrigger();
  button->setSwNoiseFilterDelay(100);

  int muliclickTimeMs = String(ConfigManager->get(KEY_AT_MULTICLICK_TIME)->getValue()).toDouble() * 1000;
  int holdTimeMs = String(ConfigManager->get(KEY_AT_HOLD_TIME)->getValue()).toDouble() * 1000;

  if (eventButton == Supla::ON_CHANGE) {
    button->setMulticlickTime(muliclickTimeMs, true);
  }
  else {
    button->setMulticlickTime(muliclickTimeMs);
    button->setHoldTime(holdTimeMs);
  }
  if (element != NULL) {
    at->setRelatedChannel(element);
  }
  at->attach(button);
}
#endif

#if defined(SUPLA_RF_BRIDGE)
void addRelayBridge(uint8_t nr) {
  uint8_t pinRelay, pinLED, pinTransmitter;
  bool highIsOn, levelLed;

  pinTransmitter = ConfigESP->getGpio(FUNCTION_RF_BRIDGE_TRANSMITTER);

  pinRelay = ConfigESP->getGpio(nr, FUNCTION_RELAY);
  pinLED = ConfigESP->getGpio(nr, FUNCTION_LED);
  levelLed = ConfigESP->getInversed(pinLED);

  if (pinRelay != OFF_GPIO && pinTransmitter != OFF_GPIO) {
    if (pinRelay == GPIO_VIRTUAL_RELAY) {
      auto bridgeVirtualRelay = new Supla::Control::RFBridgeVirtualRelay(pinTransmitter);
      bridgeVirtualRelay->setRepeatProtocol(ConfigManager->get(KEY_RF_BRIDGE_PROTOCOL)->getElement(nr).toInt());
      bridgeVirtualRelay->setPulseLengthint(ConfigManager->get(KEY_RF_BRIDGE_PULSE_LENGTHINT)->getElement(nr).toInt());
      bridgeVirtualRelay->setRepeatTransmit(20);
      bridgeVirtualRelay->setRepeatSending(ConfigManager->get(KEY_RF_BRIDGE_REPEAT)->getElement(nr).toInt());

      bridgeVirtualRelay->setCodeLength(ConfigManager->get(KEY_RF_BRIDGE_LENGTH)->getElement(nr).toInt());
      bridgeVirtualRelay->setCodeON(ConfigManager->get(KEY_RF_BRIDGE_CODE_ON)->getElement(nr).toInt());
      bridgeVirtualRelay->setCodeOFF(ConfigManager->get(KEY_RF_BRIDGE_CODE_OFF)->getElement(nr).toInt());

      relay.push_back(bridgeVirtualRelay);
    }
    else {
      highIsOn = ConfigESP->getLevel(pinRelay);
      auto bridgeRelay = new Supla::Control::RFBridgeRelay(pinTransmitter, pinRelay, highIsOn);
      bridgeRelay->setRepeatProtocol(ConfigManager->get(KEY_RF_BRIDGE_PROTOCOL)->getElement(nr).toInt());
      bridgeRelay->setPulseLengthint(ConfigManager->get(KEY_RF_BRIDGE_PULSE_LENGTHINT)->getElement(nr).toInt());
      bridgeRelay->setRepeatTransmit(20);
      bridgeRelay->setRepeatSending(ConfigManager->get(KEY_RF_BRIDGE_REPEAT)->getElement(nr).toInt());

      bridgeRelay->setCodeLength(ConfigManager->get(KEY_RF_BRIDGE_LENGTH)->getElement(nr).toInt());
      bridgeRelay->setCodeON(ConfigManager->get(KEY_RF_BRIDGE_CODE_ON)->getElement(nr).toInt());
      bridgeRelay->setCodeOFF(ConfigManager->get(KEY_RF_BRIDGE_CODE_OFF)->getElement(nr).toInt());

      relay.push_back(bridgeRelay);
    }

    int size = relay.size() - 1;

    switch (ConfigESP->getMemory(pinRelay, nr)) {
      case MEMORY_OFF:
        relay[size]->setDefaultStateOff();
        break;
      case MEMORY_ON:
        relay[size]->setDefaultStateOn();
        break;
      case MEMORY_RESTORE:
        relay[size]->setDefaultStateRestore();
        break;
    }

    relay[size]->keepTurnOnDuration();
    relay[size]->getChannel()->setDefault(SUPLA_CHANNELFNC_POWERSWITCH);

    if (pinLED != OFF_GPIO) {
      new Supla::Control::PinStatusLed(pinRelay, pinLED, !levelLed);
    }
  }
  delay(0);
}

void addButtonBridge(uint8_t nr) {
  uint8_t pinButton;
  pinButton = ConfigESP->getGpio(FUNCTION_RF_BRIDGE_RECEIVE);

  if (pinButton != OFF_GPIO) {
    auto receiveBridge = new Supla::Control::RFBridgeReceive(pinButton);
    receiveBridge->setCodeON(ConfigManager->get(KEY_RF_BRIDGE_CODE_ON)->getElement(nr).toInt());
    receiveBridge->setCodeOFF(ConfigManager->get(KEY_RF_BRIDGE_CODE_OFF)->getElement(nr).toInt());

    receiveBridge->addAction(Supla::TURN_ON, relay[nr], Supla::TURN_ON);
    receiveBridge->addAction(Supla::TURN_OFF, relay[nr], Supla::TURN_OFF);
  }
}
#endif

#if defined(SUPLA_PUSHOVER)
void addPushover(uint8_t nr) {
  if (nr <= MAX_PUSHOVER_MESSAGE) {
    if (strcmp(ConfigManager->get(KEY_PUSHOVER_MASSAGE)->getElement(nr).c_str(), "") != 0 &&
        strcmp(ConfigManager->get(KEY_PUSHOVER_TOKEN)->getValue(), "") != 0 && strcmp(ConfigManager->get(KEY_PUSHOVER_USER)->getValue(), "") != 0) {
      auto pushover =
          new Supla::Control::Pushover(ConfigManager->get(KEY_PUSHOVER_TOKEN)->getValue(), ConfigManager->get(KEY_PUSHOVER_USER)->getValue(), true);

      pushover->setTitle(ConfigManager->get(KEY_HOST_NAME)->getValue());
      pushover->setMessage(ConfigManager->get(KEY_PUSHOVER_MASSAGE)->getElement(nr).c_str());
      relay[nr]->addAction(Pushover::SEND_NOTIF_1, pushover, Supla::ON_TURN_ON);
    }
  }
}
#endif

#if defined(SUPLA_DIRECT_LINKS)
void addDirectLinks(uint8_t nr) {
  if (nr <= MAX_DIRECT_LINK) {
    if (strcmp(ConfigManager->get(KEY_DIRECT_LINKS_ON)->getElement(nr).c_str(), "") != 0 ||
        strcmp(ConfigManager->get(KEY_DIRECT_LINKS_OFF)->getElement(nr).c_str(), "") != 0) {
      auto directLink = new Supla::Control::DirectLinks(ConfigManager->get(KEY_SUPLA_SERVER)->getValue());

      if (strcmp(ConfigManager->get(KEY_DIRECT_LINKS_ON)->getElement(nr).c_str(), "") != 0) {
        directLink->setUrlON(ConfigManager->get(KEY_DIRECT_LINKS_ON)->getElement(nr).c_str());
        relay[nr]->addAction(DirectLinks::SEND_DIRECT_LINKS_ON, directLink, Supla::ON_TURN_ON);
      }
      if (strcmp(ConfigManager->get(KEY_DIRECT_LINKS_OFF)->getElement(nr).c_str(), "") != 0) {
        directLink->setUrlOFF(ConfigManager->get(KEY_DIRECT_LINKS_OFF)->getElement(nr).c_str());
        relay[nr]->addAction(DirectLinks::SEND_DIRECT_LINKS_OFF, directLink, Supla::ON_TURN_OFF);
      }
    }
  }
}
#endif

#ifdef SUPLA_DS18B20
void addDS18B20MultiThermometer(int pinNumber) {
  if (ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt() > 1) {
    for (int i = 0; i < ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt(); ++i) {
      sensorDS.push_back(new DS18B20(pinNumber, HexToBytes(ConfigManager->get(KEY_ADDR_DS18B20)->getElement(i))));
      supla_log(LOG_DEBUG, "Index %d - address %s", i, ConfigManager->get(KEY_ADDR_DS18B20)->getElement(i).c_str());

      Supla::GUI::addConditionsTurnON(SENSOR_DS18B20, sensorDS[i], i);
      Supla::GUI::addConditionsTurnOFF(SENSOR_DS18B20, sensorDS[i], i);
    }
  }
  else {
    sensorDS.push_back(new DS18B20(ConfigESP->getGpio(FUNCTION_DS18B20)));
    Supla::GUI::addConditionsTurnON(SENSOR_DS18B20, sensorDS[0]);
    Supla::GUI::addConditionsTurnOFF(SENSOR_DS18B20, sensorDS[0]);
  }
}
#endif

#ifdef SUPLA_CONFIG
void addConfigESP(int pinNumberConfig, int pinLedConfig) {
  ConfigESP->addConfigESP(pinNumberConfig, pinLedConfig);
}
#endif

#ifdef SUPLA_ROLLERSHUTTER
void addRolleShutter(uint8_t nr) {
  int pinRelayUp, pinRelayDown, pinButtonUp, pinButtonDown, pinButtonStop, pullupButtonUp, pullupButtonDown, pullupButtonStop, inversedButtonUp,
      inversedButtonDown, inversedButtonStop, pinLedUp, pinLedDown, actionButtonUp, actionButtonDown = Supla::Action::MOVE_DOWN_OR_STOP,
                                                                                    eventButtonUp, eventButtonStop;
  bool highIsOn, levelLedUp, levelLedDown;

  pinRelayUp = ConfigESP->getGpio(nr, FUNCTION_RELAY);
  pinRelayDown = ConfigESP->getGpio(nr + 1, FUNCTION_RELAY);

  pinButtonUp = ConfigESP->getGpio(nr, FUNCTION_BUTTON);
  pinButtonDown = ConfigESP->getGpio(nr + 1, FUNCTION_BUTTON);
  pinButtonStop = ConfigESP->getGpio(nr, FUNCTION_BUTTON_STOP);

  pullupButtonUp = ConfigESP->getPullUp(pinButtonUp);
  pullupButtonDown = ConfigESP->getPullUp(pinButtonDown);
  pullupButtonStop = ConfigESP->getPullUp(pinButtonStop);

  inversedButtonUp = ConfigESP->getInversed(pinButtonUp);
  inversedButtonDown = ConfigESP->getInversed(pinButtonDown);
  inversedButtonStop = ConfigESP->getInversed(pinButtonStop);

  actionButtonUp = ConfigESP->getAction(pinButtonUp);

  eventButtonUp = ConfigESP->getEvent(pinButtonUp);
  eventButtonStop = ConfigESP->getEvent(pinButtonStop);

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
#ifdef SUPLA_ACTION_TRIGGER
    addActionTriggerRelatedChannel(RollerShutterButtonOpen, eventButtonUp, RollerShutterRelay);
#endif

    RollerShutterButtonOpen->addAction(actionButtonUp, RollerShutterRelay, eventButtonUp);
  }
  else if (pinButtonUp != OFF_GPIO && pinButtonDown != OFF_GPIO) {
    auto RollerShutterButtonOpen = new Supla::Control::Button(pinButtonUp, pullupButtonUp, inversedButtonUp);
    auto RollerShutterButtonClose = new Supla::Control::Button(pinButtonDown, pullupButtonDown, inversedButtonDown);

#ifdef SUPLA_ACTION_TRIGGER
    addActionTriggerRelatedChannel(RollerShutterButtonOpen, eventButtonUp, RollerShutterRelay);
    addActionTriggerRelatedChannel(RollerShutterButtonClose, eventButtonUp, RollerShutterRelay);
#endif

    if (eventButtonUp == Supla::Event::ON_CHANGE) {
      RollerShutterButtonOpen->addAction(actionButtonUp, RollerShutterRelay, Supla::Event::ON_PRESS);
      if (pinButtonStop == OFF_GPIO)
        RollerShutterButtonOpen->addAction(Supla::Action::STOP, RollerShutterRelay, Supla::Event::ON_RELEASE);

      RollerShutterButtonClose->addAction(actionButtonDown, RollerShutterRelay, Supla::Event::ON_PRESS);
      if (pinButtonStop == OFF_GPIO)
        RollerShutterButtonClose->addAction(Supla::Action::STOP, RollerShutterRelay, Supla::Event::ON_RELEASE);
    }
    else {
      RollerShutterButtonOpen->addAction(actionButtonUp, RollerShutterRelay, eventButtonUp);
      RollerShutterButtonClose->addAction(actionButtonDown, RollerShutterRelay, eventButtonUp);
    }
  }

  if (pinButtonStop != OFF_GPIO) {
    auto RollerShutterButtonStop = new Supla::Control::Button(pinButtonStop, pullupButtonStop, inversedButtonStop);
    RollerShutterButtonStop->addAction(Supla::Action::STOP, RollerShutterRelay, eventButtonStop);
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

void addImpulseCounter(uint8_t nr) {
  uint8_t pin, pinLED, debounceDelay;
  bool lowToHigh, inputPullup, levelLed;

  pin = ConfigESP->getGpio(nr, FUNCTION_IMPULSE_COUNTER);
  lowToHigh = ConfigESP->getLevel(pin);
  inputPullup = ConfigESP->getMemory(pin);
  debounceDelay = ConfigManager->get(KEY_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT)->getValueInt();

  pinLED = ConfigESP->getGpio(nr, FUNCTION_LED);
  levelLed = ConfigESP->getInversed(pinLED);

  impulseCounter.push_back(new Supla::Sensor::ImpulseCounter(pin, lowToHigh, inputPullup, debounceDelay));

  if (pinLED != OFF_GPIO) {
    auto led = new Supla::Control::InternalPinOutput(pinLED, levelLed);
    led->setDurationMs(100);
    impulseCounter[nr]->addAction(Supla::TURN_ON, led, Supla::ON_CHANGE);
  }

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

#ifdef ARDUINO_ARCH_ESP8266
  // https://forum.supla.org/viewtopic.php?p=116483#p116483
  analogWriteFreq(400);
#endif

  if (redPin != OFF_GPIO && greenPin != OFF_GPIO && bluePin != OFF_GPIO && brightnessPin != OFF_GPIO) {
    auto rgbw = new Supla::Control::RGBWLeds(redPin, greenPin, bluePin, brightnessPin);
    setRGBWDefaultState(rgbw, ConfigESP->getMemory(redPin));
    setRGBWButton(rgbw, buttonPin);
  }
  else if (redPin != OFF_GPIO && greenPin != OFF_GPIO && bluePin != OFF_GPIO) {
    auto rgbw = new Supla::Control::RGBLeds(redPin, greenPin, bluePin);
    setRGBWDefaultState(rgbw, ConfigESP->getMemory(redPin));
    setRGBWButton(rgbw, buttonPin);
  }
  else if (brightnessPin != OFF_GPIO) {
    auto rgbw = new Supla::Control::DimmerLeds(brightnessPin);
    setRGBWDefaultState(rgbw, ConfigESP->getMemory(brightnessPin));
    setRGBWButton(rgbw, buttonPin);
  }
}

void setRGBWButton(Supla::Control::RGBWBase *rgbw, int buttonPin) {
  int pullupButton = ConfigESP->getPullUp(buttonPin);
  int inversedButton = ConfigESP->getInversed(buttonPin);

  if (buttonPin != OFF_GPIO) {
    auto button = new Supla::Control::Button(buttonPin, pullupButton, inversedButton);
    button->setMulticlickTime(200);
    button->setHoldTime(400);
    button->repeatOnHoldEvery(200);

    button->addAction(Supla::ITERATE_DIM_ALL, rgbw, Supla::ON_HOLD);
    button->addAction(Supla::TOGGLE, rgbw, Supla::ON_CLICK_1);
  }
}

void setRGBWDefaultState(Supla::Control::RGBWBase *rgbw, uint8_t memory) {
  switch (memory) {
    case MEMORY_OFF:
      rgbw->setDefaultStateOff();
      break;
    case MEMORY_ON:
      rgbw->setDefaultStateOn();
      break;
    case MEMORY_RESTORE:
      rgbw->setDefaultStateRestore();
      break;
  }
}
#endif

void addConditionsTurnON(int function, Supla::ChannelElement *client, uint8_t sensorNumber) {
#if defined(SUPLA_RELAY) && defined(SUPLA_CONDITIONS)
  for (uint8_t nr = 0; nr <= OFF_GPIO; nr++) {
    if (ConfigManager->get(KEY_CONDITIONS_SENSOR_TYPE)->getElement(nr).toInt() == function &&
        strcmp(ConfigManager->get(KEY_CONDITIONS_MIN)->getElement(nr).c_str(), "") != 0 &&
        ConfigManager->get(KEY_CONDITIONS_SENSOR_NUMBER)->getElement(nr).toInt() == sensorNumber &&
        ConfigESP->getGpio(nr, FUNCTION_RELAY) != OFF_GPIO) {
      // Serial.println("addConditionsTurnON");
      // Serial.println(ConfigManager->get(KEY_CONDITIONS_SENSOR_NUMBER)->getElement(nr).toInt());
      // Serial.println(ConfigManager->get(KEY_CONDITIONS_MIN)->getElement(nr).c_str());

      double threshold = ConfigManager->get(KEY_CONDITIONS_MIN)->getElement(nr).toDouble();

      client->addAction(Supla::TURN_OFF, Supla::GUI::relay[nr], OnInvalid());

      switch (ConfigManager->get(KEY_CONDITIONS_TYPE)->getElement(nr).toInt()) {
        case CONDITION_HEATING:
          client->addAction(Supla::TURN_ON, Supla::GUI::relay[nr], OnLess(threshold));
          break;
        case CONDITION_COOLING:
          client->addAction(Supla::TURN_ON, Supla::GUI::relay[nr], OnGreater(threshold));
          break;
        case CONDITION_MOISTURIZING:
          client->addAction(Supla::TURN_ON, Supla::GUI::relay[nr], OnLess(threshold, true));
          break;
        case CONDITION_DRAINGE:
          client->addAction(Supla::TURN_ON, Supla::GUI::relay[nr], OnGreater(threshold, true));
          break;
        case CONDITION_GPIO:
          client->addAction(Supla::TURN_ON, Supla::GUI::relay[nr], Supla::ON_TURN_ON);
          break;
      }
    }
  }
#endif
}

void addConditionsTurnOFF(int function, Supla::ChannelElement *client, uint8_t sensorNumber) {
#if defined(SUPLA_RELAY) && defined(SUPLA_CONDITIONS)
  for (uint8_t nr = 0; nr <= OFF_GPIO; nr++) {
    if (ConfigManager->get(KEY_CONDITIONS_SENSOR_TYPE)->getElement(nr).toInt() == function &&
        strcmp(ConfigManager->get(KEY_CONDITIONS_MAX)->getElement(nr).c_str(), "") != 0 &&
        ConfigManager->get(KEY_CONDITIONS_SENSOR_NUMBER)->getElement(nr).toInt() == sensorNumber &&
        ConfigESP->getGpio(nr, FUNCTION_RELAY) != OFF_GPIO) {
      // Serial.println("addConditionsTurnOFF");
      // Serial.println(ConfigManager->get(KEY_CONDITIONS_SENSOR_NUMBER)->getElement(nr).toInt());
      // Serial.println(ConfigManager->get(KEY_CONDITIONS_MAX)->getElement(nr).c_str());

      double threshold = ConfigManager->get(KEY_CONDITIONS_MAX)->getElement(nr).toDouble();

      client->addAction(Supla::TURN_OFF, Supla::GUI::relay[nr], OnInvalid());

      switch (ConfigManager->get(KEY_CONDITIONS_TYPE)->getElement(nr).toInt()) {
        case CONDITION_HEATING:
          client->addAction(Supla::TURN_OFF, Supla::GUI::relay[nr], OnGreater(threshold));
          break;
        case CONDITION_COOLING:
          client->addAction(Supla::TURN_OFF, Supla::GUI::relay[nr], OnLess(threshold));
          break;
        case CONDITION_MOISTURIZING:
          client->addAction(Supla::TURN_OFF, Supla::GUI::relay[nr], OnGreater(threshold, true));
          break;
        case CONDITION_DRAINGE:
          client->addAction(Supla::TURN_OFF, Supla::GUI::relay[nr], OnLess(threshold, true));
          break;
        case CONDITION_GPIO:
          client->addAction(Supla::TURN_OFF, Supla::GUI::relay[nr], Supla::ON_TURN_OFF);
          break;
      }
    }
  }
#endif
}

void addConditionsTurnON(int function, Supla::Sensor::ElectricityMeter *client, uint8_t sensorNumber) {
#if defined(SUPLA_RELAY) && defined(SUPLA_CONDITIONS)
  for (uint8_t nr = 0; nr <= OFF_GPIO; nr++) {
    if (ConfigManager->get(KEY_CONDITIONS_SENSOR_TYPE)->getElement(nr).toInt() == function &&
        strcmp(ConfigManager->get(KEY_CONDITIONS_MIN)->getElement(nr).c_str(), "") != 0 &&
        ConfigManager->get(KEY_CONDITIONS_SENSOR_NUMBER)->getElement(nr).toInt() == sensorNumber &&
        ConfigESP->getGpio(nr, FUNCTION_RELAY) != OFF_GPIO) {
      // Serial.println("addConditionsTurnON - ElectricityMeter");
      // Serial.println(ConfigManager->get(KEY_CONDITIONS_SENSOR_NUMBER)->getElement(nr).toInt());
      // Serial.println(ConfigManager->get(KEY_CONDITIONS_MIN)->getElement(nr).c_str());

      double threshold = ConfigManager->get(KEY_CONDITIONS_MIN)->getElement(nr).toDouble();
      // client->addAction(Supla::TURN_OFF, Supla::GUI::relay[nr], OnInvalid());

      switch (ConfigManager->get(KEY_CONDITIONS_TYPE)->getElement(nr).toInt()) {
        case CONDITION_VOLTAGE:
          client->addAction(Supla::TURN_ON, Supla::GUI::relay[nr], OnLess(threshold, EmVoltage()));
          break;
        case CONDITION_TOTAL_CURRENT:
          client->addAction(Supla::TURN_ON, Supla::GUI::relay[nr], OnLess(threshold, EmTotalCurrent()));
          break;
        case CONDITION_TOTAL_POWER_ACTIVE:
          client->addAction(Supla::TURN_ON, Supla::GUI::relay[nr], OnLess(threshold, EmTotalPowerActiveW()));
          break;

        case CONDITION_VOLTAGE_OPPOSITE:
          client->addAction(Supla::TURN_ON, Supla::GUI::relay[nr], OnGreater(threshold, EmVoltage()));
          break;
        case CONDITION_TOTAL_CURRENT_OPPOSITE:
          client->addAction(Supla::TURN_ON, Supla::GUI::relay[nr], OnGreater(threshold, EmTotalCurrent()));
          break;
        case CONDITION_TOTAL_POWER_ACTIVE_OPPOSITE:
          client->addAction(Supla::TURN_ON, Supla::GUI::relay[nr], OnGreater(threshold, EmTotalPowerActiveW()));
          break;
      }
    }
  }
#endif
}

void addConditionsTurnOFF(int function, Supla::Sensor::ElectricityMeter *client, uint8_t sensorNumber) {
#if defined(SUPLA_RELAY) && defined(SUPLA_CONDITIONS)
  for (uint8_t nr = 0; nr <= OFF_GPIO; nr++) {
    if (ConfigManager->get(KEY_CONDITIONS_SENSOR_TYPE)->getElement(nr).toInt() == function &&
        strcmp(ConfigManager->get(KEY_CONDITIONS_MAX)->getElement(nr).c_str(), "") != 0 &&
        ConfigManager->get(KEY_CONDITIONS_SENSOR_NUMBER)->getElement(nr).toInt() == sensorNumber &&
        ConfigESP->getGpio(nr, FUNCTION_RELAY) != OFF_GPIO) {
      // Serial.println("addConditionsTurnOFF - ElectricityMeter");
      // Serial.println(ConfigManager->get(KEY_CONDITIONS_SENSOR_NUMBER)->getElement(nr).toInt());
      // Serial.println(ConfigManager->get(KEY_CONDITIONS_MAX)->getElement(nr).c_str());

      double threshold = ConfigManager->get(KEY_CONDITIONS_MAX)->getElement(nr).toDouble();
      //   client->addAction(Supla::TURN_OFF, Supla::GUI::relay[nr], OnInvalid());

      switch (ConfigManager->get(KEY_CONDITIONS_TYPE)->getElement(nr).toInt()) {
        case CONDITION_VOLTAGE:
          client->addAction(Supla::TURN_OFF, Supla::GUI::relay[nr], OnGreater(threshold, EmVoltage()));
          break;
        case CONDITION_TOTAL_CURRENT:
          client->addAction(Supla::TURN_OFF, Supla::GUI::relay[nr], OnGreater(threshold, EmTotalCurrent()));
          break;
        case CONDITION_TOTAL_POWER_ACTIVE:
          client->addAction(Supla::TURN_OFF, Supla::GUI::relay[nr], OnGreater(threshold, EmTotalPowerActiveW()));
          break;

        case CONDITION_VOLTAGE_OPPOSITE:
          client->addAction(Supla::TURN_OFF, Supla::GUI::relay[nr], OnLess(threshold, EmVoltage()));
          break;
        case CONDITION_TOTAL_CURRENT_OPPOSITE:
          client->addAction(Supla::TURN_OFF, Supla::GUI::relay[nr], OnLess(threshold, EmTotalCurrent()));
          break;
        case CONDITION_TOTAL_POWER_ACTIVE_OPPOSITE:
          client->addAction(Supla::TURN_OFF, Supla::GUI::relay[nr], OnLess(threshold, EmTotalPowerActiveW()));
          break;
      }
    }
  }
#endif
}

#if defined(GUI_SENSOR_1WIRE) || defined(GUI_SENSOR_I2C) || defined(GUI_SENSOR_SPI)
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
#endif

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

    Supla::GUI::addConditionsTurnON(SENSOR_HLW8012, counterHLW8012);
    Supla::GUI::addConditionsTurnOFF(SENSOR_HLW8012, counterHLW8012);
  }
  eeprom.setStateSavePeriod(TIME_SAVE_PERIOD_IMPULSE_COUNTER_SEK * 1000);
}
#endif

#ifdef SUPLA_CSE7766
Supla::Sensor::CSE_7766 *counterCSE7766 = nullptr;

void addCSE7766(int8_t pinRX) {
  if (counterCSE7766 == NULL && pinRX != OFF_GPIO) {
    counterCSE7766 = new Supla::Sensor::CSE_7766(pinRX);

    Supla::GUI::addConditionsTurnON(SENSOR_CSE7766, counterCSE7766);
    Supla::GUI::addConditionsTurnOFF(SENSOR_CSE7766, counterCSE7766);
  }
  eeprom.setStateSavePeriod(TIME_SAVE_PERIOD_IMPULSE_COUNTER_SEK * 1000);
}
#endif

#ifdef SUPLA_MPX_5XXX
Supla::Sensor::MPX_5XXX *mpx = nullptr;
#endif

#ifdef SUPLA_ANALOG_READING_MAP
Supla::Sensor::AnalogRedingMap **analog = nullptr;
#endif
}  // namespace GUI
}  // namespace Supla

SuplaConfigManager *ConfigManager = nullptr;
SuplaConfigESP *ConfigESP = nullptr;
SuplaWebServer *WebServer = nullptr;
#ifdef SUPLA_WT32_ETH01_LAN8720
Supla::WT32_ETH01 *eth = nullptr;
#else
Supla::GUIESPWifi *wifi = nullptr;
#endif