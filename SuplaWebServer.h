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

#ifndef SuplaWebServer_h
#define SuplaWebServer_h

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

#include <supla/element.h>
#include "SuplaConfigManager.h"

#define  GUI_BLUE               "#005c96"
#define  GUI_GREEN              "#00D151"

#define  DEFAULT_LOGIN         "admin"
#define  DEFAULT_PASSWORD      "password"

#define MAX_GPIO               16
#define OFF_GPIO               17

#define MAX_THERMOMETER         5

#define  UPDATE_PATH           "/firmware"
#define  PATH_START           "/"
#define  PATH_SET             "set"
//#define  PATH_SERCH           "serch"
//#define  PATH_SAVE_DS         "saveds"
#define  PATH_UPDATE          "update"
#define  PATH_REBOT           "rbt"
//#define  PATH_SENSOR           "sensor"
//#define  PATH_SAVE_SENSOR      "savesensor"
//#define  PATH_MULTI_DS          "multids"
//#define  PATH_SAVE_MULTI_DS     "savemultids"
#define  PATH_CONFIG            "config"
#define  PATH_SAVE_CONFIG       "saveconfig"

#define  INPUT_WIFI_SSID           "sid"
#define  INPUT_WIFI_PASS           "wpw"
#define  INPUT_EMAIL               "eml"
#define  INPUT_SERVER              "svr"
#define  INPUT_FUNCTION            "stp"
#define  INPUT_HOSTNAME            "shn"
#define  INPUT_MODUL_LOGIN         "mlg"
#define  INPUT_MODUL_PASS          "mps"
//#define  INPUT_MULTI_DS_GPIO       "mdsg"
//#define  INPUT_DHT11_GPIO          "dht11"
//#define  INPUT_DHT22_GPIO          "dht22"
#define  INPUT_CFG_LED_GPIO        "cfgl"
#define  INPUT_CFG_BTN_GPIO        "cfgb"
//#define  INPUT_SDA_GPIO            "sdag"
//#define  INPUT_SCL_GPIO            "sclg"
#define  INPUT_CFG_LED_LEVEL       "icll"
#define  INPUT_ROLLERSHUTTER       "irsr"

//#define  INPUT_BME280              "bme280"
//#define  INPUT_ALTITUDE_BME280      "abme280"
//
//#define  INPUT_TRIG_GPIO            "trig"
//#define  INPUT_ECHO_GPIO            "echo"
//
//#define  INPUT_MAX_DHT11            "mdht11"
//#define  INPUT_MAX_DHT22            "mdht22"


class SuplaWebServer : public Supla::Element {
  public:
    SuplaWebServer();
    void begin();
    void setBusyGpio(int gpio, int busy);
    int getBusyGpio(int gpio);
 
    char www_username[MAX_MLOGIN];
    char www_password[MAX_MPASSWORD];
    char* update_path = (char*)UPDATE_PATH;

    const String SuplaMetas();
    const String SuplaStyle();
    const String SuplaFavicon();
    const String SuplaLogo();
    const String SuplaSummary();
    const String SuplaJavaScript(String java_return = "/");
    const String SuplaCopyrightBar();
    const String SuplaSaveResult(int save);

    void sendContent(const String content);

    ESP8266WebServer httpServer = {80};
    ESP8266HTTPUpdateServer httpUpdater;

    char* Supported_Gpio[18][2] = {
      {"GPIO0-D3", "0"},
      {"GPIO1-TX", "0"},
      {"GPIO2-D4", "0"},
      {"GPIO3-RX", "0"},
      {"GPIO4-D2", "0"},
      {"GPIO5-D1", "0"},
      {"", "1"},
      {"", "1"},
      {"", "1"},
      {"GPIO9-D11", "0"},
      {"GPIO10-D12", "0"},
      {"", "1"},
      {"GPIO12-D6", "0"},
      {"GPIO13-D7", "0"},
      {"GPIO14-D5", "0"},
      {"GPIO15-D8", "0"},
      {"GPIO16-D0", "0"},
      {"OFF", "0"}
    };
    
    const char* Supported_Level[2] = {
      "ODWRÓCONE",
      "NORMALNE"
    };

private:
    void iterateAlways();
    void handle();
    void handleWizardSave();
    void handleSave();
    void handleSearchDS();
    void handleDSSave();
    void handleFirmwareUp();
    void handleSensor();
    void handleSensorSave();
    void handleConfig();
    void handleConfigSave();
    
    void createWebServer();

    String supla_webpage_start(int save);
    String supla_webpage_search(int save);
    String supla_webpage_upddate();
    String supla_webpage_sensor(int save);
    String supla_webpage_config(int save);
    void supla_webpage_reboot();

    void rebootESP();
    void redirectToIndex();
    
    const char* Supported_Type[2] = {
      "ROLETA",
      "ŚWIATŁA I INNE..."
    }; 
};

#endif //SuplaWebServer_h
