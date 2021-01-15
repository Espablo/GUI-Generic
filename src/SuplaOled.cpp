#include "SuplaOled.h"
#include "SuplaDeviceGUI.h"

#ifdef SUPLA_OLED

uint8_t* framesCountSensor;
uint8_t* chanelSensor;

String getTempString(double temperature) {
  if (temperature == -275) {
    return F("error");
  }
  else {
    return String(temperature, 1);
  }
}

String getHumidityString(double humidity) {
  if (humidity == -1) {
    return F("error");
  }
  else {
    return String(humidity, 1);
  }
}

String getPressureString(double pressure) {
  if (pressure == -1) {
    return F("error");
  }
  else {
    return String(pressure, 0);
  }
}

uint8_t getFramesCountSensor(OLEDDisplayUiState* state) {
  if (state->frameState) {
    return framesCountSensor[state->currentFrame];
  }
  return 0;
}

int32_t readRssi(void) {
  int32_t rssi = WiFi.RSSI();
  if (WiFi.status() != WL_CONNECTED)
    return -1;
  if (rssi <= -100)
    return 0;
  if (rssi >= -50)
    return 100;
  return (2 * (rssi + 100));
}

void displayUiSignal(OLEDDisplay* display) {
  int x = display->getWidth() - 17;
  int y = 0;
  int value = readRssi();
  // clear area only
  display->setColor(BLACK);
  display->fillRect(x, y, x + 46, 16);
  display->setColor(WHITE);
  if (value == -1) {
    display->setFont(ArialMT_Plain_10);
    display->drawString(x + 1, y, "x");
  }
  else {
    if (value > 0)
      display->fillRect(x, y + 6, 3, 4);
    else
      display->drawRect(x, y + 6, 3, 4);

    if (value >= 25)
      display->fillRect(x + 4, y + 4, 3, 6);
    else
      display->drawRect(x + 4, y + 4, 3, 6);

    if (value >= 50)
      display->fillRect(x + 8, y + 2, 3, 8);
    else
      display->drawRect(x + 8, y + 2, 3, 8);

    if (value >= 75)
      display->fillRect(x + 12, y, 3, 10);
    else
      display->drawRect(x + 12, y, 3, 10);
  }
}

void displayUiRelayState(OLEDDisplay* display) {
  int y = 0;
  int x = 0;

  display->setFont(ArialMT_Plain_10);
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

void msOverlay(OLEDDisplay* display, OLEDDisplayUiState* state) {
  displayUiSignal(display);
  if (Supla::GUI::relay.size()) {
    displayUiRelayState(display);
  }
}

void displayUiSuplaStatus(OLEDDisplay* display) {
  int x = 0;
  int y = display->getHeight() / 3;
  display->clear();

  displayUiSignal(display);

  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setColor(WHITE);
  display->drawStringMaxWidth(x, y, display->getWidth(), ConfigESP->supla_status.msg);
  display->display();
}

void displayConfigMode(OLEDDisplay* display) {
  display->clear();
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setColor(WHITE);
  display->drawString(0, 15, F("Tryb konfiguracyjny"));
  display->drawString(0, 28, F("AP name:"));
  display->drawString(0, 41, ConfigESP->getConfigNameAP());
  display->drawString(0, 54, F("IP: 192.168.4.1"));
  display->display();
}

void displayUiBlank(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // display->drawXbm(10, 17, supla_logo_width, supla_logo_height, supla_logo_bits);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_16);
  display->drawString(10, display->getHeight() / 2, F("SUPLA"));
}

void displayUiTemperature(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y, double temp, const String& name) {
  uint8_t temp_width, temp_height;

  int drawHeightIcon = display->getHeight() / 2 - 10;
  int drawStringIcon = display->getHeight() / 2 - 6;

  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);

  if (display->getWidth() <= 64 || display->getHeight() <= 48) {
    temp_width = 0;
    temp_height = 0;
  }
  else {
    display->drawXbm(x + 0, y + drawHeightIcon, TEMP_WIDTH, TEMP_HEIGHT, temp_bits);
    display->setFont(ArialMT_Plain_10);
    if (name != NULL) {
      display->drawString(x + TEMP_WIDTH + 20, y + display->getHeight() / 2 - 15, name);
    }

    temp_width = TEMP_WIDTH + 10;
    temp_height = TEMP_HEIGHT;
  }

  display->setFont(ArialMT_Plain_24);
  display->drawString(x + temp_width, y + drawStringIcon, getTempString(temp));
  display->setFont(ArialMT_Plain_16);
  display->drawString(x + temp_width + (getTempString(temp).length() * 12), y + drawStringIcon, "ºC");
}

