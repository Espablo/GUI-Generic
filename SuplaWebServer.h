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

#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <supla/element.h>

#include "SuplaConfigManager.h"

#define GUI_BLUE  "#005c96"
#define GUI_GREEN "#00D151"

#define DEFAULT_LOGIN    "admin"
#define DEFAULT_PASSWORD "password"

#define MAX_GPIO 13
#define OFF_GPIO 17

#define PATH_START            "/"
#define PATH_SAVE_LOGIN       "savelogin"
#define UPDATE_PATH           "/firmware"
#define PATH_UPDATE           "update"
#define PATH_REBOT            "rbt"
#define PATH_DEVICE_SETTINGS  "devicesettings"
#define PATH_DEFAULT_SETTINGS "defaultsettings"
#define PATH_LOGIN_SETTINGS   "loginsettings"

#define INPUT_WIFI_SSID     "sid"
#define INPUT_WIFI_PASS     "wpw"
#define INPUT_EMAIL         "eml"
#define INPUT_SERVER        "svr"
#define INPUT_HOSTNAME      "shn"
#define INPUT_MODUL_LOGIN   "mlg"
#define INPUT_MODUL_PASS    "mps"
#define INPUT_ROLLERSHUTTER "irsr"

const char HTTP_META[] PROGMEM = "<!DOCTYPE HTML><meta http-equiv='content-type' content='text/html; charset=UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no'>\n";
const char HTTP_STYLE[] PROGMEM = "<style>body{font-size:14px;font-family:HelveticaNeue,'Helvetica Neue',HelveticaNeueRoman,HelveticaNeue-Roman,'Helvetica Neue Roman',TeXGyreHerosRegular,Helvetica,Tahoma,Geneva,Arial,sans-serif;font-weight:400;font-stretch:normal;background:#00d151;color:#fff;line-height:20px;padding:0}.s{width:460px;margin:0 auto;margin-top:calc(50vh - 340px);border:solid 3px #fff;padding:0 10px 10px;border-radius:3px}#l{display:block;max-width:150px;height:155px;margin:-80px auto 20px;background:#00d151;padding-right:5px}#l path{fill:#000}.w{margin:3px 0 16px;padding:5px 0;border-radius:3px;background:#fff;box-shadow:0 1px 3px rgba(0,0,0,.3)}h1,h3{margin:10px 8px;font-family:HelveticaNeueLight,HelveticaNeue-Light,'Helvetica Neue Light',HelveticaNeue,'Helvetica Neue',TeXGyreHerosRegular,Helvetica,Tahoma,Geneva,Arial,sans-serif;font-weight:300;font-stretch:normal;color:#000;font-size:23px}h1{margin-bottom:14px;color:#fff}span{display:block;margin:10px 7px 14px}i{display:block;font-style:normal;position:relative;border-bottom:solid 1px #00d151;height:42px}i:last-child{border:none}label{position:absolute;display:inline-block;top:0;left:8px;color:#00d151;line-height:41px;}input,select{width:calc(100% - 145px);border:none;font-size:16px;line-height:40px;border-radius:0;letter-spacing:-.5px;background:#fff;color:#000;padding-left:143px;-webkit-appearance:none;-moz-appearance:none;appearance:none;outline:0!important;height:40px}select{padding:0;float:right;margin:1px 3px 1px 2px}button{width:100%;border:0;background:#000;padding:5px 10px;font-size:16px;line-height:40px;color:#fff;border-radius:3px;box-shadow:0 1px 3px rgba(0,0,0,.3);cursor:pointer}.c{background:#ffe836;position:fixed;width:100%;line-height:80px;color:#000;top:0;left:0;box-shadow:0 1px 3px rgba(0,0,0,.3);text-align:center;font-size:26px;z-index:100}@media all and (max-height:920px){.s{margin-top:80px}}@media all and (max-width:900px){.s{width:calc(100% - 20px);margin-top:40px;border:none;padding:0 8px;border-radius:0}#l{max-width:80px;height:auto;margin:10px auto 20px}h1,h3{font-size:19px}i{border:none;height:auto}label{display:block;margin:4px 0 12px;color:#00d151;font-size:13px;position:relative;line-height:18px}input,select{width:calc(100% - 10px);font-size:16px;line-height:28px;padding:0 5px;border-bottom:solid 1px #00d151}select{width:100%;float:none;margin:0}}</style>";
const char HTTP_LOGO[] PROGMEM = "<div class='s'><svg version='1.1' id='l' x='0' y='0' viewBox='0 0 200 200' xml:space='preserve'><path d='M59.3,2.5c18.1,0.6,31.8,8,40.2,23.5c3.1,5.7,4.3,11.9,4.1,18.3c-0.1,3.6-0.7,7.1-1.9,10.6c-0.2,0.7-0.1,1.1,0.6,1.5c12.8,7.7,25.5,15.4,38.3,23c2.9,1.7,5.8,3.4,8.7,5.3c1,0.6,1.6,0.6,2.5-0.1c4.5-3.6,9.8-5.3,15.7-5.4c12.5-0.1,22.9,7.9,25.2,19c1.9,9.2-2.9,19.2-11.8,23.9c-8.4,4.5-16.9,4.5-25.5,0.2c-0.7-0.3-1-0.2-1.5,0.3c-4.8,4.9-9.7,9.8-14.5,14.6c-5.3,5.3-10.6,10.7-15.9,16c-1.8,1.8-3.6,3.7-5.4,5.4c-0.7,0.6-0.6,1,0,1.6c3.6,3.4,5.8,7.5,6.2,12.2c0.7,7.7-2.2,14-8.8,18.5c-12.3,8.6-30.3,3.5-35-10.4c-2.8-8.4,0.6-17.7,8.6-22.8c0.9-0.6,1.1-1,0.8-2c-2-6.2-4.4-12.4-6.6-18.6c-6.3-17.6-12.7-35.1-19-52.7c-0.2-0.7-0.5-1-1.4-0.9c-12.5,0.7-23.6-2.6-33-10.4c-8-6.6-12.9-15-14.2-25c-1.5-11.5,1.7-21.9,9.6-30.7C32.5,8.9,42.2,4.2,53.7,2.7c0.7-0.1,1.5-0.2,2.2-0.2C57,2.4,58.2,2.5,59.3,2.5z M76.5,81c0,0.1,0.1,0.3,0.1,0.6c1.6,6.3,3.2,12.6,4.7,18.9c4.5,17.7,8.9,35.5,13.3,53.2c0.2,0.9,0.6,1.1,1.6,0.9c5.4-1.2,10.7-0.8,15.7,1.6c0.8,0.4,1.2,0.3,1.7-0.4c11.2-12.9,22.5-25.7,33.4-38.7c0.5-0.6,0.4-1,0-1.6c-5.6-7.9-6.1-16.1-1.3-24.5c0.5-0.8,0.3-1.1-0.5-1.6c-9.1-4.7-18.1-9.3-27.2-14c-6.8-3.5-13.5-7-20.3-10.5c-0.7-0.4-1.1-0.3-1.6,0.4c-1.3,1.8-2.7,3.5-4.3,5.1c-4.2,4.2-9.1,7.4-14.7,9.7C76.9,80.3,76.4,80.3,76.5,81z M89,42.6c0.1-2.5-0.4-5.4-1.5-8.1C83,23.1,74.2,16.9,61.7,15.8c-10-0.9-18.6,2.4-25.3,9.7c-8.4,9-9.3,22.4-2.2,32.4c6.8,9.6,19.1,14.2,31.4,11.9C79.2,67.1,89,55.9,89,42.6z M102.1,188.6c0.6,0.1,1.5-0.1,2.4-0.2c9.5-1.4,15.3-10.9,11.6-19.2c-2.6-5.9-9.4-9.6-16.8-8.6c-8.3,1.2-14.1,8.9-12.4,16.6C88.2,183.9,94.4,188.6,102.1,188.6z M167.7,88.5c-1,0-2.1,0.1-3.1,0.3c-9,1.7-14.2,10.6-10.8,18.6c2.9,6.8,11.4,10.3,19,7.8c7.1-2.3,11.1-9.1,9.6-15.9C180.9,93,174.8,88.5,167.7,88.5z'/></svg>";
const char HTTP_SUMMARY[] PROGMEM = "<h1>{h}</h1><span>LAST STATE: {s}<br>Firmware: SuplaDevice {v}<br>GUID: {g}<br>MAC: {m}</span>\n";
const char HTTP_COPYRIGHT[] PROGMEM = "<a target='_blank' rel='noopener noreferrer' href='https://forum.supla.org/viewtopic.php?f=11&t=7133'><span style='color: #ffffff !important;'>https://forum.supla.org/</span></a>\n";
  
class SuplaWebServer : public Supla::Element {
  public:
    SuplaWebServer();
    void begin();

    char www_username[MAX_MLOGIN];
    char www_password[MAX_MPASSWORD];
    char* update_path = (char*)UPDATE_PATH;

    const String SuplaFavicon();
    const String SuplaIconEdit();
    const String SuplaJavaScript(String java_return = PATH_START);
    const String SuplaSaveResult(int save);

    void sendContent(const String content);
    void rebootESP();

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
  void handleWizardSave();
  void handleFirmwareUp();
  void handleDeviceSettings();
  void handleDefaultSettings();
  void handleLoginSettings();
  void createWebServer();

  String supla_webpage_start(int save);
  String supla_webpage_upddate();
  void supla_webpage_reboot();
  String deviceSettings();
  String loginSettings();

  void redirectToIndex();
};

#endif  // SuplaWebServer_h
