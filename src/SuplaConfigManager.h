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

#ifndef SuplaConfigManager_h
#define SuplaConfigManager_h

#define DEFAULT_HOSTNAME "GUI Generic"

#define DEFAULT_LOGIN      "admin"
#define DEFAULT_LOGIN_PASS "pass"

#define DEFAULT_SERVER "svrX.supla.org"
#define DEFAULT_EMAIL  "email@address.com"

#define MAX_GUID                SUPLA_GUID_SIZE
#define MAX_AUTHKEY             SUPLA_GUID_SIZE
#define MAX_SSID                32
#define MAX_PASSWORD            64
#define MIN_PASSWORD            8
#define MAX_MLOGIN              32
#define MAX_MPASSWORD           64
#define MAX_HOSTNAME            32
#define MAX_SUPLA_SERVER        SUPLA_SERVER_NAME_MAXSIZE
#define MAX_EMAIL               SUPLA_EMAIL_MAXSIZE
#define MAX_DS18B20_ADDRESS_HEX 16
#define MAX_DS18B20_ADDRESS     8
#define MAX_DS18B20_NAME        8
#define MAX_TYPE_BUTTON         4
#define MAX_MONOSTABLE_TRIGGER  1
#define MAX_FUNCTION            1
#define MAX_DS18B20             10
#define MAX_GPIO                17

enum _key {
KEY_SUPLA_GUID,
KEY_SUPLA_AUTHKEY,
KEY_WIFI_SSID,
KEY_WIFI_PASS,
KEY_LOGIN,
KEY_LOGIN_PASS,
KEY_HOST_NAME,
KEY_SUPLA_SERVER,
KEY_SUPLA_EMAIL,
KEY_MAX_RELAY,
KEY_MAX_BUTTON,
KEY_MAX_LIMIT_SWITCH,
KEY_MAX_DHT22,
KEY_MAX_DHT11,
KEY_MULTI_MAX_DS18B20,
KEY_MAX_ROLLERSHUTTER,
KEY_ALTITUDE_BME280,
KEY_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT,
KEY_MAX_IMPULSE_COUNTER,
KEY_ACTIVE_SENSOR,
KEY_BOARD,
KEY_CFG_MODE,
KEY_GPIO,
KEY_DS = KEY_GPIO + MAX_GPIO + MAX_DS18B20,
KEY_DS_NAME = KEY_DS + MAX_DS18B20
};

//#define GPIO      "GPIO"
#define SEPARATOR ','

enum _settings
{
  NR,
  FUNCTION,
  LEVEL,
  MEMORY,
  CFG,
  ACTION,
  SETTINGSCOUNT
};

enum _function
{
  FUNCTION_OFF,
  FUNCTION_RELAY,
  FUNCTION_BUTTON,
  FUNCTION_LIMIT_SWITCH,
  FUNCTION_CFG_LED,
  FUNCTION_CFG_BUTTON,
  FUNCTION_DS18B20,
  FUNCTION_DHT11,
  FUNCTION_DHT22,
  FUNCTION_SDA,
  FUNCTION_SCL,
  FUNCTION_TRIG,
  FUNCTION_ECHO,
  FUNCTION_SI7021_SONOFF,
  FUNCTION_CLK,
  FUNCTION_CS,
  FUNCTION_D0,
  FUNCTION_IMPULSE_COUNTER
};

enum _e_onfig
{
  E_CONFIG_OK,
  E_CONFIG_FS_ACCESS,
  E_CONFIG_FILE_NOT_FOUND,
  E_CONFIG_FILE_OPEN,
  E_CONFIG_PARSE_ERROR,
  E_CONFIG_MAX
};

#define CONFIG_MAX_OPTIONS 88

class ConfigOption {
 public:
  ConfigOption(uint8_t key, const char *value, int maxLength);
  uint8_t getKey();
  const char *getValue();
  int getValueInt();
  uint8_t *getValueBin(size_t size);
  const char *getValueHex(size_t size);
  int getValueElement(int element);

  int getLength();
  void setValue(const char *value);
  String getElement(int index);
  String replaceElement(int index, int value);

 private:
  uint8_t _key;
  char *_value;
  int _maxLength;
};

class SuplaConfigManager {
 public:
  SuplaConfigManager();
  uint8_t addKey(uint8_t key, int maxLength);
  uint8_t addKey(uint8_t key, const char *value, int maxLength);
  uint8_t addKeyAndRead(uint8_t key, const char *value, int maxLength);
  uint8_t deleteKey(uint8_t key);
  uint8_t load();
  uint8_t loadItem(uint8_t key);
  uint8_t save();
  void showAllValue();

  ConfigOption *get(uint8_t key);
  bool set(uint8_t key, const char *value);
  bool setElement(uint8_t key, int index, int newvalue);

  bool isDeviceConfigured();
  void setGUIDandAUTHKEY();

 private:
  int _optionCount;
  ConfigOption *_options[CONFIG_MAX_OPTIONS];
};
#endif