void displaUiHumidity(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y, double humidity) {
  uint8_t humidity_width, humidity_height;

  int drawHeightIcon = display->getHeight() / 2 - 10;
  int drawStringIcon = display->getHeight() / 2 - 6;

  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);

  if (display->getWidth() <= 64 || display->getHeight() <= 48) {
    humidity_width = 0;
    humidity_height = 0;
  }
  else {
    display->drawXbm(x + 0, y + drawHeightIcon, HUMIDITY_WIDTH, HUMIDITY_HEIGHT, humidity_bits);
    humidity_width = HUMIDITY_WIDTH + 20;
    humidity_height = HUMIDITY_HEIGHT;
  }

  display->setFont(ArialMT_Plain_24);
  display->drawString(x + humidity_width, y + drawStringIcon, getHumidityString(humidity));
  display->setFont(ArialMT_Plain_16);
  display->drawString(x + humidity_width + (getHumidityString(humidity).length() * 12), y + drawStringIcon, "%");
}

void displayUiPressure(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y, double pressure) {
  uint8_t pressure_width, pressure_height;

  int drawHeightIcon = display->getHeight() / 2 - 10;
  int drawStringIcon = display->getHeight() / 2 - 6;

  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);

  if (display->getWidth() <= 64 || display->getHeight() <= 48) {
    pressure_width = 0;
    pressure_height = 0;
  }
  else {
    display->drawXbm(x + 0, y + drawHeightIcon, PRESSURE_WIDTH, PRESSURE_HEIGHT, pressure_bits);
    pressure_width = PRESSURE_WIDTH + 15;
    pressure_height = PRESSURE_HEIGHT;
  }

  display->setFont(ArialMT_Plain_24);
  display->drawString(x + pressure_width, y + drawStringIcon, getPressureString(pressure));
  display->setFont(ArialMT_Plain_10);
  display->drawString(x + pressure_width + (getPressureString(pressure).length() * 14), y + drawStringIcon, "hPa");
}

void displayTemperature(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();
      if (channel->getChannelNumber() == chanelSensor[state->currentFrame]) {
        double lastTemperature = channel->getValueDouble();

        if (ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt() >= state->currentFrame) {
          displayUiTemperature(display, state, x, y, lastTemperature, ConfigManager->get(KEY_DS_NAME + state->currentFrame)->getValue());
        }
        else {
          displayUiTemperature(display, state, x, y, lastTemperature);
        }
      }
    }
  }
}

void displayDoubleTemperature(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();
      if (channel->getChannelNumber() == chanelSensor[state->currentFrame]) {
        double lastTemperature = channel->getValueDoubleFirst();
        displayUiTemperature(display, state, x, y, lastTemperature);
      }
    }
  }
}

void displayDoubleHumidity(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();
      if (channel->getChannelNumber() == chanelSensor[state->currentFrame]) {
        double lastHumidit = channel->getValueDoubleSecond();
        displaUiHumidity(display, state, x, y, lastHumidit);
      }
    }
  }
}

void displayPressure(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getSecondaryChannel()) {
      auto channel = element->getSecondaryChannel();
      if (channel->getChannelNumber() == chanelSensor[state->currentFrame]) {
        double lastPressure = channel->getValueDouble();
        displayUiPressure(display, state, x, y, lastPressure);
      }
    }
  }
}

