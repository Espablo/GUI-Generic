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

#include "SuplaWebPageSensorI2c.h"

#ifdef GUI_SENSOR_I2C
void createWebPageSensorI2c() {
  WebServer->httpServer->on(getURL(PATH_I2C), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handleSensorI2c();
    else
      handleSensorI2cSave();
  });
}

void handleSensorI2c(int save) {
  uint8_t selected;

  WebServer->sendHeaderStart();
  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_I2C);

  addForm(webContentBuffer, F("post"), PATH_I2C);
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_I2C);
  addListGPIOBox(webContentBuffer, INPUT_SDA_GPIO, S_SDA, FUNCTION_SDA);
  addListGPIOBox(webContentBuffer, INPUT_SCL_GPIO, S_SCL, FUNCTION_SCL);
  addFormHeaderEnd(webContentBuffer);

  if (ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO) {
#ifdef SUPLA_BME280
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_BME280).toInt();
    addFormHeader(webContentBuffer);
    addListBox(webContentBuffer, INPUT_BME280, String(S_ADDRESS) + S_SPACE + S_BME280, BMx280_P, 4, selected);
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_BME280).toInt())
      addNumberBox(webContentBuffer, INPUT_ALTITUDE_BMx280, S_ALTITUDE_ABOVE_SEA_LEVEL, KEY_ALTITUDE_BMX280, 1500);
    addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_BMP280
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_BMP280).toInt();
    addFormHeader(webContentBuffer);
    addListBox(webContentBuffer, INPUT_BMP280, String(S_ADDRESS) + S_SPACE + S_BMP280, BMx280_P, 4, selected);
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_BMP280).toInt())
      addNumberBox(webContentBuffer, INPUT_ALTITUDE_BMx280, S_ALTITUDE_ABOVE_SEA_LEVEL, KEY_ALTITUDE_BMX280, 1500);
    addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_SHT3x
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_SHT3x).toInt();
    addFormHeader(webContentBuffer);
    addListBox(webContentBuffer, INPUT_SHT3x, S_SHT3X, SHT3x_P, 4, selected);
    addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_SI7021
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_SI7021).toInt();
    addFormHeader(webContentBuffer);
    addListBox(webContentBuffer, INPUT_SI7021, S_SI702, STATE_P, 2, selected);
    addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_VL53L0X
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_VL53L0X).toInt();
    addFormHeader(webContentBuffer);
    addListBox(webContentBuffer, INPUT_VL53L0X, F("VL53L0X"), STATE_P, 2, selected);
    addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_HDC1080
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_HDC1080).toInt();
    addFormHeader(webContentBuffer);
    addListBox(webContentBuffer, INPUT_HDC1080, F("HDC1080"), STATE_P, 2, selected);
    addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_OLED
    addFormHeader(webContentBuffer);

    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_OLED).toInt();
    addListBox(webContentBuffer, INPUT_OLED, S_OLED, OLED_P, 4, selected);

    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_OLED).toInt()) {
      String name, sensorName, input;

      addListGPIOBox(webContentBuffer, INPUT_BUTTON_GPIO, S_OLED_BUTTON, FUNCTION_BUTTON);
      addNumberBox(webContentBuffer, INPUT_OLED_ANIMATION, S_SCREEN_TIME, KEY_OLED_ANIMATION, 99);
      addNumberBox(webContentBuffer, INPUT_OLED_BRIGHTNESS_TIME, S_BACKLIGHT_S, KEY_OLED_BACK_LIGHT_TIME, 99);
      addNumberBox(webContentBuffer, INPUT_OLED_BRIGHTNESS_LVL, S_BACKLIGHT_PERCENT, KEY_OLED_BACK_LIGHT, 99);

      for (uint8_t i = 0; i < getCountSensorChannels(); i++) {
        sensorName = String(ConfigManager->get(KEY_NAME_SENSOR)->getElement(i));
        input = INPUT_OLED_NAME;
        input += i;
        name = S_SCREEN;
        name += i + 1;
        addTextBox(webContentBuffer, input, name, sensorName, 0, MAX_DS18B20_NAME, false);
      }
    }
    addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_LCD_HD44780
    addFormHeader(webContentBuffer);

    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_HD44780).toInt();
    addListBox(webContentBuffer, INPUT_LCD, "HD44780", HD44780_P, 11, selected);

    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_HD44780).toInt()) {
      String name, sensorName, input;

      selected = ConfigManager->get(KEY_HD44780_TYPE)->getValueInt();
      addListBox(webContentBuffer, INPUT_HD44780_TYPE, S_TYPE, HD44780_TYPE_P, 4, selected);
      addListGPIOBox(webContentBuffer, INPUT_BUTTON_GPIO, S_BUTTON, FUNCTION_BUTTON);
      addNumberBox(webContentBuffer, INPUT_OLED_ANIMATION, S_SCREEN_TIME, KEY_OLED_ANIMATION, 99);
      addNumberBox(webContentBuffer, INPUT_OLED_BRIGHTNESS_TIME, S_BACKLIGHT_S, KEY_OLED_BACK_LIGHT_TIME, 99);

      for (uint8_t i = 0; i < getCountSensorChannels(); i++) {
        sensorName = String(ConfigManager->get(KEY_NAME_SENSOR)->getElement(i));
        input = INPUT_OLED_NAME;
        input += i;
        name = S_SCREEN;
        name += i + 1;
        addTextBox(webContentBuffer, input, name, sensorName, 0, MAX_DS18B20_NAME, false);
      }
    }
    addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_MCP23017
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt();
    addFormHeader(webContentBuffer);
    addListBox(webContentBuffer, INPUT_MCP23017, S_MCP23017, STATE_P, 2, selected);
    addFormHeaderEnd(webContentBuffer);
