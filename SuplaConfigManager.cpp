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

#include "FS.h"
#include "SuplaConfigManager.h"

#define CONFIG_FILE_PATH "/dat"

ConfigOption::ConfigOption(const char *key, const char *value, int maxLength) {
  size_t size = strlen(key) + 1;
  _key = (char *)malloc(sizeof(char) * size);
  memcpy(_key, key, size - 1);
  _key[size - 1] = 0;

  _maxLength = maxLength;
  setValue(value);
}

const char *ConfigOption::getKey() {
  return _key;
}

const char *ConfigOption::getValue() {
  return _value;
}

int ConfigOption::getValueInt() {
  return atoi(_value);
}

uint8_t *ConfigOption::getValueBin(size_t size) {
  uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * (size));

  for (int i = 0; i < size; i++) {
    sscanf(&_value[i * 2], "%2hhx", &buffer[i]);
  }
  return buffer;
}

const char *ConfigOption::getValueHex(size_t size) {
  char *buffer = (char *)malloc(sizeof(char) * (size * 2));
  int a, b;

  buffer[0] = 0;
  b = 0;

  for (a = 0; a < size; a++) {
    snprintf(&buffer[b], 3, "%02X", (unsigned char)_value[a]);  // NOLINT
    b += 2;
  }
  return buffer;
}

int ConfigOption::getValueElement(int element) {
  return _value[element] - 48;
}

int ConfigOption::getLength() {
  return _maxLength;
}