SuplaOled::SuplaOled() {
  if (ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO) {
    switch (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_OLED).toInt()) {
      case OLED_SSD1306_0_96:
        display = new SSD1306Wire(0x3c, ConfigESP->getGpio(FUNCTION_SDA), ConfigESP->getGpio(FUNCTION_SCL));
        break;
      case OLED_SH1106_1_3:
        display = new SH1106Wire(0x3c, ConfigESP->getGpio(FUNCTION_SDA), ConfigESP->getGpio(FUNCTION_SCL));
        break;
      case OLED_SSD1306_0_66:
        display = new SSD1306Wire(0x3c, ConfigESP->getGpio(FUNCTION_SDA), ConfigESP->getGpio(FUNCTION_SCL), GEOMETRY_64_48);
        break;
    }

    ui = new OLEDDisplayUi(display);

    overlays[0] = {msOverlay};
    int maxFrame = getMaxFrame();

    frames = (FrameCallback*)malloc(sizeof(FrameCallback) * maxFrame);
    chanelSensor = (uint8_t*)malloc(sizeof(uint8_t) * maxFrame);

    for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
      if (element->getChannel()) {
        auto channel = element->getChannel();
        Serial.println(channel->getChannelType());

        if (channel->getChannelType() == SUPLA_CHANNELTYPE_THERMOMETER) {
          frames[frameCount] = {displayTemperature};
          chanelSensor[frameCount] = channel->getChannelNumber();
          frameCount += 1;
        }

        if (channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {
          frames[frameCount] = {displayDoubleTemperature};
          chanelSensor[frameCount] = channel->getChannelNumber();
          frameCount += 1;
          frames[frameCount] = {displayDoubleHumidity};
          chanelSensor[frameCount] = channel->getChannelNumber();
          frameCount += 1;
        }
      }
      if (element->getSecondaryChannel()) {
        auto channel = element->getSecondaryChannel();
        if (channel->getChannelType() == SUPLA_CHANNELTYPE_PRESSURESENSOR) {
          frames[frameCount] = {displayPressure};
          chanelSensor[frameCount] = channel->getChannelNumber();
          frameCount += 1;
        }
      }
    }

    if (frameCount == 0) {
      frames[frameCount] = {displayUiBlank};
      frameCount += 1;
    }

    if (frameCount == 1) {
      ui->disableAllIndicators();
      ui->disableAutoTransition();
    }
    else {
      ui->setIndicatorPosition(BOTTOM);
      ui->setIndicatorDirection(LEFT_RIGHT);
      ui->setFrameAnimation(SLIDE_LEFT);
    }

    ui->setTargetFPS(60);
    ui->setFrames(frames, frameCount);
    ui->setOverlays(overlays, overlaysCount);
    ui->init();
    display->flipScreenVertically();
  }
}

void SuplaOled::iterateAlways() {
  if (ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO) {
    if (ConfigESP->supla_status.status != STATUS_REGISTERED_AND_READY) {
      displayUiSuplaStatus(display);
      return;
    }

    if (millis() - timeLastChangeOled > 30000 && oledON) {
      display->setBrightness(50);
      oledON = false;
      // display.displayOff();
    }

    if (ConfigESP->configModeESP == NORMAL_MODE) {
      int remainingTimeBudget = ui->update();

      if (remainingTimeBudget > 0)
        delay(remainingTimeBudget);
    }
    else {
      displayConfigMode(display);
    }
  }
}

void SuplaOled::addButtonOled(int pin) {
  if (pin != OFF_GPIO) {
    Supla::Control::Button* button = new Supla::Control::Button(pin, true, true);
    button->addAction(TURN_ON_OLED, this, Supla::ON_PRESS);
  }
}

void SuplaOled::runAction(int event, int action) {
  if (action == TURN_ON_OLED) {
    display->setBrightness(255);
    timeLastChangeOled = millis();
    oledON = true;
  }
}

int SuplaOled::getMaxFrame() {
  int maxFrame = 0;
  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();
      Serial.println(channel->getChannelType());

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_THERMOMETER) {
        maxFrame += 1;
      }

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {
        maxFrame += 2;
      }
    }

    if (element->getSecondaryChannel()) {
      auto channel = element->getSecondaryChannel();
      if (channel->getChannelType() == SUPLA_CHANNELTYPE_PRESSURESENSOR) {
        maxFrame += 1;
      }
    }
  }

  if (maxFrame == 0)
    maxFrame = 1;

  return maxFrame;
}
#endif