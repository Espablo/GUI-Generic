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
#include "SuplaLCD.h"

#ifdef SUPLA_LCD_HD44780
SuplaLCD::SuplaLCD(uint8_t lcdAddr, uint8_t lcdCols, uint8_t lcdRows) {
  int frameCount = 0;
  this->lcdCols = lcdCols;

  lcd = new LiquidCrystal_I2C(lcdAddr, lcdCols, lcdRows);

  lcd->init();
  lcd->backlight();

  sensorCount = getCountSensorChannels();

  if (sensorCount != 0) {
    screenMax = (sensorCount - 1) / lcdRows;
    lcdElement = new LCDElement[sensorCount];
  }

  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();

      switch (channel->getChannelType()) {
        case SUPLA_CHANNELTYPE_DISTANCESENSOR:
        case SUPLA_CHANNELTYPE_THERMOMETER:
          lcdElement[frameCount].chanelSensor = channel->getChannelNumber();
          lcdElement[frameCount].screenNumbers = frameCount / lcdRows;
          frameCount += 1;
          break;
        case SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR:
          lcdElement[frameCount].chanelSensor = channel->getChannelNumber();
          lcdElement[frameCount].screenNumbers = frameCount / lcdRows;
          frameCount += 1;

          lcdElement[frameCount].chanelSensor = channel->getChannelNumber();
          lcdElement[frameCount].useAlternativeMeasurement = true;
          lcdElement[frameCount].screenNumbers = frameCount / lcdRows;
          frameCount += 1;
          break;
        case SUPLA_CHANNELTYPE_HUMIDITYSENSOR:
          lcdElement[frameCount].chanelSensor = channel->getChannelNumber();
          lcdElement[frameCount].useAlternativeMeasurement = true;
          lcdElement[frameCount].screenNumbers = frameCount / lcdRows;
          frameCount += 1;
          break;
      }
    }
    if (element->getSecondaryChannel()) {
      auto channel = element->getSecondaryChannel();
      if (channel->getChannelType() == SUPLA_CHANNELTYPE_PRESSURESENSOR) {
        lcdElement[frameCount].chanelSensor = channel->getChannelNumber();
        lcdElement[frameCount].screenNumbers = frameCount / lcdRows;
        frameCount += 1;
      }
    }
  }
}

void SuplaLCD::onInit() {
  uint8_t pinButton = ConfigESP->getGpio(FUNCTION_BUTTON);

  if (pinButton != OFF_GPIO) {
    bool pullUp = ConfigESP->getPullUp(ConfigESP->getGpio(FUNCTION_BUTTON));
    bool invertLogic = ConfigESP->getInversed(ConfigESP->getGpio(FUNCTION_BUTTON));

    Supla::Control::Button* button = new Supla::Control::Button(pinButton, pullUp, invertLogic);
    button->addAction(ActionLCD::LCD_NEXT_FRAME, this, Supla::ON_PRESS);
    button->addAction(ActionLCD::LCD_TURN_ON, this, Supla::ON_PRESS);
  }
}

void SuplaLCD::setup(SuplaConfigManager* configManager, SuplaConfigESP* configESP) {
  ConfigManager = configManager;
  ConfigESP = configESP;
}

