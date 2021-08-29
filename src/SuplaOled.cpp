#include "SuplaOled.h"
#include "SuplaDeviceGUI.h"

#ifdef SUPLA_OLED

struct oledStruct {
  uint8_t chanelSensor;
  bool forSecondaryValue;
};

oledStruct* oled;

String getTempString(double temperature) {
  if (temperature == -275) {
    return S_ERROR;
  }
  else {
    return String(temperature, 1);
  }
}

String getHumidityString(double humidity) {
  if (humidity == -1) {
    return S_ERROR;
  }
  else {
    return String(humidity, 1);
  }
}

String getPressureString(double pressure) {
  if (pressure == -1) {
    return S_ERROR;
  }
  else {
    return String(pressure, 0);
  }
}

int getQuality() {
  if (WiFi.status() != WL_CONNECTED)
    return -1;
  int dBm = WiFi.RSSI();
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
  for (int i = 0; i < Supla::GUI::relay.size(); i++) {
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
  display->setColor(WHITE);
  display->drawHorizontalLine(0, 14, display->getWidth());
}
#endif

void msOverlay(OLEDDisplay* display, OLEDDisplayUiState* state) {
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

void displayConfigMode(OLEDDisplay* display) {
  display->clear();
  display->setFont(ArialMT_Win1250_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setColor(WHITE);
  display->drawString(0, 15, S_CONFIGURATION_MODE);
  display->drawString(0, 28, S_AP_NAME);
  display->drawString(0, 41, ConfigESP->getConfigNameAP());
  display->drawString(0, 54, F("IP: 192.168.4.1"));
  display->display();
}

void displayUiBlank(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // display->drawXbm(10, 17, supla_logo_width, supla_logo_height, supla_logo_bits);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Win1250_Plain_16);
  display->drawString(10, display->getHeight() / 2, F("SUPLA"));
}

void displayUiTemperature(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y, double temp, const String& name) {
  uint8_t temp_width;

  int drawHeightIcon = display->getHeight() / 2 - 10;
  int drawStringIcon = display->getHeight() / 2 - 6;

  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);

  if (display->getWidth() <= 64 || display->getHeight() <= 48) {
    temp_width = 0;
  }
  else {
    display->drawXbm(x + 0, y + drawHeightIcon, TEMP_WIDTH, TEMP_HEIGHT, temp_bits);

    temp_width = TEMP_WIDTH + 10;
  }

  if (name != NULL) {
    display->setFont(ArialMT_Win1250_Plain_10);
    display->drawString(x + TEMP_WIDTH + 20, y + display->getHeight() / 2 - 15, name);
  }

  display->setFont(ArialMT_Win1250_Plain_24);
  display->drawString(x + temp_width, y + drawStringIcon, getTempString(temp));
  display->setFont(ArialMT_Win1250_Plain_16);
  display->drawString(x + temp_width + (getTempString(temp).length() * 12), y + drawStringIcon, "°C");
}

void displaUiHumidity(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y, double humidity, const String& name) {
  uint8_t humidity_width;

  int drawHeightIcon = display->getHeight() / 2 - 10;
  int drawStringIcon = display->getHeight() / 2 - 6;

  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);

  if (display->getWidth() <= 64 || display->getHeight() <= 48) {
    humidity_width = 0;
  }
  else {
    display->drawXbm(x + 0, y + drawHeightIcon, HUMIDITY_WIDTH, HUMIDITY_HEIGHT, humidity_bits);
    humidity_width = HUMIDITY_WIDTH + 20;
  }

  if (name != NULL) {
    display->setFont(ArialMT_Win1250_Plain_10);
    display->drawString(x + TEMP_WIDTH + 20, y + display->getHeight() / 2 - 15, name);
  }

  display->setFont(ArialMT_Win1250_Plain_24);
  display->drawString(x + humidity_width, y + drawStringIcon, getHumidityString(humidity));
  display->setFont(ArialMT_Win1250_Plain_16);
  display->drawString(x + humidity_width + (getHumidityString(humidity).length() * 12), y + drawStringIcon, "%");
}

void displayUiPressure(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y, double pressure, const String& name) {
  uint8_t pressure_width;

  int drawHeightIcon = display->getHeight() / 2 - 10;
  int drawStringIcon = display->getHeight() / 2 - 6;

  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);

  if (display->getWidth() <= 64 || display->getHeight() <= 48) {
    pressure_width = 0;
  }
  else {
    display->drawXbm(x + 0, y + drawHeightIcon, PRESSURE_WIDTH, PRESSURE_HEIGHT, pressure_bits);
    pressure_width = PRESSURE_WIDTH + 10;
  }

  if (name != NULL) {
    display->setFont(ArialMT_Win1250_Plain_10);
    display->drawString(x + TEMP_WIDTH + 20, y + display->getHeight() / 2 - 15, name);
  }

  display->setFont(ArialMT_Win1250_Plain_24);
  display->drawString(x + pressure_width, y + drawStringIcon, getPressureString(pressure));
  display->setFont(ArialMT_Win1250_Plain_16);
  display->drawString(x + pressure_width + (getPressureString(pressure).length() * 14), y + drawStringIcon, "hPa");
}