String ConfigOption::getElement(int index) {
  String data = _value;
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

String ConfigOption::replaceElement(int index, int newvalue) {
  String data = _value;
  int lenght = 5;
  String table;
  for (int i = 0; i < lenght; i++) {
    if (i == index) {
      table += newvalue;
    } else {
      table += this->getElement(i);
    }
    if (i < lenght - 1) table += SEPARATOR;
  }
  return table;
}

void ConfigOption::setValue(const char *value) {
  size_t size = _maxLength + 1;
  _value = (char *)malloc(sizeof(char) * (size));

  if (value != NULL) {
    memcpy(_value, value, size - 1);
    _value[size - 1] = 0;
  }
}

//
// class SuplaConfigManager
//
SuplaConfigManager::SuplaConfigManager() {
  if (SPIFFS.begin()) {
    //    Serial.println(F("\nSPIFFS mounted"));
    //  } else {
    //    Serial.println(F("\nFailed to mount SPIFFS"));
  }
  _optionCount = 0;

  this->addKey(KEY_SUPLA_GUID, MAX_GUID);
  this->addKey(KEY_SUPLA_AUTHKEY, MAX_AUTHKEY);
  this->addKey(KEY_WIFI_SSID, MAX_SSID);
  this->addKey(KEY_WIFI_PASS, MAX_PASSWORD);
  this->addKey(KEY_LOGIN, MAX_MLOGIN);
  this->addKey(KEY_LOGIN_PASS, MAX_MPASSWORD);
  this->addKey(KEY_HOST_NAME, DEFAULT_HOSTNAME, MAX_HOSTNAME);
  this->addKey(KEY_SUPLA_SERVER, DEFAULT_SERVER, MAX_SUPLA_SERVER);
  this->addKey(KEY_SUPLA_EMAIL, DEFAULT_EMAIL, MAX_EMAIL);
  this->addKey(KEY_MAX_ROLLERSHUTTER, "0", 2);
  this->addKey(KEY_MAX_RELAY, "0", 2);
  this->addKey(KEY_MAX_BUTTON, "0", 2);
  this->addKey(KEY_MAX_LIMIT_SWITCH, "0", 2);
  this->addKey(KEY_MAX_DHT22, "0", 2);
  this->addKey(KEY_MAX_DHT11, "0", 2);
  this->addKey(KEY_MULTI_MAX_DS18B20, "1", 2);
  this->addKey(KEY_ADR_BME280, "0", 2);
  this->addKey(KEY_ALTITUDE_BME280, "0", 4);

  int nr;
  String key;
  for (nr = 0; nr <= 17; nr++) {
    key = GPIO;
    key += nr;
    String func;
    func = "0";
    func += SEPARATOR;
    func += "0";
    func += SEPARATOR;
    func += "0";
    func += SEPARATOR;
    func += "0";
    func += SEPARATOR;
    func += "0";
    this->addKey(key.c_str(), func.c_str(), 14);
  }

  for (nr = 0; nr <= MAX_DS18B20; nr++) {
    key = KEY_DS;
    key += nr;
    this->addKey(key.c_str(), MAX_DS18B20_ADDRESS_HEX);
    key = KEY_DS_NAME;
    key += nr;
    this->addKey(key.c_str(), MAX_DS18B20_NAME);
  }
  this->load();
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

uint8_t SuplaConfigManager::addKey(const char *key, int maxLength) {
  return addKey(key, "", maxLength);
}

uint8_t SuplaConfigManager::addKey(const char *key, const char *value, int maxLength) {
  if (_optionCount == CONFIG_MAX_OPTIONS) {
    return E_CONFIG_MAX;
  }
  _options[_optionCount] = new ConfigOption(key, value, maxLength);
  _optionCount += 1;

  return E_CONFIG_OK;
}

uint8_t SuplaConfigManager::addKeyAndRead(const char *key, const char *value, int maxLength) {
  addKey(key, maxLength);
  if (this->loadItem(key) != E_CONFIG_OK) {
    this->set(key, value);
  }
  return E_CONFIG_OK;
}

uint8_t SuplaConfigManager::deleteKey(const char *key) {
  for (int i = 0; i < _optionCount; i++) {
    if (strcmp(_options[i]->getKey(), key) == 0) {
      delete _options[_optionCount];
      _optionCount -= 1;
    }
  }

  return E_CONFIG_OK;
}

uint8_t SuplaConfigManager::load() {

  if (SPIFFS.begin()) {
    if (SPIFFS.exists(CONFIG_FILE_PATH)) {
      File configFile = SPIFFS.open(CONFIG_FILE_PATH, "r");

      if (configFile) {
        int i = 0;
        int offset = 0;
        int length = 0;

        for (i = 0; i < _optionCount; i++) {
          length += _options[i]->getLength();
        }

        // if (length != configFile.size()) {
        //   return E_CONFIG_PARSE_ERROR;
        // }

        uint8_t *content = (uint8_t *)malloc(sizeof(uint8_t) * length);
        configFile.read(content, length);

        for (i = 0; i < _optionCount; i++) {
          _options[i]->setValue((const char *)(content + offset));
          offset += _options[i]->getLength();
        }

        configFile.close();
        free(content);

        return E_CONFIG_OK;
      } else {
        configFile.close();
        return E_CONFIG_FILE_OPEN;
      }
    } else {
      return E_CONFIG_FILE_NOT_FOUND;
    }
  } else {
    return E_CONFIG_FS_ACCESS;
  }
}

uint8_t SuplaConfigManager::loadItem(const char *key) {

  if (SPIFFS.begin()) {
    if (SPIFFS.exists(CONFIG_FILE_PATH)) {
      File configFile = SPIFFS.open(CONFIG_FILE_PATH, "r");

      if (configFile) {
        int i = 0;
        int offset = 0;
        int length = 0;

        for (i = 0; i < _optionCount; i++) {
          length += _options[i]->getLength();
        }

        uint8_t *content = (uint8_t *)malloc(sizeof(uint8_t) * length);
        configFile.read(content, length);

        for (i = 0; i < _optionCount; i++) {
          if (strcmp(_options[i]->getKey(), key) == 0) {
            _options[i]->setValue((const char *)(content + offset));
          }
          offset += _options[i]->getLength();
        }

        configFile.close();
        free(content);

        return E_CONFIG_OK;
      } else {
        configFile.close();
        return E_CONFIG_FILE_OPEN;
      }
    } else {
      return E_CONFIG_FILE_NOT_FOUND;
    }
  } else {
    return E_CONFIG_FS_ACCESS;
  }
}

uint8_t SuplaConfigManager::save() {
  if (SPIFFS.begin()) {
    int i = 0;
    int offset = 0;
    int length = 0;

    for (i = 0; i < _optionCount; i++) {
      length += _options[i]->getLength();
    }

    File configFile = SPIFFS.open(CONFIG_FILE_PATH, "w+");
    if (configFile) {
      uint8_t *content = (uint8_t *)malloc(sizeof(uint8_t) * length);
      for (i = 0; i < _optionCount; i++) {
        Serial.println("Save key " + String(_options[i]->getKey()) + ": " + String(_options[i]->getValue()));
        memcpy(content + offset, _options[i]->getValue(), _options[i]->getLength());
        offset += _options[i]->getLength();
      }

      configFile.write(content, length);
      configFile.close();

      free(content);
      return E_CONFIG_OK;
    } else {
      return E_CONFIG_FILE_OPEN;
    }
  }

  return E_CONFIG_FS_ACCESS;
}

void SuplaConfigManager::showAllValue() {
  for (int i = 0; i < _optionCount; i++) {
    Serial.println("Key: " + String(_options[i]->getKey()) + " Value: " + String(_options[i]->getValue()));
  }
}

bool SuplaConfigManager::isDeviceConfigured() {
  return strcmp(this->get(KEY_SUPLA_GUID)->getValue(), "") == 0 ||
         strcmp(this->get(KEY_SUPLA_AUTHKEY)->getValue(), "") == 0 ||
         strcmp(this->get(KEY_WIFI_SSID)->getValue(), "") == 0 ||
         strcmp(this->get(KEY_WIFI_PASS)->getValue(), "") == 0 ||
         strcmp(this->get(KEY_LOGIN)->getValue(), "") == 0;
}

ConfigOption *SuplaConfigManager::get(const char *key) {
  for (int i = 0; i < _optionCount; i++) {
    if (strcmp(_options[i]->getKey(), key) == 0) {
      return _options[i];
    }
  }
  return NULL;
}

bool SuplaConfigManager::set(const char *key, const char *value) {
  for (int i = 0; i < _optionCount; i++) {
    if (strcmp(key, _options[i]->getKey()) == 0) {
      _options[i]->setValue(value);
      return true;
    }
  }
  return false;
}

bool SuplaConfigManager::setElement(const char *key, int index, int newvalue) {
  for (int i = 0; i < _optionCount; i++) {
    if (strcmp(key, _options[i]->getKey()) == 0) {
      String data = _options[i]->replaceElement(index, newvalue);
      _options[i]->setValue(data.c_str());
      return true;
    }
  }
  return false;
}

void SuplaConfigManager::setGUIDandAUTHKEY() {
  if (strcmp(this->get(KEY_SUPLA_GUID)->getValue(), "") != 0 ||
      strcmp(this->get(KEY_SUPLA_AUTHKEY)->getValue(), "") != 0) {
    return;
  }

  char mac[6];
  int a;

  char GUID[SUPLA_GUID_SIZE];
  char AUTHKEY[SUPLA_AUTHKEY_SIZE];

  memset(GUID, 0, SUPLA_GUID_SIZE);
  memset(AUTHKEY, 0, SUPLA_AUTHKEY_SIZE);

  os_get_random((unsigned char *)GUID, SUPLA_GUID_SIZE);
  os_get_random((unsigned char *)AUTHKEY, SUPLA_AUTHKEY_SIZE);

  if (SUPLA_GUID_SIZE >= 6) {
    wifi_get_macaddr(STATION_IF, (unsigned char *)mac);

    for (a = 0; a < 6; a++)
      GUID[a] = (GUID[a] * mac[a]) % 255;
  }

  if (SUPLA_GUID_SIZE >= 12) {
    wifi_get_macaddr(SOFTAP_IF, (unsigned char *)mac);

    for (a = 0; a < 6; a++) GUID[a + 6] = (GUID[a + 6] * mac[a]) % 255;
  }

  for (a = 0; a < SUPLA_GUID_SIZE; a++) {
    GUID[a] = (GUID[a] + system_get_time() + spi_flash_get_id() + system_get_chip_id() + system_get_rtc_time()) % 255;
  }

  a = SUPLA_GUID_SIZE > SUPLA_AUTHKEY_SIZE ? SUPLA_AUTHKEY_SIZE : SUPLA_GUID_SIZE;
  a--;
  for (; a > 0; a--) {
    AUTHKEY[a] += GUID[a];
  }

  this->set(KEY_SUPLA_GUID, GUID);
  this->set(KEY_SUPLA_AUTHKEY, AUTHKEY);
}