void SuplaLCD::iterateAlways() {
  if (ConfigESP->configModeESP == CONFIG_MODE) {
    if (millis() - timeLastChange > 2000) {
      timeLastChange = millis();

      lcd->clear();
      lcd->setCursor(0, 0);
      lcd->print(S_CONFIGURATION_MODE);
      lcd->setCursor(0, 1);
      lcd->print(S_IP_AP);
    }
    return;
  }

  if (ConfigESP->getLastStatusSupla() != STATUS_REGISTERED_AND_READY && ConfigESP->getLastStatusSupla() != STATUS_NETWORK_DISCONNECTED &&
      ConfigESP->getLastStatusSupla() != STATUS_INITIALIZED) {
    if (millis() - timeLastChange > 2000) {
      timeLastChange = millis();

      lcd->clear();
      lcd->setCursor(0, 0);
      lcd->print(ConfigESP->supla_status.msg);
    }
    return;
  }

  if (millis() - backLightTimeLastChange > (unsigned long)(ConfigManager->get(KEY_OLED_BACK_LIGHT_TIME)->getValueInt() * 1000) && oledON &&
      ConfigManager->get(KEY_OLED_BACK_LIGHT_TIME)->getValueInt() != 0 && ConfigESP->getGpio(FUNCTION_BUTTON) != OFF_GPIO) {
    lcd->setBacklight(false);
    oledON = false;
  }

  if (millis() - timeLastChange > (unsigned long)(ConfigManager->get(KEY_OLED_ANIMATION)->getValueInt() * 1000) &&
      ConfigManager->get(KEY_OLED_ANIMATION)->getValueInt() > 0) {
    timeLastChange = millis();
    updateDisplay();
  }

  if (millis() - timeLastChange > 2000 && ConfigManager->get(KEY_OLED_ANIMATION)->getValueInt() == 0) {
    timeLastChange = millis();
    display();
  }
}

void SuplaLCD::updateDisplay() {
  display();
  nextDisplay();
}

void SuplaLCD::display() {
  uint8_t row = 0;

  lcd->clear();

  for (size_t i = 0; i < sensorCount; i++) {
    if (screenNumbers == lcdElement[i].screenNumbers) {
      String name = ConfigManager->get(KEY_NAME_SENSOR)->getElement(i);

      lcd->setCursor(0, row);
      lcd->print(name.c_str());
      lcd->setCursor(lcdCols / 2, row);
      lcd->print(getValueSensor(i).c_str());

      row++;
    }
  }
}

void SuplaLCD::nextDisplay() {
  if (screenNumbers == screenMax)
    screenNumbers = 0;
  else
    screenNumbers++;
}

String SuplaLCD::getValueSensor(uint8_t numberSensor) {
  String value;

  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();

      if (channel->getChannelNumber() == lcdElement[numberSensor].chanelSensor) {
        if (channel->getChannelType() == SUPLA_CHANNELTYPE_THERMOMETER) {
          value = String(channel->getValueDouble(), 2);
          value += "\xDF";
          value += "C";
        }

        if (channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {
          if (lcdElement[numberSensor].useAlternativeMeasurement) {
            value = String(channel->getValueDoubleSecond(), 2);
            value += "%";
          }
          else {
            value = String(channel->getValueDoubleFirst(), 2);
            value += "\xDF";
            value += "C";
          }
        }

        if (channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYSENSOR) {
          value = String(channel->getValueDoubleSecond(), 2);
          value += "%";
        }

        if (channel->getChannelType() == SUPLA_CHANNELTYPE_DISTANCESENSOR) {
          value = String(channel->getValueDouble(), 2);
        }
      }

      if (element->getSecondaryChannel()) {
        auto channel = element->getSecondaryChannel();
        if (channel->getChannelNumber() == lcdElement[numberSensor].chanelSensor) {
          if (channel->getChannelType() == SUPLA_CHANNELTYPE_PRESSURESENSOR) {
            value = String(channel->getValueDouble(), 2);
            value += "hPa";
          }
        }
      }
    }
  }
  return value;
}

void SuplaLCD::handleAction(int event, int action) {
  if (action == ActionLCD::LCD_NEXT_FRAME && oledON) {
    updateDisplay();
  }

  if (action == ActionLCD::LCD_TURN_ON && oledON == false) {
    if (ConfigManager->get(KEY_OLED_BACK_LIGHT_TIME)->getValueInt() != 0 && ConfigESP->getGpio(FUNCTION_BUTTON) != OFF_GPIO) {
      lcd->setBacklight(true);
      backLightTimeLastChange = millis();
    }
    oledON = true;
  }
}
#endif