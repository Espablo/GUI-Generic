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

#define MAX_GPIO               13
#define OFF_GPIO               17

#define MAX_THERMOMETER         5

#define  PATH_START               "/"
#define  UPDATE_PATH              "/firmware"
#define  PATH_SET                 "set"
#define  PATH_UPDATE              "update"
#define  PATH_REBOT               "rbt"

#define  INPUT_WIFI_SSID          "sid"
#define  INPUT_WIFI_PASS          "wpw"
#define  INPUT_EMAIL              "eml"
#define  INPUT_SERVER             "svr"
#define  INPUT_HOSTNAME           "shn"
#define  INPUT_MODUL_LOGIN        "mlg"
#define  INPUT_MODUL_PASS         "mps"
#define  INPUT_ROLLERSHUTTER      "irsr"

class SuplaWebServer : public Supla::Element {
  public:
    SuplaWebServer();
    void begin();
 
    char www_username[MAX_MLOGIN];
    char www_password[MAX_MPASSWORD];
    char* update_path = (char*)UPDATE_PATH;

    const String SuplaMetas();
    const String SuplaStyle();
    const String SuplaFavicon();
    const String SuplaLogo();
    const String SuplaSummary();
    const String SuplaJavaScript(String java_return = PATH_START);
    const String SuplaCopyrightBar();
    const String SuplaSaveResult(int save);

    void sendContent(const String content);

    ESP8266WebServer httpServer = {80};
    ESP8266HTTPUpdateServer httpUpdater;

    const char* Supported_Gpio[18] = {
      "GPIO0-D3",
      "GPIO1-TX",
      "GPIO2-D4",
      "GPIO3-RX",
      "GPIO4-D2",
      "GPIO5-D1",
      "",
      "",
      "",
      "GPIO9-D11",
      "GPIO10-D12",
      "",
      "GPIO12-D6",
      "GPIO13-D7",
      "GPIO14-D5",
      "GPIO15-D8",
      "GPIO16-D0",
      "OFF",
    };

    const char* Supported_Level[2] = {
      "ODWRÓCONE",
      "NORMALNE"
    };

private:
    void iterateAlways();
    void handle();
    void handleSave();
    void handleFirmwareUp();    
    void createWebServer();

    String supla_webpage_start(int save);
    String supla_webpage_upddate();
    void supla_webpage_reboot();

    void rebootESP();
    void redirectToIndex();
};

#endif //SuplaWebServer_h