void displayUiGeneral(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y, double value, const String& name) {
  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_CENTER);

  if (name != NULL) {
    display->setFont(ArialMT_Win1250_Plain_10);
    display->drawString(x + ((display->getWidth() - String(name).length()) / 2), y + display->getHeight() / 2 - 15, name);
  }

  display->setFont(ArialMT_Win1250_Plain_24);
  display->drawString(x + ((display->getWidth() - String(value).length()) / 2), y + display->getHeight() / 2, String(value));
}

void displayTemperature(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();
      if (channel->getChannelNumber() == oled[state->currentFrame].chanelSensor) {
        double lastTemperature = channel->getValueDouble();
        String name = ConfigManager->get(KEY_NAME_SENSOR)->getElement(state->currentFrame);
        displayUiTemperature(display, state, x, y, lastTemperature, name);
      }
    }
  }
}

void displayDoubleTemperature(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();
      if (channel->getChannelNumber() == oled[state->currentFrame].chanelSensor) {
        double lastTemperature = channel->getValueDoubleFirst();
        String name = ConfigManager->get(KEY_NAME_SENSOR)->getElement(state->currentFrame);
        displayUiTemperature(display, state, x, y, lastTemperature, name);
      }
    }
  }
}

void displayDoubleHumidity(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();
      if (channel->getChannelNumber() == oled[state->currentFrame].chanelSensor) {
        double lastHumidit = channel->getValueDoubleSecond();
        String name = ConfigManager->get(KEY_NAME_SENSOR)->getElement(state->currentFrame);
        displaUiHumidity(display, state, x, y, lastHumidit, name);
      }
    }
  }
}

void displayPressure(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getSecondaryChannel()) {
      auto channel = element->getSecondaryChannel();
      if (channel->getChannelNumber() == oled[state->currentFrame].chanelSensor) {
        double lastPressure = channel->getValueDouble();
        String name = ConfigManager->get(KEY_NAME_SENSOR)->getElement(state->currentFrame);
        displayUiPressure(display, state, x, y, lastPressure, name);
      }
    }
  }
}

void displayGeneral(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  double lastValue;
  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();
      if (channel->getChannelNumber() == oled[state->currentFrame].chanelSensor) {
        if (oled[state->currentFrame].forSecondaryValue) {
          lastValue = channel->getValueDoubleSecond();
        }
        else {
          lastValue = channel->getValueDouble();
        }

        String name = ConfigManager->get(KEY_NAME_SENSOR)->getElement(state->currentFrame);

        displayUiGeneral(display, state, x, y, lastValue, name);
      }
    }
  }
}

SuplaOled::SuplaOled() {
  if (ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO) {
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
          frames[frameCount] = {displayGeneral};
          oled[frameCount].chanelSensor = channel->getChannelNumber();
          oled[frameCount].forSecondaryValue = false;
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
    if (ConfigESP->configModeESP == CONFIG_MODE) {
      displayConfigMode(display);
      return;
    }

    if (ConfigESP->getLastStatusSupla() == STATUS_REGISTERED_AND_READY || ConfigESP->getLastStatusSupla() == STATUS_NETWORK_DISCONNECTED || ConfigESP->getLastStatusSupla() == STATUS_INITIALIZED) {
      // setupAnimate();

      if (millis() - timeLastChangeOled > (ConfigManager->get(KEY_OLED_BACK_LIGHT_TIME)->getValueInt() * 1000) && oledON &&
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
      button->addAction(NEXT_FRAME, this, Supla::ON_PRESS);
    }

    button->addAction(TURN_ON_OLED, this, Supla::ON_PRESS);
  }
}

void SuplaOled::handleAction(int event, int action) {
  if (action == NEXT_FRAME && oledON) {
    ui->nextFrame();
  }

  if (action == TURN_ON_OLED && oledON == false) {
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
