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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <IPAddress.h>
#include <supla-common/proto.h>

#include "SuplaConfigManager.h"
#include "SuplaDeviceGUI.h"

ConfigOption::ConfigOption(uint8_t key, const char *value, int maxLength, bool loadKey)
    : _key(key), _value(nullptr), _maxLength(maxLength), _loadKey(loadKey) {
  if (maxLength > 0) {
    _maxLength = maxLength + 1;

    if (_loadKey) {
      _value = new char[_maxLength];
      setValue(value);
    }
  }
}

uint8_t ConfigOption::getKey() {
  return _key;
}

const char *ConfigOption::getValue() {
  if (_value)
    return _value;
  else
    return "";
}

int ConfigOption::getValueInt() {
  return atoi(this->getValue());
}

bool ConfigOption::getValueBool() {
  return atoi(this->getValue());
}

const char *ConfigOption::getValueHex(size_t size) {
  char *buffer = (char *)malloc(sizeof(char) * (size * 2));
  size_t a, b;

  buffer[0] = 0;
  b = 0;

  for (a = 0; a < size; a++) {
    snprintf(&buffer[b], 3, "%02X", (unsigned char)_value[a]);  // NOLINT
    b += 2;
  }
  return buffer;
}

int ConfigOption::getValueElement(int element) {
  return this->getValue()[element] - 48;
}

int ConfigOption::getLength() {
  return _maxLength;
}

void ConfigOption::setLength(int maxLength) {
  if (maxLength > 0) {
    char *oldValue = new char[_maxLength];
    strncpy(oldValue, this->getValue(), _maxLength);
    oldValue[_maxLength - 1] = '\0';

    _maxLength = maxLength + 1;
    _value = new char[_maxLength];
    setValue(oldValue);
  }
  else {
    _maxLength = maxLength;
  }
}

bool ConfigOption::getLoadKey() {
  return _loadKey;
}

