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

#include "SuplaOled.h"
#include "SuplaDeviceGUI.h"
#include <supla/clock/clock.h>
#include <supla/sensor/therm_hygro_press_meter.h>
#include <supla/sensor/distance.h>

#ifdef SUPLA_OLED

struct oledStruct {
  uint8_t chanelSensor;
  bool forSecondaryValue;
};

oledStruct* oled;

String getTempString(double temperature) {
  if (temperature == TEMPERATURE_NOT_AVAILABLE) {
    return S_ERROR;
  }
  else {
    return String(temperature, 1);
  }
}

String getHumidityString(double humidity) {
  if (humidity == HUMIDITY_NOT_AVAILABLE) {
    return S_ERROR;
  }
  else {
    return String(humidity, 1);
  }
}

String getPressureString(double pressure) {
  if (pressure == PRESSURE_NOT_AVAILABLE) {
    return S_ERROR;
  }
  else {
    return String(pressure, 0);
  }
}

String getDistanceString(double distance) {
  if (distance == DISTANCE_NOT_AVAILABLE) {
    return S_ERROR;
  }
  else {
    return String(distance, 2).c_str();
  }
}

int getWidthUnit(OLEDDisplay* display, const String& value) {
  if (value == S_ERROR) {
    return (display->getWidth() / 2) + (display->getStringWidth(value) / 4.5);
  }

  return (display->getWidth() / 2) + (display->getStringWidth(value) / 2);
}

int getWidthValue(OLEDDisplay* display, const String& value) {
  if (value == S_ERROR) {
    return (display->getWidth() / 2) - (display->getStringWidth(value) / 5);
  }

  return (display->getWidth() / 2) - (display->getStringWidth(value) / 2);
}

int getQuality() {
  if (WiFi.status() != WL_CONNECTED)
    return -1;
#ifdef ARDUINO_ARCH_ESP32
  int dBm = 100;
  // WiFi.RSSI() dla ESP32 zawiesza całą pętlę
#else
  int dBm = WiFi.RSSI();
#endif
  if (dBm <= -100)
    return 0;
  if (dBm >= -50)
    return 100;
  return 2 * (dBm + 100);
}

