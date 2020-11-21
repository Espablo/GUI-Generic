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

#define KEY_SUPLA_GUID                       "GUID"
#define KEY_SUPLA_AUTHKEY                    "AUTHKEY"
#define KEY_WIFI_SSID                        "wifiSSID"
#define KEY_WIFI_PASS                        "wifiPass"
#define KEY_LOGIN                            "login"
#define KEY_LOGIN_PASS                       "loginPass"
#define KEY_HOST_NAME                        "hostName"
#define KEY_SUPLA_SERVER                     "suplaServer"
#define KEY_SUPLA_EMAIL                      "suplaEmail"
#define KEY_DS                               "ds"
#define KEY_DS_NAME                          "dsName"
#define KEY_MULTI_MAX_DS18B20                "multiMaxDs"
#define KEY_SUPLA_FUNCTION                   "function"
#define KEY_MAX_RELAY                        "maxRelay"
#define KEY_MAX_BUTTON                       "maxButton"
#define KEY_MAX_LIMIT_SWITCH                 "maxLimitSwitch"
#define KEY_MAX_DHT11                        "maxDht11"
#define KEY_MAX_DHT22                        "maxDht22"
#define KEY_MAX_ROLLERSHUTTER                "maxRollerShutter"
#define KEY_ALTITUDE_BME280                  "altbme280"
#define KEY_ACTIVE_SENSOR                    "sensor"
#define KEY_BOARD                            "board"
#define KEY_CFG_MODE                         "cfgmode"
#define KEY_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT "icDebounceTimeout"
#define KEY_IMPULSE_COUNTER_PULL_UP          "icPullUp"
#define KEY_IMPULSE_COUNTER_RAISING_EDGE     "icRaisingEdge"

#define GPIO      "GPIO"
#define SEPARATOR ','

enum _settings
{
  NR,
  FUNCTION,
  LEVEL,
  MEMORY,
  CFG
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
  ConfigOption(const char *key, const char *value, int maxLength);
  const char *getKey();
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
  char *_key;
  char *_value;
  int _maxLength;
};

class SuplaConfigManager {
 public:
  SuplaConfigManager();
  uint8_t addKey(const char *key, int maxLength);
  uint8_t addKey(const char *key, const char *value, int maxLength);
  uint8_t addKeyAndRead(const char *key, const char *value, int maxLength);
  uint8_t deleteKey(const char *key);
  uint8_t load();
  uint8_t loadItem(const char *key);
  uint8_t save();
  void showAllValue();

  ConfigOption *get(const char *key);
  bool set(const char *key, const char *value);
  bool setElement(const char *key, int index, int newvalue);

  bool isDeviceConfigured();
  void setGUIDandAUTHKEY();

 private:
  int _optionCount;
  ConfigOption *_options[CONFIG_MAX_OPTIONS];
};
#endif