const String ConfigOption::getElement(int index) {
  String data = this->getValue();
  // data.reserve(_maxLength);
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == SEPARATOR || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

const String ConfigOption::replaceElement(int index, int newvalue) {
  String data = this->getValue();
  data.reserve(_maxLength);
  int lenght = _maxLength;
  String table;
  for (int i = 0; i <= lenght; i++) {
    if (i == index) {
      table += newvalue;
    }
    else {
      table += this->getElement(i);
    }

    if (i < lenght - 1)
      table += SEPARATOR;
  }
  return table;
}

const String ConfigOption::replaceElement(int index, const char *newvalue) {
  int lenght = _maxLength;
  String table;
  for (int i = 0; i <= lenght; i++) {
    if (i == index) {
      table += newvalue;
    }
    else {
      table += this->getElement(i);
    }
    if (i < lenght - 1)
      table += SEPARATOR;
  }
  return table;
}

void ConfigOption::setValue(const char *value) {
  if (value && _value) {
    size_t size = getLength();
    strncpy(_value, value, size);
    _value[size - 1] = '\0';
  }
}

//
// class SuplaConfigManager
//
SuplaConfigManager::SuplaConfigManager() {
  if (SPIFFSbegin()) {
    _optionCount = OPTION_COUNT;

    this->addKey(KEY_SUPLA_GUID, SUPLA_GUID_SIZE);
    this->addKey(KEY_SUPLA_AUTHKEY, SUPLA_AUTHKEY_SIZE);
    this->addKey(KEY_WIFI_SSID, MAX_SSID);
    this->addKey(KEY_WIFI_PASS, MAX_PASSWORD);
    this->addKey(KEY_LOGIN, MAX_MLOGIN);
    this->addKey(KEY_LOGIN_PASS, MAX_MPASSWORD);
    this->addKey(KEY_HOST_NAME, DEFAULT_HOSTNAME, MAX_HOSTNAME);
    this->addKey(KEY_SUPLA_SERVER, DEFAULT_SERVER, MAX_SUPLA_SERVER);
    this->addKey(KEY_SUPLA_EMAIL, DEFAULT_EMAIL, MAX_EMAIL);

    this->addKey(KEY_CFG_MODE, 2);
    this->addKey(KEY_ENABLE_GUI, "1", 1);
    this->addKey(KEY_ENABLE_SSL, "0", 1);

    this->addKey(KEY_BOARD, 2);

#ifdef ARDUINO_ARCH_ESP8266
    uint8_t nr, key;
    for (nr = 0; nr <= MAX_GPIO; nr++) {
      key = KEY_GPIO + nr;
      this->addKey(key, 36);
    }
#elif ESP32
    uint8_t nr, key;
    for (nr = 0; nr <= MAX_GPIO; nr++) {
      key = KEY_GPIO + nr;
      this->addKey(key, 36);
    }
#endif

#ifdef SUPLA_RELAY
    this->addKey(KEY_MAX_RELAY, "1", 2);
    this->addKey(KEY_CONDITIONS_SENSOR_TYPE, MAX_GPIO * 2);
    this->addKey(KEY_CONDITIONS_TYPE, MAX_GPIO * 1);
    this->addKey(KEY_CONDITIONS_MIN, MAX_GPIO * 4);
    this->addKey(KEY_CONDITIONS_MAX, MAX_GPIO * 4);
    this->addKey(KEY_VIRTUAL_RELAY, MAX_VIRTUAL_RELAY * 2);
    this->addKey(KEY_VIRTUAL_RELAY_MEMORY, MAX_VIRTUAL_RELAY * 2);

#else
    this->addKey(KEY_MAX_RELAY, 2, false);
    this->addKey(KEY_CONDITIONS_SENSOR_TYPE, MAX_GPIO * 2, false);
    this->addKey(KEY_CONDITIONS_TYPE, MAX_GPIO * 1, false);
    this->addKey(KEY_CONDITIONS_MIN, MAX_GPIO * 4, false);
    this->addKey(KEY_CONDITIONS_MAX, MAX_GPIO * 4, false);
    this->addKey(KEY_VIRTUAL_RELAY, MAX_GPIO * 2, false);
    this->addKey(KEY_VIRTUAL_RELAY_MEMORY, MAX_GPIO * 2, false);

#endif

#ifdef SUPLA_CONDITIONS
    this->addKey(KEY_CONDITIONS_SENSOR_TYPE, MAX_GPIO * 2);
    this->addKey(KEY_CONDITIONS_TYPE, MAX_GPIO * 1);
    this->addKey(KEY_CONDITIONS_MIN, MAX_GPIO * 4);
    this->addKey(KEY_CONDITIONS_MAX, MAX_GPIO * 4);

#else
    this->addKey(KEY_CONDITIONS_SENSOR_TYPE, MAX_GPIO * 2, false);
    this->addKey(KEY_CONDITIONS_TYPE, MAX_GPIO * 1, false);
    this->addKey(KEY_CONDITIONS_MIN, MAX_GPIO * 4, false);
    this->addKey(KEY_CONDITIONS_MAX, MAX_GPIO * 4, false);
#endif

#ifdef SUPLA_BUTTON
    this->addKey(KEY_MAX_BUTTON, "1", 2);
    this->addKey(KEY_ANALOG_BUTTON, 2 * MAX_ANALOG_BUTTON);
    this->addKey(KEY_ANALOG_INPUT_EXPECTED, 5 * MAX_ANALOG_BUTTON);
    this->addKey(KEY_NUMBER_BUTTON, MAX_GPIO * 2);
    this->addKey(KEY_AT_HOLD_TIME, "0.45", 4);
#else
    this->addKey(KEY_MAX_BUTTON, 2, false);
    this->addKey(KEY_ANALOG_BUTTON, 2 * MAX_ANALOG_BUTTON, false);
    this->addKey(KEY_ANALOG_INPUT_EXPECTED, 5 * MAX_ANALOG_BUTTON, false);
    this->addKey(KEY_NUMBER_BUTTON, MAX_GPIO * 2, false);
    this->addKey(KEY_AT_HOLD_TIME, "0.45", 4, false);
#endif

#ifdef SUPLA_LIMIT_SWITCH
    this->addKey(KEY_MAX_LIMIT_SWITCH, "1", 2);
#else
    this->addKey(KEY_MAX_LIMIT_SWITCH, 2, false);
#endif

#ifdef SUPLA_DHT22
    this->addKey(KEY_MAX_DHT22, "1", 2);
#else
    this->addKey(KEY_MAX_DHT22, 2, false);
#endif

#ifdef SUPLA_DHT11
    this->addKey(KEY_MAX_DHT11, "1", 2);
#else
    this->addKey(KEY_MAX_DHT11, 2, false);
#endif

#ifdef SUPLA_RGBW
    this->addKey(KEY_MAX_RGBW, "0", 2);
#else
    this->addKey(KEY_MAX_RGBW, 2, false);
#endif

#ifdef SUPLA_DS18B20
    this->addKey(KEY_MULTI_MAX_DS18B20, "1", 2);
    this->addKey(KEY_ADDR_DS18B20, MAX_DS18B20_ADDRESS_HEX * MAX_DS18B20);
#else
    this->addKey(KEY_MULTI_MAX_DS18B20, 2, false);
    this->addKey(KEY_ADDR_DS18B20, MAX_DS18B20_ADDRESS_HEX * MAX_DS18B20, false);
#endif

#if defined(SUPLA_DS18B20) || defined(SUPLA_OLED) || defined(SUPLA_LCD_HD44780)
    this->addKey(KEY_NAME_SENSOR, MAX_DS18B20_NAME * MAX_DS18B20);
#else
    this->addKey(KEY_NAME_SENSOR, MAX_DS18B20_NAME * MAX_DS18B20, false);
#endif

#ifdef SUPLA_ROLLERSHUTTER
    this->addKey(KEY_MAX_ROLLERSHUTTER, "0", 2);
#else
    this->addKey(KEY_MAX_ROLLERSHUTTER, 2, false);
#endif

#if defined(SUPLA_BME280) || defined(SUPLA_BMP280)
    this->addKey(KEY_ALTITUDE_BMX280, "0", 4);
#else
    this->addKey(KEY_ALTITUDE_BMX280, 4, false);
#endif

#ifdef SUPLA_IMPULSE_COUNTER
    this->addKey(KEY_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT, "10", 4);
    this->addKey(KEY_MAX_IMPULSE_COUNTER, "0", 2);
#else
    this->addKey(KEY_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT, 4, false);
    this->addKey(KEY_MAX_IMPULSE_COUNTER, 2, false);
#endif

#if defined(SUPLA_OLED) || defined(SUPLA_LCD_HD44780)
    this->addKey(KEY_OLED_ANIMATION, "5", 2);
    this->addKey(KEY_OLED_BACK_LIGHT_TIME, "5", 2);
    this->addKey(KEY_OLED_BACK_LIGHT, "20", 2);
#else
    this->addKey(KEY_OLED_ANIMATION, 2, false);
    this->addKey(KEY_OLED_BACK_LIGHT_TIME, 2, false);
    this->addKey(KEY_OLED_BACK_LIGHT, 2, false);
#endif

#ifdef SUPLA_PUSHOVER
    this->addKey(KEY_PUSHOVER_TOKEN, "0", MAX_TOKEN_SIZE);
    this->addKey(KEY_PUSHOVER_USER, "0", MAX_USER_SIZE);
    this->addKey(KEY_PUSHOVER_MASSAGE, MAX_MESSAGE_SIZE * MAX_PUSHOVER_MESSAGE);
#else
    this->addKey(KEY_PUSHOVER_TOKEN, MAX_TOKEN_SIZE, false);
    this->addKey(KEY_PUSHOVER_USER, MAX_USER_SIZE, false);
    this->addKey(KEY_PUSHOVER_MASSAGE, MAX_MESSAGE_SIZE * MAX_PUSHOVER_MESSAGE, false);
#endif

#ifdef SUPLA_HC_SR04
    this->addKey(KEY_HC_SR04_MAX_SENSOR_READ, 3);
#else
    this->addKey(KEY_HC_SR04_MAX_SENSOR_READ, 3, false);
#endif

#ifdef SUPLA_DIRECT_LINKS
    this->addKey(KEY_DIRECT_LINKS_ON, MAX_DIRECT_LINK * MAX_DIRECT_LINKS_SIZE);
    this->addKey(KEY_DIRECT_LINKS_OFF, MAX_DIRECT_LINK * MAX_DIRECT_LINKS_SIZE);
#else
    this->addKey(KEY_DIRECT_LINKS_ON, MAX_DIRECT_LINK * MAX_DIRECT_LINKS_SIZE, false);
    this->addKey(KEY_DIRECT_LINKS_OFF, MAX_DIRECT_LINK * MAX_DIRECT_LINKS_SIZE, false);
#endif

#if defined(GUI_SENSOR_SPI) || defined(GUI_SENSOR_I2C) || defined(GUI_SENSOR_1WIRE) || defined(GUI_SENSOR_OTHER)
    this->addKey(KEY_CORRECTION_TEMP, 6 * MAX_DS18B20);
    this->addKey(KEY_CORRECTION_HUMIDITY, 6 * MAX_DS18B20);
#else
    this->addKey(KEY_CORRECTION_TEMP, 6 * MAX_DS18B20, false);
    this->addKey(KEY_CORRECTION_HUMIDITY, 6 * MAX_DS18B20, false);
#endif

#if defined(GUI_SENSOR_I2C) || defined(GUI_SENSOR_SPI)
    this->addKey(KEY_ACTIVE_SENSOR, 16);
#else
    this->addKey(KEY_ACTIVE_SENSOR, 16, false);
#endif

#ifdef SUPLA_DEEP_SLEEP
    this->addKey(KEY_DEEP_SLEEP_TIME, "0", 3);
#else
    this->addKey(KEY_DEEP_SLEEP_TIME, 3, false);
#endif

#ifdef SUPLA_LCD_HD44780
    this->addKey(KEY_HD44780_TYPE, "2", 1);
#else
    this->addKey(KEY_HD44780_TYPE, 1, false);
#endif

#ifdef SUPLA_DIRECT_LINKS_SENSOR_THERMOMETR
    this->addKey(KEY_MAX_DIRECT_LINKS_SENSOR_THERMOMETR, "0", 2);
    this->addKey(KEY_DIRECT_LINKS_SENSOR_THERMOMETR, "0", MAX_DIRECT_LINK * MAX_DIRECT_LINKS_SIZE);
#else
    this->addKey(KEY_MAX_DIRECT_LINKS_SENSOR_THERMOMETR, 2, false);
    this->addKey(KEY_DIRECT_LINKS_SENSOR_THERMOMETR, MAX_DIRECT_LINK * MAX_DIRECT_LINKS_SIZE, false);
#endif

#ifdef SUPLA_CONDITIONS
    this->addKey(KEY_CONDITIONS_SENSOR_NUMBER, "0", MAX_GPIO * 3);
#else
    this->addKey(KEY_CONDITIONS_SENSOR_NUMBER, MAX_GPIO * 3, false);
#endif

#ifdef SUPLA_RF_BRIDGE
    this->addKey(KEY_RF_BRIDGE_CODE_ON, MAX_BRIDGE_RF * 10);
    this->addKey(KEY_RF_BRIDGE_CODE_OFF, MAX_BRIDGE_RF * 10);
    this->addKey(KEY_RF_BRIDGE_LENGTH, MAX_BRIDGE_RF * 3);
    this->addKey(KEY_RF_BRIDGE_TYPE, MAX_BRIDGE_RF * 2);
    this->addKey(KEY_RF_BRIDGE_PROTOCOL, MAX_BRIDGE_RF * 3);
    this->addKey(KEY_RF_BRIDGE_PULSE_LENGTHINT, MAX_BRIDGE_RF * 4);
    this->addKey(KEY_RF_BRIDGE_REPEAT, MAX_BRIDGE_RF * 2);
#else
    this->addKey(KEY_RF_BRIDGE_CODE_ON, MAX_BRIDGE_RF * 10, false);
    this->addKey(KEY_RF_BRIDGE_CODE_OFF, MAX_BRIDGE_RF * 10, false);
    this->addKey(KEY_RF_BRIDGE_LENGTH, MAX_BRIDGE_RF * 3, false);
    this->addKey(KEY_RF_BRIDGE_TYPE, MAX_BRIDGE_RF * 2, false);
    this->addKey(KEY_RF_BRIDGE_PROTOCOL, MAX_BRIDGE_RF * 3, false);
    this->addKey(KEY_RF_BRIDGE_PULSE_LENGTHINT, MAX_BRIDGE_RF * 4, false);
    this->addKey(KEY_RF_BRIDGE_REPEAT, MAX_BRIDGE_RF * 2, false);
#endif

#ifdef SUPLA_ACTION_TRIGGER
    this->addKey(KEY_AT_MULTICLICK_TIME, "0.45", 4);
#else
    this->addKey(KEY_AT_MULTICLICK_TIME, 4, false);
#endif

#ifdef SUPLA_ANALOG_READING_MAP
    this->addKey(KEY_MAX_ANALOG_READING, "1", 2);
#else
    this->addKey(KEY_MAX_ANALOG_READING, 2, false);
#endif

    this->addKey(KEY_FORCE_RESTART_ESP, "0", 1);

#ifdef GUI_SENSOR_I2C_EXPENDER
    this->addKey(KEY_ACTIVE_EXPENDER, 20);
#else
    this->addKey(KEY_ACTIVE_EXPENDER, 20, false);
#endif

    //  this->addKey(KEY_VERSION_CONFIG, String(CURENT_VERSION).c_str(), 2);

    SPIFFS.end();
    switch (this->load()) {
      case E_CONFIG_OK:
        Serial.println(F("Config read"));
        this->showAllValue();
        return;
      case E_CONFIG_PARSE_ERROR:
        Serial.println(F("E_CONFIG_PARSE_ERROR: File was not parsable"));
        return;
      case E_CONFIG_FILE_NOT_FOUND:
        Serial.println(F("File not found"));
        return;
      default:
        Serial.println(F("Config read error"));
        delay(2000);
        ESP.restart();
        return;
    }
  }
  else {
    Serial.println(F("Failed to mount SPIFFS"));
    Serial.println(F("Formatting SPIFFS"));
    SPIFFS.format();
    delay(500);
    SuplaConfigManager();
    // ESP.restart();
  }
  //  switch (this->load()) {
  //    case E_CONFIG_OK:
  //      Serial.println(F("Config read"));
  //      return;
  //    case E_CONFIG_FS_ACCESS:
  //      Serial.println(F("E_CONFIG_FS_ACCESS: Couldn't access file system"));
  //      return;
  //    case E_CONFIG_FILE_NOT_FOUND:
  //      Serial.println(F("E_CONFIG_FILE_NOT_FOUND: File not found"));
  //      return;
  //    case E_CONFIG_FILE_OPEN:
  //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
  //      return;
  //    case E_CONFIG_PARSE_ERROR:
  //      Serial.println(F("E_CONFIG_PARSE_ERROR: File was not parsable"));
  //      return;
  //  }
}

bool SuplaConfigManager::SPIFFSbegin() {
  for (uint8_t t = 4; t > 0; t--) {
    if (
#ifdef ARDUINO_ARCH_ESP8266
        !SPIFFS.begin()
#elif ESP32
        !SPIFFS.begin(true)
#endif
    ) {
      Serial.printf("[SPIFFS] WAIT %d...\n", t);
      Serial.flush();
      delay(250);
    }
    else {
      return true;
    }
  }
  return false;
}

bool SuplaConfigManager::migrationConfig() {
  bool migration = false;
  // Serial.print(F("migration Config ver:"));
  // Serial.println(this->get(KEY_VERSION_CONFIG)->getValueInt());

  // if (this->get(KEY_VERSION_CONFIG)->getValueInt() == 0) {
  //   Serial.println(F("0 -> 1"));

  //   if (this->load(false) == E_CONFIG_OK) {
  //     this->get(KEY_VERSION_CONFIG)->setValue("1");
  //     migration = true;
  //   }
  // }

  // if (this->get(KEY_VERSION_CONFIG)->getValueInt() == 1) {
  //   Serial.println(F("1 -> 2"));
  //   // ustawienie starej długości zmiennej przed wczytaniem starego konfiga
  //   this->get(KEY_SUPLA_GUID)->setLength(SUPLA_GUID_SIZE);
  //   this->get(KEY_SUPLA_AUTHKEY)->setLength(SUPLA_AUTHKEY_SIZE);

  //   if (this->load(false) == E_CONFIG_OK) {
  //     // po poprawnym wczytaniu konfiga ustawienie poprawnej wartośći zmiennej
  //     this->get(KEY_SUPLA_GUID)->setLength(SUPLA_GUID_HEXSIZE);
  //     this->get(KEY_SUPLA_AUTHKEY)->setLength(SUPLA_AUTHKEY_HEXSIZE);

  //     this->get(KEY_VERSION_CONFIG)->setValue("2");
  //     migration = true;
  //   }
  // }

  // if (migration) {
  //   this->get(KEY_VERSION_CONFIG)->setValue(String(CURENT_VERSION).c_str());
  //   this->save();
  //   Serial.println(F("successful Config migration"));
  // }

  return migration;
}

uint8_t SuplaConfigManager::addKey(uint8_t key, int maxLength, bool loadKey) {
  return addKey(key, "", maxLength, loadKey);
}

uint8_t SuplaConfigManager::addKey(uint8_t key, const char *value, int maxLength, bool loadKey) {
  if (_optionCount == CONFIG_MAX_OPTIONS) {
    return E_CONFIG_MAX;
  }
  _options[key] = new ConfigOption(key, value, maxLength, loadKey);
  //_optionCount += 1; OPTION_COUNT

  return E_CONFIG_OK;
}

uint8_t SuplaConfigManager::deleteKey(uint8_t key) {
  for (int i = 0; i < _optionCount; i++) {
    if (_options[i]->getKey() == key) {
      delete _options[_optionCount];
      _optionCount -= 1;
    }
  }

  return E_CONFIG_OK;
}

uint8_t SuplaConfigManager::load(bool configParse) {
  if (SPIFFSbegin()) {
    if (SPIFFS.exists(CONFIG_FILE_PATH)) {
      File configFile = SPIFFS.open(CONFIG_FILE_PATH, "r");
      configFile.setTimeout(5000);

      if (configFile) {
        int i = 0;
        int offset = 0;
        size_t length = 0;

        for (i = 0; i < _optionCount; i++) {
          length += _options[i]->getLength();
        }

        // #ifdef ARDUINO_ARCH_ESP8266
        //         FSInfo fs_info;
        //         SPIFFS.info(fs_info);

        //         float fileTotalKB = (float)fs_info.totalBytes / 1024.0;
        //         float fileUsedKB = (float)fs_info.usedBytes / 1024.0;

        //         Serial.println(F("File system (SPIFFS): "));
        //         Serial.print(F(" Total KB: "));
        //         Serial.print(fileTotalKB);
        //         Serial.println(F(" KB"));
        //         Serial.print(F(" Used KB: "));
        //         Serial.print(fileUsedKB);
        //         Serial.println(F(" KB"));
        //         Serial.print(F("Size file: "));
        //         Serial.println(configFile.size());
        //         Serial.print(F("Size conf: "));
        //         Serial.println(length);
        // #endif

        uint8_t *content = new uint8_t[length];
        configFile.read(content, length);

        for (i = 0; i < _optionCount; i++) {
          if (_options[i]->getLoadKey()) {
            if (strcmp((const char *)(content + offset), "") != 0) {
              // if (strcmp((const char *)(content + offset), "") != 0 || i == KEY_VERSION_CONFIG) {
              _options[i]->setValue((const char *)(content + offset));
            }
          }
          offset += _options[i]->getLength();
          delay(0);
        }

        // if (this->get(KEY_VERSION_CONFIG)->getValueInt() != CURENT_VERSION && configParse) {
        //   if (!this->migrationConfig())
        //     return E_CONFIG_PARSE_ERROR;
        // }

        configFile.close();
        SPIFFS.end();
        delete content;

        return E_CONFIG_OK;
      }
      else {
        configFile.close();
        SPIFFS.end();
        return E_CONFIG_FILE_OPEN;
      }
    }
    else {
      return E_CONFIG_FILE_NOT_FOUND;
    }
  }
  else {
    return E_CONFIG_FS_ACCESS;
  }
}

uint8_t SuplaConfigManager::save() {
  if (SPIFFSbegin()) {
    int i = 0;
    int offset = 0;
    int length = 0;

    for (i = 0; i < _optionCount; i++) {
      length += _options[i]->getLength();
    }

    File configFile = SPIFFS.open(CONFIG_FILE_PATH, "w");
    configFile.setTimeout(5000);

    if (configFile) {
      uint8_t *content = new uint8_t[length];
      for (i = 0; i < _optionCount; i++) {
        if (_options[i]->getLoadKey() && strcmp(_options[i]->getValue(), "") != 0) {
          Serial.print(F("Save key: "));
          Serial.print(_options[i]->getKey());
          Serial.print(F(" Value: "));
          Serial.println(_options[i]->getValue());
          memcpy(content + offset, _options[i]->getValue(), _options[i]->getLength());
        }
        else {
          memset(content + offset, 0, _options[i]->getLength());
        }
        offset += _options[i]->getLength();
        delay(0);
      }

      configFile.write(content, length);

      configFile.flush();
      configFile.close();
      SPIFFS.end();

      delete content;

      return E_CONFIG_OK;
    }
    else {
      configFile.close();
      SPIFFS.end();
      return E_CONFIG_FILE_OPEN;
    }
  }

  return E_CONFIG_FS_ACCESS;
}

void SuplaConfigManager::showAllValue() {
  for (int i = 0; i < _optionCount; i++) {
    Serial.print(F("Key: "));
    Serial.print(_options[i]->getKey());
    Serial.print(F(" Value: "));
    Serial.println(_options[i]->getValue());
  }
}

void SuplaConfigManager::deleteAllValues() {
  deleteWifiSuplaAdminValues();
  deleteDeviceValues();
}

void SuplaConfigManager::deleteDeviceValues() {
  for (int i = KEY_MAX_RELAY; i < _optionCount; i++) {
    _options[i]->setValue("");
  }
}

void SuplaConfigManager::deleteWifiSuplaAdminValues() {
  for (int i = KEY_WIFI_SSID; i <= KEY_SUPLA_EMAIL; i++) {
    _options[i]->setValue("");
  }
}

void SuplaConfigManager::deleteGPIODeviceValues() {
  for (int i = KEY_GPIO; i <= KEY_GPIO + MAX_GPIO; i++) {
    _options[i]->setValue("");
  }
}

bool SuplaConfigManager::isDeviceConfigured() {
  return strcmp(this->get(KEY_SUPLA_GUID)->getValue(), "") == 0 || strcmp(this->get(KEY_SUPLA_AUTHKEY)->getValue(), "") == 0 ||
         strcmp(this->get(KEY_LOGIN)->getValue(), "") == 0 || strcmp(this->get(KEY_ENABLE_SSL)->getValue(), "") == 0 ||
         strcmp(this->get(KEY_ENABLE_GUI)->getValue(), "") == 0 || strcmp(this->get(KEY_SUPLA_SERVER)->getValue(), DEFAULT_SERVER) == 0 ||
         strcmp(this->get(KEY_SUPLA_EMAIL)->getValue(), DEFAULT_EMAIL) == 0 || ConfigESP->getGpio(FUNCTION_CFG_BUTTON) == OFF_GPIO;
}

ConfigOption *SuplaConfigManager::get(uint8_t key) {
  for (int i = 0; i < _optionCount; i++) {
    if (_options[i]->getKey() == key) {
      return _options[i];
    }
  }
  return NULL;
}
bool SuplaConfigManager::set(uint8_t key, int value) {
  char buffer[10];
  itoa(value, buffer, 10);
  return set(key, buffer);
}

bool SuplaConfigManager::set(uint8_t key, const char *value) {
  for (int i = 0; i < _optionCount; i++) {
    if (key == _options[i]->getKey()) {
      _options[i]->setValue(value);
      return true;
    }
  }
  return false;
}

bool SuplaConfigManager::setElement(uint8_t key, int index, int newvalue) {
  for (int i = 0; i < _optionCount; i++) {
    if (key == _options[i]->getKey()) {
      String data = _options[i]->replaceElement(index, newvalue);
      _options[i]->setValue(data.c_str());
      return true;
    }
  }
  return false;
}

bool SuplaConfigManager::setElement(uint8_t key, int index, const char *newvalue) {
  for (int i = 0; i < _optionCount; i++) {
    if (key == _options[i]->getKey()) {
      String data = _options[i]->replaceElement(index, newvalue);
      _options[i]->setValue(data.c_str());
      return true;
    }
  }
  return false;
}

void SuplaConfigManager::setGUIDandAUTHKEY() {
  char GUID[SUPLA_GUID_SIZE];
  char AUTHKEY[SUPLA_AUTHKEY_SIZE];

  memset(GUID, 0, SUPLA_GUID_SIZE);
  memset(AUTHKEY, 0, SUPLA_AUTHKEY_SIZE);

#if defined(ESP32)
  esp_fill_random((unsigned char *)GUID, SUPLA_GUID_SIZE);
  esp_fill_random((unsigned char *)AUTHKEY, SUPLA_AUTHKEY_SIZE);
#else
  os_get_random((unsigned char *)GUID, SUPLA_GUID_SIZE);
  os_get_random((unsigned char *)AUTHKEY, SUPLA_AUTHKEY_SIZE);
#endif

  this->set(KEY_SUPLA_GUID, GUID);
  this->set(KEY_SUPLA_AUTHKEY, AUTHKEY);

  String GUID_S = ConfigManager->get(KEY_SUPLA_GUID)->getValueHex(SUPLA_GUID_SIZE);
  String AUTHKEY_S = ConfigManager->get(KEY_SUPLA_AUTHKEY)->getValueHex(SUPLA_AUTHKEY_SIZE);
  GUID_S.reserve(32);
  AUTHKEY_S.reserve(32);

  if (GUID_S.endsWith("0") || AUTHKEY_S.endsWith("0")) {
    setGUIDandAUTHKEY();
  }
}