void displayUiSignal(OLEDDisplay* display) {
  int x = display->getWidth() - 17;
  int y = 0;
  int value = getQuality();

  display->setColor(BLACK);
  display->fillRect(x, y, x + 46, 16);
  display->setColor(WHITE);
  if (value == -1) {
    display->setFont(ArialMT_Win1250_Plain_10);
    display->drawString(x + 1, y, "x");
  }
  else {
    if (value > 0)
      display->fillRect(x, y + 6, 3, 4);

    if (value >= 25)
      display->fillRect(x + 4, y + 4, 3, 6);

    if (value >= 50)
      display->fillRect(x + 8, y + 2, 3, 8);

    if (value >= 75)
      display->fillRect(x + 12, y, 3, 10);
  }
}

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
void displayUiRelayState(OLEDDisplay* display) {
  int y = 0;
  int x = 0;

  display->setFont(ArialMT_Win1250_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  for (size_t i = 0; i < Supla::GUI::relay.size(); i++) {
    if (Supla::GUI::relay[i] != nullptr) {
      if (Supla::GUI::relay[i]->isOn()) {
        display->setColor(WHITE);
        display->fillRect(x, y + 1, 10, 10);
        display->setColor(BLACK);
        display->drawString(x + 2, y, String(i + 1));
      }
      else {
        display->setColor(WHITE);
        display->drawString(x + 2, y, String(i + 1));
      }
      x += 15;
    }
  }
  display->setColor(WHITE);
  display->drawHorizontalLine(0, 14, display->getWidth());
}
#endif

void msOverlay(OLEDDisplay* display, OLEDDisplayUiState* state) {
  displayUiSuplaClock(display);
  displayUiSignal(display);

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
  if (Supla::GUI::relay.size()) {
    displayUiRelayState(display);
  }
#endif
}

void displayUiSuplaStatus(OLEDDisplay* display) {
  int x = 0;
  int y = display->getHeight() / 3;
  display->clear();

  displayUiSignal(display);

  display->setFont(ArialMT_Win1250_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setColor(WHITE);
  display->drawStringMaxWidth(x, y, display->getWidth(), ConfigESP->supla_status.msg);
  display->display();
}

void displayUiSuplaClock(OLEDDisplay* display) {
  char clockBuff[6];
  auto suplaClock = SuplaDevice.getClock();

  if (suplaClock->isReady()) {
    sprintf_P(clockBuff, PSTR("%02d:%02d"), suplaClock->getHour(), suplaClock->getMin());
    display->setColor(WHITE);
    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->drawString(0, display->getHeight() - 10, String(clockBuff));
  }
}

void displayConfigMode(OLEDDisplay* display) {
  display->clear();
  display->setFont(ArialMT_Win1250_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setColor(WHITE);
  display->drawString(0, 15, S_CONFIGURATION_MODE);
  display->drawString(0, 28, S_AP_NAME);
  display->drawString(0, 41, ConfigESP->getConfigNameAP());
  display->drawString(0, 54, S_IP_AP);
  display->display();
}

void displayUiBlank(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // display->drawXbm(10, 17, supla_logo_width, supla_logo_height, supla_logo_bits);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Win1250_Plain_16);
  display->drawString(10, display->getHeight() / 2, F("SUPLA"));
}

void displayUiGeneral(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y, double value, const String& unit, const uint8_t* xbm) {
  displayUiGeneral(display, state, x, y, String(value), unit, xbm);
}

void displayUiGeneral(
    OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y, const String& value, const String& unit, const uint8_t* xbm) {
  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);

  if (xbm != NULL && display->getWidth() > 64) {
    int drawHeightIcon = display->getHeight() / 2 - 10;
    display->drawXbm(x + 0, y + drawHeightIcon, 32, 32, xbm);
  }

  String name = ConfigManager->get(KEY_NAME_SENSOR)->getElement(state->currentFrame);
  if (!name.isEmpty()) {
    display->setFont(ArialMT_Win1250_Plain_10);
    display->drawString(x + getWidthValue(display, name), y + display->getHeight() / 2 - 12, name);
  }

  display->setFont(ArialMT_Win1250_Plain_24);
  display->drawString(x + getWidthValue(display, value), y + display->getHeight() / 2 - 2, value);

  if (!unit.isEmpty()) {
    uint8_t widthUnit = getWidthUnit(display, value);

    display->setFont(ArialMT_Win1250_Plain_16);
    display->drawString(x + widthUnit, y + display->getHeight() / 2 + 5, unit);
  }
}

void displayTemperature(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  auto channel = Supla::Element::getElementByChannelNumber(oled[state->currentFrame].chanelSensor)->getChannel();
  double lastTemperature = channel->getValueDouble();
  displayUiGeneral(display, state, x, y, getTempString(lastTemperature), "°C", temp_bits);
}

void displayDoubleTemperature(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  auto channel = Supla::Element::getElementByChannelNumber(oled[state->currentFrame].chanelSensor)->getChannel();
  double lastTemperature = channel->getValueDoubleFirst();

  displayUiGeneral(display, state, x, y, getTempString(lastTemperature), "°C", temp_bits);
}

void displayDoubleHumidity(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  auto channel = Supla::Element::getElementByChannelNumber(oled[state->currentFrame].chanelSensor)->getChannel();
  double lastHumidit = channel->getValueDoubleSecond();

  displayUiGeneral(display, state, x, y, getHumidityString(lastHumidit), "%", humidity_bits);
}

void displayPressure(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  auto channel = Supla::Element::getElementByChannelNumber(oled[state->currentFrame].chanelSensor)->getChannel();
  double lastPressure = lastPressure = channel->getValueDouble();

  displayUiGeneral(display, state, x, y, getPressureString(lastPressure), "hPa", pressure_bits);
}

void displayDistance(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  auto channel = Supla::Element::getElementByChannelNumber(oled[state->currentFrame].chanelSensor)->getChannel();
  double distance = channel->getValueDouble();

  displayUiGeneral(display, state, x, y, getDistanceString(distance), "m", distance_bits);
}

void displayGeneral(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  auto channel = Supla::Element::getElementByChannelNumber(oled[state->currentFrame].chanelSensor)->getChannel();

  if (oled[state->currentFrame].forSecondaryValue) {
    displayUiGeneral(display, state, x, y, channel->getValueDoubleSecond());
  }
  else {
    displayUiGeneral(display, state, x, y, channel->getValueDouble());
    channel->getValueDouble();
  }
}

void displayEnergyVoltage(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  auto channel = Supla::Element::getElementByChannelNumber(oled[state->currentFrame].chanelSensor)->getChannel();

  TSuplaChannelExtendedValue* extValue = channel->getExtValue();
  if (extValue == nullptr)
    return;

  TElectricityMeter_ExtendedValue_V2* emValue = reinterpret_cast<TElectricityMeter_ExtendedValue_V2*>(extValue->value);
  if (emValue->m_count < 1 || emValue == nullptr)
    return;

  displayUiGeneral(display, state, x, y, String(emValue->m[0].voltage[0] / 100.0, 1), "V");
}

void displayEnergyCurrent(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  auto channel = Supla::Element::getElementByChannelNumber(oled[state->currentFrame].chanelSensor)->getChannel();

  TSuplaChannelExtendedValue* extValue = channel->getExtValue();
  if (extValue == nullptr)
    return;

  TElectricityMeter_ExtendedValue_V2* emValue = reinterpret_cast<TElectricityMeter_ExtendedValue_V2*>(extValue->value);
  if (emValue->m_count < 1 || emValue == nullptr)
    return;

  displayUiGeneral(display, state, x, y, emValue->m[0].current[0] / 1000.0, "A");
}

void displayEnergyPowerActive(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  auto channel = Supla::Element::getElementByChannelNumber(oled[state->currentFrame].chanelSensor)->getChannel();
  TSuplaChannelExtendedValue* extValue = channel->getExtValue();
  if (extValue == nullptr)
    return;

  TElectricityMeter_ExtendedValue_V2* emValue = reinterpret_cast<TElectricityMeter_ExtendedValue_V2*>(extValue->value);
  if (emValue->m_count < 1 || emValue == nullptr)
    return;

  displayUiGeneral(display, state, x, y, String(emValue->m[0].power_active[0] / 100000.0, 1), "W");
}

SuplaOled::SuplaOled() {
  if (ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO) {
    SuplaDevice.addClock(new Supla::Clock);

    switch (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_OLED).toInt()) {
      case OLED_SSD1306_0_96:
        display = new SSD1306Wire(0x3c, ConfigESP->getGpio(FUNCTION_SDA), ConfigESP->getGpio(FUNCTION_SCL), GEOMETRY_128_64);
        break;
      case OLED_SH1106_1_3:
        display = new SH1106Wire(0x3c, ConfigESP->getGpio(FUNCTION_SDA), ConfigESP->getGpio(FUNCTION_SCL), GEOMETRY_128_64);
        break;
      case OLED_SSD1306_0_66:
        display = new SSD1306Wire(0x3c, ConfigESP->getGpio(FUNCTION_SDA), ConfigESP->getGpio(FUNCTION_SCL), GEOMETRY_64_48);
        break;
    }

    ui = new OLEDDisplayUi(display);

    overlays[0] = {msOverlay};
    int maxFrame = getCountSensorChannels();

    if (maxFrame == 0) {
      maxFrame = 1;
    }

    frames = new FrameCallback[maxFrame];
    oled = new oledStruct[maxFrame];

    for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
      if (element->getChannel()) {
        auto channel = element->getChannel();

        if (channel->getChannelType() == SUPLA_CHANNELTYPE_THERMOMETER) {
          frames[frameCount] = {displayTemperature};
          oled[frameCount].chanelSensor = channel->getChannelNumber();
          frameCount += 1;
        }

        if (channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {
          frames[frameCount] = {displayDoubleTemperature};
          oled[frameCount].chanelSensor = channel->getChannelNumber();
          frameCount += 1;
          frames[frameCount] = {displayDoubleHumidity};
          oled[frameCount].chanelSensor = channel->getChannelNumber();
          frameCount += 1;
        }

        if (channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYSENSOR) {
          frames[frameCount] = {displayGeneral};
          oled[frameCount].chanelSensor = channel->getChannelNumber();
          oled[frameCount].forSecondaryValue = true;
          frameCount += 1;
        }

        if (channel->getChannelType() == SUPLA_CHANNELTYPE_DISTANCESENSOR) {
          frames[frameCount] = {displayDistance};
          oled[frameCount].chanelSensor = channel->getChannelNumber();
          oled[frameCount].forSecondaryValue = false;
          frameCount += 1;
        }

        if (channel->getChannelType() == SUPLA_CHANNELTYPE_ELECTRICITY_METER) {
          frames[frameCount] = {displayEnergyVoltage};
          oled[frameCount].chanelSensor = channel->getChannelNumber();
          oled[frameCount].forSecondaryValue = false;
          frameCount += 1;

          frames[frameCount] = {displayEnergyCurrent};
          oled[frameCount].chanelSensor = channel->getChannelNumber();
          oled[frameCount].forSecondaryValue = false;
          frameCount += 1;

          frames[frameCount] = {displayEnergyPowerActive};
          oled[frameCount].chanelSensor = channel->getChannelNumber();
          oled[frameCount].forSecondaryValue = false;
          frameCount += 1;
        }
        if (channel->getChannelType() == SUPLA_CHANNELTYPE_PRESSURESENSOR) {
          frames[frameCount] = {displayPressure};
          oled[frameCount].chanelSensor = channel->getChannelNumber();
          frameCount += 1;
        }
      }
      if (element->getSecondaryChannel()) {
        auto channel = element->getSecondaryChannel();
        if (channel->getChannelType() == SUPLA_CHANNELTYPE_PRESSURESENSOR) {
          frames[frameCount] = {displayPressure};
          oled[frameCount].chanelSensor = channel->getChannelNumber();
          frameCount += 1;
        }
      }
    }

    if (frameCount == 0) {
      frames[frameCount] = {displayUiBlank};
      frameCount += 1;
    }

    setupAnimate();

    ui->setTargetFPS(60);
    ui->setIndicatorPosition(BOTTOM);
    ui->setIndicatorDirection(LEFT_RIGHT);
    ui->setFrameAnimation(SLIDE_LEFT);

    ui->setFrames(frames, frameCount);
    ui->setOverlays(overlays, overlaysCount);
    ui->init();

    if (ConfigManager->get(KEY_OLED_BACK_LIGHT_TIME)->getValueInt() != 0) {
      display->setBrightness(255);
      timeLastChangeOled = millis();
    }
    else {
      display->setBrightness((ConfigManager->get(KEY_OLED_BACK_LIGHT)->getValueInt() / 100.0) * 255);
    }

    display->flipScreenVertically();
    display->setFontTableLookupFunction(&utf8win1250);
  }
}

void SuplaOled::setupAnimate() {
  if (frameCount == 1) {
    ui->disableAllIndicators();
    ui->disableAutoTransition();
  }
  else {
    if (ConfigManager->get(KEY_OLED_ANIMATION)->getValueInt() > 0) {
      ui->enableAutoTransition();
      ui->setTimePerFrame(ConfigManager->get(KEY_OLED_ANIMATION)->getValueInt() * 1000);
    }
    else {
      ui->disableAutoTransition();
      ui->setTimePerTransition(250);
    }
  }
}

void SuplaOled::iterateAlways() {
  if (ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO) {
    if (ConfigESP->configModeESP == Supla::DEVICE_MODE_CONFIG) {
      displayConfigMode(display);
      return;
    }

    if (ConfigESP->getLastStatusSupla() == STATUS_REGISTERED_AND_READY || ConfigESP->getLastStatusSupla() == STATUS_NETWORK_DISCONNECTED ||
        ConfigESP->getLastStatusSupla() == STATUS_INITIALIZED) {
      // setupAnimate();

      if (millis() - timeLastChangeOled > (unsigned long)(ConfigManager->get(KEY_OLED_BACK_LIGHT_TIME)->getValueInt() * 1000) && oledON &&
          ConfigManager->get(KEY_OLED_BACK_LIGHT_TIME)->getValueInt() != 0) {
        display->setBrightness((ConfigManager->get(KEY_OLED_BACK_LIGHT)->getValueInt() / 100.0) * 255);
        oledON = false;
      }
      int remainingTimeBudget = ui->update();

      if (remainingTimeBudget > 0)
        delay(remainingTimeBudget);
    }
    else {
      displayUiSuplaStatus(display);
    }
  }
}

void SuplaOled::addButtonOled(uint8_t pin) {
  if (pin != OFF_GPIO) {
    bool pullUp = true, invertLogic = true;

    if (ConfigESP->getGpio(FUNCTION_BUTTON) != OFF_GPIO) {
      pullUp = ConfigESP->getPullUp(ConfigESP->getGpio(FUNCTION_BUTTON));
      invertLogic = ConfigESP->getInversed(ConfigESP->getGpio(FUNCTION_BUTTON));
    }

    Supla::Control::Button* button = new Supla::Control::Button(pin, pullUp, invertLogic);

    if (frameCount > 1) {
      button->addAction(OLED_NEXT_FRAME, this, Supla::ON_PRESS);
    }

    button->addAction(OLED_TURN_ON, this, Supla::ON_PRESS);
  }
}

void SuplaOled::handleAction(int event, int action) {
  if (action == OLED_NEXT_FRAME && oledON) {
    ui->nextFrame();
  }

  if (action == OLED_TURN_ON && oledON == false) {
    if (ConfigManager->get(KEY_OLED_BACK_LIGHT_TIME)->getValueInt() != 0) {
      display->setBrightness(255);
      timeLastChangeOled = millis();
    }
    oledON = true;
  }
}

// In ESP8266 Arduino core v2.3.0 missing bsearch: https://github.com/esp8266/Arduino/issues/2314
// Part of GNU C Library
void* gnu_c_bsearch(const void* key, const void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*)) {
  size_t l, u, idx;
  const void* p;
  int comparison;

  l = 0;
  u = nmemb;
  while (l < u) {
    idx = (l + u) / 2;

    p = (void*)(((const char*)base) + (idx * size));
    comparison = (*compar)(key, p);
    if (comparison < 0)
      u = idx;
    else if (comparison > 0)
      l = idx + 1;
    else
      return (void*)p;
  }

  return NULL;
}