#endif
  }

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);

  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);
  WebServer->sendHeaderEnd();
}

void handleSensorI2cSave() {
  String input;
  uint8_t key;

  if (!WebServer->saveGPIO(INPUT_SDA_GPIO, FUNCTION_SDA)) {
    handleSensorI2c(6);
    return;
  }
  if (!WebServer->saveGPIO(INPUT_SCL_GPIO, FUNCTION_SCL)) {
    handleSensorI2c(6);
    return;
  }

#ifdef SUPLA_BME280
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_BME280;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_BME280, WebServer->httpServer->arg(input).toInt());
  }

  key = KEY_ALTITUDE_BMX280;
  input = INPUT_ALTITUDE_BMx280;
  if (strcmp(WebServer->httpServer->arg(INPUT_ALTITUDE_BMx280).c_str(), "") != 0) {
    ConfigManager->set(key, WebServer->httpServer->arg(input).c_str());
  }
#endif

#ifdef SUPLA_BMP280
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_BMP280;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_BMP280, WebServer->httpServer->arg(input).toInt());
  }

  key = KEY_ALTITUDE_BMX280;
  input = INPUT_ALTITUDE_BMx280;
  if (strcmp(WebServer->httpServer->arg(INPUT_ALTITUDE_BMx280).c_str(), "") != 0) {
    ConfigManager->set(key, WebServer->httpServer->arg(input).c_str());
  }
#endif

#ifdef SUPLA_SHT3x
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_SHT3x;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_SHT3x, WebServer->httpServer->arg(input).toInt());
  }
#endif

#ifdef SUPLA_SI7021
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_SI7021;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_SI7021, WebServer->httpServer->arg(input).toInt());
  }
#endif

#ifdef SUPLA_VL53L0X
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_VL53L0X;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_VL53L0X, WebServer->httpServer->arg(input).toInt());
  }
#endif

#ifdef SUPLA_HDC1080
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_HDC1080;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_HDC1080, WebServer->httpServer->arg(input).toInt());
  }
#endif

#ifdef SUPLA_OLED
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_OLED;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_OLED, WebServer->httpServer->arg(input).toInt());
  }

  if (!WebServer->saveGPIO(INPUT_BUTTON_GPIO, FUNCTION_BUTTON, 0)) {
    handleSensorI2c(6);
    return;
  }

  input = INPUT_OLED_ANIMATION;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0)
    ConfigManager->set(KEY_OLED_ANIMATION, WebServer->httpServer->arg(input).c_str());

  input = INPUT_OLED_BRIGHTNESS_TIME;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0)
    ConfigManager->set(KEY_OLED_BACK_LIGHT_TIME, WebServer->httpServer->arg(input).c_str());

  input = INPUT_OLED_BRIGHTNESS_LVL;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0)
    ConfigManager->set(KEY_OLED_BACK_LIGHT, WebServer->httpServer->arg(input).c_str());

  for (uint8_t i = 0; i < getCountSensorChannels(); i++) {
    input = INPUT_OLED_NAME;
    input += i;
    if (strcmp(WebServer->httpServer->arg(input).c_str(), ConfigManager->get(KEY_NAME_SENSOR)->getElement(i).c_str()) != 0) {
      ConfigManager->setElement(KEY_NAME_SENSOR, i, WebServer->httpServer->arg(input).c_str());
    }
  }
#endif

#ifdef SUPLA_LCD_HD44780
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_LCD;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_HD44780, WebServer->httpServer->arg(input).toInt());
  }

  if (!WebServer->saveGPIO(INPUT_BUTTON_GPIO, FUNCTION_BUTTON, 0)) {
    handleSensorI2c(6);
    return;
  }
  input = INPUT_HD44780_TYPE;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0)
    ConfigManager->set(KEY_HD44780_TYPE, WebServer->httpServer->arg(input).c_str());

  input = INPUT_OLED_ANIMATION;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0)
    ConfigManager->set(KEY_OLED_ANIMATION, WebServer->httpServer->arg(input).c_str());

  input = INPUT_OLED_BRIGHTNESS_TIME;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0)
    ConfigManager->set(KEY_OLED_BACK_LIGHT_TIME, WebServer->httpServer->arg(input).c_str());

  for (uint8_t i = 0; i < getCountSensorChannels(); i++) {
    input = INPUT_OLED_NAME;
    input += i;
    if (strcmp(WebServer->httpServer->arg(input).c_str(), ConfigManager->get(KEY_NAME_SENSOR)->getElement(i).c_str()) != 0) {
      ConfigManager->setElement(KEY_NAME_SENSOR, i, WebServer->httpServer->arg(input).c_str());
    }
  }
#endif

#ifdef SUPLA_MCP23017
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_MCP23017;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_MCP23017, WebServer->httpServer->arg(input).toInt());

    // if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt()) {
    //   ConfigESP->clearFunctionGpio(FUNCTION_RELAY);
    //   ConfigESP->clearFunctionGpio(FUNCTION_BUTTON);
    // }
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleSensorI2c(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleSensorI2c(2);
      break;
  }
}
#endif
