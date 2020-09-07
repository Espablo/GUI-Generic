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

#define  DEFAULT_HOSTNAME      "GUI Generic"  
  
#define  DEFAULT_LOGIN         "admin"
#define  DEFAULT_LOGIN_PASS    "pass"

#define  DEFAULT_SERVER         "svrX.supla.org"
#define  DEFAULT_EMAIL          "email@address.com"

#define KEY_SUPLA_GUID          "GUID"
#define KEY_SUPLA_AUTHKEY       "AUTHKEY"
#define KEY_WIFI_SSID           "wifiSSID"
#define KEY_WIFI_PASS           "wifiPass"
#define KEY_LOGIN               "login"
#define KEY_LOGIN_PASS          "loginPass"
#define KEY_HOST_NAME           "hostName"
#define KEY_SUPLA_SERVER        "suplaServer"
#define KEY_SUPLA_EMAIL         "suplaEmail"
#define KEY_DS                  "ds"
#define KEY_DS_NAME             "dsName"
#define KEY_MULTI_MAX_DS18B20   "multiMaxDs"
#define KEY_TYPE_BUTTON         "typeButton"
#define KEY_MONOSTABLE_TRIGGER  "trigger"
#define KEY_SUPLA_FUNCTION      "function"
#define KEY_RELAY               "relayGpio"
#define KEY_MAX_RELAY           "maxRelay"
#define KEY_BUTTON              "buttonGpio"
#define KEY_MAX_BUTTON          "maxButton"
#define KEY_LIMIT_SWITCH        "limitSwitchGpio"
#define KEY_MAX_LIMIT_SWITCH    "maxLimitSwitch"
#define KEY_DHT11               "dht11Gpio"
#define KEY_MAX_DHT11           "maxDht11"
#define KEY_DHT22               "dht22Gpio"
#define KEY_MAX_DHT22           "maxDht22"
#define KEY_MULTI_DS            "multiDsGpio"
#define KEY_BME280              "bme280"
#define KEY_ALTITUDE_BME280     "altbme280"
#define KEY_SDA                 "sdaGpio"
#define KEY_SCL                 "sclGpio"
#define KEY_CFG_LED             "cfgLedGpio"
#define KEY_CFG_BTN             "cfgBtnGpio"
#define KEY_CFG_LED_LEVEL       "cfgLedLevel"
#define KEY_RELAY_LEVEL         "cfgrRelayLevel"
#define KEY_TRIG                "trigGpio"
#define KEY_ECHO                "echoGpio"


#define  MAX_GUID                 SUPLA_GUID_SIZE
#define  MAX_AUTHKEY              SUPLA_GUID_SIZE
#define  MAX_SSID                 32
#define  MAX_PASSWORD             64
#define  MIN_PASSWORD             8
#define  MAX_MLOGIN               32
#define  MAX_MPASSWORD            64
#define  MAX_HOSTNAME             32
#define  MAX_SUPLA_SERVER         SUPLA_SERVER_NAME_MAXSIZE
#define  MAX_EMAIL                SUPLA_EMAIL_MAXSIZE
#define  MAX_DS18B20_ADDRESS_HEX  16
#define  MAX_DS18B20_ADDRESS      8
#define  MAX_DS18B20_NAME         8
#define  MAX_TYPE_BUTTON          4
#define  MAX_MONOSTABLE_TRIGGER   2
#define  MAX_FUNCTION             1

#define E_CONFIG_OK               0
#define E_CONFIG_FS_ACCESS        1
#define E_CONFIG_FILE_NOT_FOUND   2
#define E_CONFIG_FILE_OPEN        3
#define E_CONFIG_PARSE_ERROR      4
#define E_CONFIG_MAX              5

#define CONFIG_MAX_OPTIONS        188

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
    bool setElement(const char *key, const char value);

    bool isDeviceConfigured();
    void setGUIDandAUTHKEY();

  private:
    int _optionCount;
    ConfigOption *_options[CONFIG_MAX_OPTIONS];
};
#endif