// compare function for bsearch
int charset_table_cmp(const void* p_key, const void* p_item) {
  const uint16_t key = *(uint16_t*)p_key;
  const uint16_t item = (*((char*)p_item)) << 8 | *(((char*)p_item) + 1);

  if (key < item)
    return -1;
  else if (key == item)
    return 0;
  else
    return 1;
}

// convert utf-8 character to windows-1250
// if utf-8 char continue with next byte, returns 0, otherwise windows-1250 char
// for unconvertable char returns 0
char utf8win1250(const uint8_t ch) {
  static uint16_t uChar;
  static uint8_t len;
  char* found;

  if ((ch & 0x80) == 0x00) {
    uChar = len = 0;
    return ch;
  }
  else if ((ch & 0xE0) == 0xC0) {
    uChar = ch & 0x1F;
    len = 1;
    return 0;
  }
  else if ((ch & 0xF0) == 0xE0) {
    uChar = ch & 0x0F;
    len = 2;
    return 0;
  }
  else if ((ch & 0xF8) == 0xF0) {
    uChar = ch & 0x07;
    len = 3;
    return 0;
  }
  else if ((ch & 0xC0) == 0x80 && len > 0) {
    uChar = (uChar << 6) | (ch & 0x7F);
    len--;
    if (len > 0)
      return 0;
  }
  else {
    uChar = len = 0;
    return 0;
  }

  found = (char*)gnu_c_bsearch(&uChar, utf8_win1250_table + 1, utf8_win1250_table[0], 3 * sizeof(char), charset_table_cmp);

  if (found != NULL) {
    uChar = len = 0;
    return *(found + 2);  // return win1250 char at 3rd position;
  }

  return 0;
}

#endif
