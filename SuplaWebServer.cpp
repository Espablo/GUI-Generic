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

#include "SuplaWebServer.h"
#include "SuplaDeviceGUI.h"
#include "SuplaWebPageConfig.h"
#include "SuplaWebPageControl.h"
#include "SuplaWebPageRelay.h"
#include "SuplaWebPageSensor.h"
#include "SuplaCommonPROGMEM.h"

SuplaWebServer::SuplaWebServer() {
}

void SuplaWebServer::begin() {
  this->createWebServer();

  strcpy(this->www_username, ConfigManager->get(KEY_LOGIN)->getValue());
  strcpy(this->www_password, ConfigManager->get(KEY_LOGIN_PASS)->getValue());

  httpUpdater.setup(&httpServer, UPDATE_PATH, www_username, www_password);
  httpServer.begin();
}

void SuplaWebServer::iterateAlways() {
  httpServer.handleClient();
}

void SuplaWebServer::createWebServer() {
  String path = PATH_START;
  httpServer.on(path, HTTP_GET, std::bind(&SuplaWebServer::handle, this));
  path = PATH_START;
  httpServer.on(path, std::bind(&SuplaWebServer::handleSave, this));
  path = PATH_START;
  path += PATH_UPDATE;
  httpServer.on(path, std::bind(&SuplaWebServer::handleFirmwareUp, this));
  path = PATH_START;
  path += PATH_REBOT;
  httpServer.on(path, std::bind(&SuplaWebServer::supla_webpage_reboot, this));
  path = PATH_START;
  path += PATH_DEVICE_SETTINGS;
  httpServer.on(path, std::bind(&SuplaWebServer::handleDeviceSettings, this));
  path = PATH_START;
  path += PATH_SAVE_BOARD;
  httpServer.on(path, std::bind(&SuplaWebServer::handleBoardSave, this));

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
  WebPageRelay->createWebPageRelay();
#endif
#if defined(SUPLA_BUTTON) || defined(SUPLA_LIMIT_SWITCH)
  WebPageControl->createWebPageControl();
#endif
#if defined(SUPLA_DS18B20) || defined(SUPLA_DHT11) || defined(SUPLA_DHT22) || defined(SUPLA_BME280) || defined(SUPLA_HC_SR04)
  WebPageSensor->createWebPageSensor();
#endif
#ifdef SUPLA_CONFIG
  WebPageConfig->createWebPageConfig();
#endif
}

void SuplaWebServer::handle() {
  //  Serial.println(F("HTTP_GET - metoda handle"));
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!httpServer.authenticate(this->www_username, this->www_password))
      return httpServer.requestAuthentication();
  }
  this->sendContent(supla_webpage_start(0));
}

void SuplaWebServer::handleSave() {
  //  Serial.println(F("HTTP_POST - metoda handleSave"));
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!httpServer.authenticate(this->www_username, this->www_password))
      return httpServer.requestAuthentication();
  }

  if (strcmp(httpServer.arg(PATH_REBOT).c_str(), "1") == 0) {
    this->rebootESP();
    return;
  }

  ConfigManager->set(KEY_WIFI_SSID, httpServer.arg(INPUT_WIFI_SSID).c_str());
  ConfigManager->set(KEY_WIFI_PASS, httpServer.arg(INPUT_WIFI_PASS).c_str());
  ConfigManager->set(KEY_SUPLA_SERVER, httpServer.arg(INPUT_SERVER).c_str());
  ConfigManager->set(KEY_SUPLA_EMAIL, httpServer.arg(INPUT_EMAIL).c_str());
  ConfigManager->set(KEY_HOST_NAME, httpServer.arg(INPUT_HOSTNAME).c_str());
  ConfigManager->set(KEY_LOGIN, httpServer.arg(INPUT_MODUL_LOGIN).c_str());
  ConfigManager->set(KEY_LOGIN_PASS, httpServer.arg(INPUT_MODUL_PASS).c_str());

#ifdef SUPLA_ROLLERSHUTTER
  ConfigManager->set(KEY_MAX_ROLLERSHUTTER, httpServer.arg(INPUT_ROLLERSHUTTER).c_str());
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      //      Serial.println(F("E_CONFIG_OK: Dane zapisane"));
      if (ConfigESP->configModeESP == NORMAL_MODE) {
        this->sendContent(supla_webpage_start(5));
        this->rebootESP();
      }
      else {
        this->sendContent(supla_webpage_start(7));
      }
      break;

    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      this->sendContent(supla_webpage_start(4));
      break;
  }
}

void SuplaWebServer::handleFirmwareUp() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!httpServer.authenticate(www_username, www_password))
      return httpServer.requestAuthentication();
  }
  this->sendContent(supla_webpage_upddate());
}

void SuplaWebServer::handleDeviceSettings() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!httpServer.authenticate(www_username, www_password))
      return httpServer.requestAuthentication();
  }
  this->sendContent(deviceSettings(0));
}

String SuplaWebServer::supla_webpage_start(int save) {
  String content = F("");
  content += SuplaSaveResult(save);
  content += SuplaJavaScript();
  content += F("<form method='post'>");
  content += F("<div class='w'>");
  content += F("<h3>Ustawienia WIFI</h3>");
  content += F("<i><input name='");
  content += INPUT_WIFI_SSID;
  content += F("' value='");
  content += String(ConfigManager->get(KEY_WIFI_SSID)->getValue());
  content += F("' length=");
  content += MAX_SSID;
  content += F(" required><label>Nazwa sieci</label></i>");
  content += F("<i><input name='");
  content += INPUT_WIFI_PASS;
  if (ConfigESP->configModeESP != NORMAL_MODE) {
    content += F("' type='password' ");
  }
  content += F("' value='");
  content += String(ConfigManager->get(KEY_WIFI_PASS)->getValue());
  content += F("'");

  if (ConfigManager->get(KEY_WIFI_PASS)->getValue() != 0) {
    content += F("required>");
  }
  else {
    content += F("'minlength='");
    content += MIN_PASSWORD;
    content += F("' length=");
    content += MAX_PASSWORD;
    content += F(" required>");
  }
  content += F("<label>Hasło</label></i>");
  content += F("<i><input name='");
  content += INPUT_HOSTNAME;
  content += F("' value='");
  content += ConfigManager->get(KEY_HOST_NAME)->getValue();
  content += F("' length=");
  content += MAX_HOSTNAME;
  content += F(" required><label>Nazwa modułu</label></i>");
  content += F("</div>");

  content += F("<div class='w'>");
  content += F("<h3>Ustawienia SUPLA</h3>");
  content += F("<i><input name='");
  content += INPUT_SERVER;
  content += F("' length=");
  content += MAX_SUPLA_SERVER;
  String def = DEFAULT_SERVER;
  String def_2 = String(ConfigManager->get(KEY_SUPLA_SERVER)->getValue());
  if (def == def_2) {
    content += F(" placeholder='");
  }
  else {
    content += F(" value='");
  }
  content += def_2;
  content += F("' required><label>Adres serwera</label></i>");
  content += F("<i><input name='");
  content += INPUT_EMAIL;
  content += F("' length=");
  content += MAX_EMAIL;
  def = DEFAULT_EMAIL;
  def_2 = String(ConfigManager->get(KEY_SUPLA_EMAIL)->getValue());
  if (def == def_2) {
    content += F(" placeholder='");
  }
  else {
    content += F(" value='");
  }
  content += def_2;
  content += F("' required><label>Email</label></i>");
  content += F("</div>");

  content += F("<div class='w'>");
  content += F("<h3>Ustawienia administratora</h3>");
  content += F("<i><input name='");
  content += INPUT_MODUL_LOGIN;
  content += F("' value='");
  content += String(ConfigManager->get(KEY_LOGIN)->getValue());
  content += F("' length=");
  content += MAX_MLOGIN;
  content += F("><label>Login</label></i>");
  content += F("<i><input name='");
  content += INPUT_MODUL_PASS;
  if (ConfigESP->configModeESP != NORMAL_MODE) {
    content += F("' type='password' ");
  }
  content += F("' value='");
  content += String(ConfigManager->get(KEY_LOGIN_PASS)->getValue());
  content += F("'");
  content += F("'minlength='");
  content += MIN_PASSWORD;
  content += F("' length=");
  content += MAX_MPASSWORD;
  content += F(" required>");
  content += F("<label>Hasło</label></i>");
  content += F("</div>");

#ifdef SUPLA_ROLLERSHUTTER
  uint8_t maxrollershutter = ConfigManager->get(KEY_MAX_RELAY)->getValueInt();
  if (maxrollershutter >= 2) {
    content += F("<div class='w'>");
    content += F("<h3>Rolety</h3>");
    content += F("<i><label>Ilość</label><input name='");
    content += INPUT_ROLLERSHUTTER;
    content += F("' type='number' placeholder='1' step='1' min='0' max='");
    content += maxrollershutter / 2;
    content += F("' value='");
    content += String(ConfigManager->get(KEY_MAX_ROLLERSHUTTER)->getValue());
    content += F("'></i>");
    content += F("</div>");
  }
#endif

#ifdef SUPLA_DS18B20
  WebPageSensor->showDS18B20(content, true);
#endif

  content += F("<button type='submit'>Zapisz</button></form>");
  content += F("<br>");
  content += F("<a href='");
  content += PATH_START;
  content += PATH_DEVICE_SETTINGS;
  content += F("'><button>Ustawienia urządzenia</button></a>");
  content += F("<br><br>");
  content += F("<a href='");
  content += PATH_START;
  content += PATH_UPDATE;
  content += F("'><button>Aktualizacja</button></a>");
  content += F("<br><br>");
  content += F("<form method='post' action='");
  content += PATH_REBOT;
  content += F("'>");
  content += F("<button type='submit'>Restart</button></form></div>");
  return content;
}

String SuplaWebServer::supla_webpage_upddate() {
  String content = "";
  content += F("<div class='w'>");
  content += F("<h3>Aktualizacja oprogramowania</h3>");
  content += F("<br>");
  content += F("<center>");
  content += F("<iframe src=");
  content += UPDATE_PATH;
  content +=
    F(">Twoja przeglądarka nie akceptuje ramek! width='200' height='100' "
      "frameborder='100'></iframe>");
  content += F("</center>");
  content += F("</div>");
  content += F("<a href='/'><button>Powrót</button></a></div>");

  return content;
}

void SuplaWebServer::supla_webpage_reboot() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!httpServer.authenticate(www_username, www_password))
      return httpServer.requestAuthentication();
  }
  this->sendContent(supla_webpage_start(2));
  this->rebootESP();
}

String SuplaWebServer::deviceSettings(int save) {
  String content = "";

  content += WebServer->SuplaSaveResult(save);
  content += WebServer->SuplaJavaScript(PATH_DEVICE_SETTINGS);
  content += F("<form method='post' action='");
  content += PATH_SAVE_BOARD;
  content += F("'>");
  content += F("<div class='w'><h3>Ustawienia płytki</h3>");
  content += F("<i><label>Rodzaj</label><select name='");
  content += INPUT_BOARD;
  content += F("'>");
  uint8_t selected = ConfigManager->get(KEY_BOARD)->getValueInt();
  for (uint8_t suported = 0; suported < 8; suported++) {
    content += F("<option value='");
    content += suported;
    if (selected == suported) {
      content += F("' selected>");
    }
    else
      content += F("'>");
    content += BoardString(suported);
  }
  content += F("</select></i>");
  content += F("</div><button type='submit'>Zapisz</button></form><br><br>");

  content += F("<div class='w'>");
  content += F("<h3>Ustawienia urządzenia</h3>");
  content += F("<br>");
  content += F("<center>");

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
  content += F("<a href='");
  content += PATH_START;
  content += PATH_RELAY;
  content += F("'><button>PRZEKAŹNIKI</button></a>");
  content += F("<br><br>");
#endif

#ifdef SUPLA_BUTTON
  content += F("<a href='");
  content += PATH_START;
  content += PATH_CONTROL;
  content += F("'><button>PRZYCISKI</button></a>");
  content += F("<br><br>");
#endif

#if defined(SUPLA_DS18B20) || defined(SUPLA_DHT11) || defined(SUPLA_DHT22) || defined(SUPLA_SI7021_SONOFF)
  content += F("<a href='");
  content += PATH_START;
  content += PATH_1WIRE;
  content += F("'><button>SENSORY 1Wire</button></a>");
  content += F("<br><br>");
#endif

#if defined(SUPLA_BME280) || defined(SUPLA_HC_SR04) || defined(SUPLA_SHT30) || defined(SUPLA_SI7021)
  content += F("<a href='");
  content += PATH_START;
  content += PATH_I2C;
  content += F("'><button>SENSORY i2c</button></a>");
  content += F("<br><br>");
#endif

#if defined(SUPLA_MAX6675)
  content += F("<a href='");
  content += PATH_START;
  content += PATH_SPI;
  content += F("'><button>SENSORY SPI</button></a>");
  content += F("<br><br>");
#endif

#ifdef SUPLA_CONFIG
  content += F("<a href='");
  content += PATH_START;
  content += PATH_CONFIG;
  content += F("'><button>LED, BUTTON CONFIG</button></a>");
  content += F("<br><br>");
#endif
  content += F("</div>");
  content += F("</center>");
  content += F("<a href='/'><button>Powrót</button></a></div>");

  return content;
}

void SuplaWebServer::handleBoardSave() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!httpServer.authenticate(this->www_username, this->www_password))
      return httpServer.requestAuthentication();
  }
  String input = INPUT_BOARD;

  if (strcmp(WebServer->httpServer.arg(input).c_str(), "") != 0) {
    ConfigManager->set(KEY_BOARD, httpServer.arg(input).c_str());

    int nr;
    String key;
    for (nr = 0; nr <= 17; nr++) {
      key = GPIO;
      key += nr;
      ConfigManager->set(key.c_str(), "0,0,0,0,0");
    }

    switch (WebServer->httpServer.arg(input).toInt()) {
      case BOARD_SONOFF_BASIC:
        ConfigManager->set(KEY_MAX_BUTTON, "1");
        ConfigESP->setGpio(14, 1, FUNCTION_BUTTON, Supla::ON_CHANGE);

        ConfigManager->set(KEY_MAX_RELAY, "1");
        ConfigESP->setGpio(12, 1, FUNCTION_RELAY, HIGH, MEMORY_RELAY_RESTORE);

        ConfigESP->setGpio(0, FUNCTION_CFG_BUTTON);
        ConfigESP->setGpio(13, FUNCTION_CFG_LED, HIGH);
        break;
      case BOARD_SONOFF_TH:
        break;
      case BOARD_SONOFF_TOUCH:
        break;
      case BOARD_SONOFF_TOUCH_2CH:
        break;
      case BOARD_SONOFF_TOUCH_3CH:
        break;
      case BOARD_SONOFF_4CH:
        ConfigManager->set(KEY_MAX_BUTTON, "4");
        ConfigESP->setGpio(0, 1, FUNCTION_BUTTON, Supla::ON_CHANGE);
        ConfigESP->setGpio(9, 2, FUNCTION_BUTTON, Supla::ON_CHANGE);
        ConfigESP->setGpio(10, 3, FUNCTION_BUTTON, Supla::ON_CHANGE);
        ConfigESP->setGpio(14, 4, FUNCTION_BUTTON, Supla::ON_CHANGE);

        ConfigManager->set(KEY_MAX_RELAY, "4");
        ConfigESP->setGpio(12, 1, FUNCTION_RELAY, HIGH, MEMORY_RELAY_RESTORE);
        ConfigESP->setGpio(5, 2, FUNCTION_RELAY, HIGH, MEMORY_RELAY_RESTORE);
        ConfigESP->setGpio(4, 3, FUNCTION_RELAY, HIGH, MEMORY_RELAY_RESTORE);
        ConfigESP->setGpio(15, 4, FUNCTION_RELAY, HIGH, MEMORY_RELAY_RESTORE);

        ConfigESP->setGpio(0, FUNCTION_CFG_BUTTON);
        //ConfigESP->setGpio(13, FUNCTION_CFG_LED, HIGH);
        break;
      case BOARD_YUNSHA:
        break;
      default:
        break;
    }
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      WebServer->sendContent(deviceSettings(1));
      break;
    case E_CONFIG_FILE_OPEN:
      WebServer->sendContent(deviceSettings(2));
      break;
  }
}

String SuplaWebServer::selectGPIO(const char* input, uint8_t function, uint8_t nr, uint8_t exeptionCfg) {
  String page = "";
  page += F("<select name='");
  page += input;
  if (nr != 0) {
    page += nr;
  }
  else {
    nr = 1;
  }
  page += F("'>");

  uint8_t selected = ConfigESP->getGpio(nr, function);
  uint8_t cfg = 0;
  for (uint8_t suported = 0; suported <= OFF_GPIO; suported++) {
    if (ConfigESP->checkBusyGpio(suported, function) == false || ConfigESP->checkBusyGpio(suported, exeptionCfg) == false || selected == suported) {
      page += F("<option value='");
      page += suported;
      if (selected == suported || (ConfigESP->getCfgFlag() == suported && selected == OFF_GPIO)) {
        if (cfg == 0) {
          page += F("' selected>");
          cfg = 1;
        }
        else {
          page += F("'>");
        }
      }
      else {
        page += F("'>");
      }
      page += GIPOString(suported);
    }
  }
  page += F("</select>");
  return page;
}

const String SuplaWebServer::SuplaFavicon() {
  //  return F("<link rel='icon'
  //  href='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgBAMAAACBVGfHAAAAB3RJTUUH5AUUCCQbIwTFfgAAAAlwSFlzAAALEgAACxIB0t1+/AAAAARnQU1BAACxjwv8YQUAAAAwUExURf7nNZuNIOvWMci2KWRbFJGEHnRpFy8rCdrGLSAdBgwLAod6G7inJkI8DVJLEKeYIg6cTsoAAAGUSURBVHjaY2CAAFUGNLCF4QAyl4mhmP8BB4LPcWtdAe+BEBiX9QD77Kzl24GKHCAC/OVZH5hkVyUCFQlCRJhnKjAwLVlVb8lQDOY/ZFrG8FDVQbVqbU8BWODc3BX8dbMMGJhfrUyAaOla+/dAP8jyncsbgJTKgVP/b+pOAUudegAkGMsrGZhE1EFyDGwLwNaucmZyl1TgKTdg4JvAwMBzn3txeKWrMwP7wQcMWiAtf2c9YDjUfYBJapsDw66bm4AiUesOnJty0/O9iwLDPI5EhhCD6/q3Chk4dgCleJYpAEOmfCkDB+sbsK1886YBRfgWMTBwbi896wR04YZuAyAH6OmzDCbr3RgYsj6A1HEBPXCfgWHONgaG6eUBII0LFTiA7jn+iIF/MbMTyEu3lphtAJtpvl4BTLPNWgVSySA+y28aWIDdyGtVBgNH5psshVawwHGGO+arLr7MYFoJjZr/zBPYj85a1sC4ulwAIsIdcJzh2qt1WReYBWBR48gxgd1ziQIi6hTYEsxR45pZwRU9+oWgNAB1F3c/H6bYqgAAAABJRU5ErkJggg=='
  //  type='image/x-png' />\n");
  return F("");
}

const String SuplaWebServer::SuplaIconEdit() {
  return F(
           "<img "
           "src='data:image/"
           "png;base64,"
           "iVBORw0KGgoAAAANSUhEUgAAAAwAAAAMCAQAAAD8fJRsAAAAB3RJTUUH5AYHChEfgNCVHgAAAAlwSFlzAAAuIwAALiMB"
           "eKU/dgAAAARnQU1BAACxjwv8YQUAAABBSURBVHjaY1BiwA4xhWqU/"
           "gMxAzZhEGRAF2ZQmoGpA6R6BlSaAV34P0QYIYEmDJPAEIZJQFxSg+"
           "kPDGFsHiQkAQDjTS5MMLyE4wAAAABJRU5ErkJggg=='>");
}

const String SuplaWebServer::SuplaJavaScript(String java_return) {
  String java_script =
    F("<script type='text/javascript'>setTimeout(function(){var "
      "element=document.getElementById('msg');if( element != "
      "null){element.style.visibility='hidden';location.href='");
  java_script += java_return;
  java_script += F("';}},1600);</script>\n");
  return java_script;
}

const String SuplaWebServer::SuplaSaveResult(int save) {
  if (save == 0)
    return F("");
  String saveresult = "";
  saveresult += F("<div id=\"msg\" class=\"c\">");
  if (save == 1) {
    saveresult += F("Dane zapisane");
  }
  else if (save == 2) {
    saveresult += F("Restart modułu");
  }
  else if (save == 3) {
    saveresult += F("Dane wymazane - należy zrobić restart urządzenia");
  }
  else if (save == 4) {
    saveresult += F("Błąd zapisu - nie można odczytać pliku - brak partycji FS.");
  }
  else if (save == 5) {
    saveresult += F("Dane zapisane - restart modułu.");
  }
  else if (save == 6) {
    saveresult += F("Błąd zapisu - złe dane.");
  }
  else if (save == 7) {
    saveresult += F("data saved");
  }
  saveresult += F("</div>");
  return saveresult;
}

void SuplaWebServer::rebootESP() {
  wdt_reset();
  ESP.restart();
  while (1) wdt_reset();
}

void SuplaWebServer::sendContent(const String content) {
  // httpServer.send(200, "text/html", "");
  const int bufferSize = 1000;
  String _buffer;
  int bufferCounter = 0;
  int fileSize = content.length();

#ifdef DEBUG_MODE
  Serial.print("Content size: ");
  Serial.println(fileSize);
#endif

  httpServer.setContentLength(fileSize);
  httpServer.chunkedResponseModeStart(200, "text/html");

  httpServer.sendContent_P(HTTP_META);
  httpServer.sendContent_P(HTTP_STYLE);
  httpServer.sendContent_P(HTTP_LOGO);

  String summary = FPSTR(HTTP_SUMMARY);
  summary.replace("{h}", ConfigManager->get(KEY_HOST_NAME)->getValue());
  summary.replace("{s}", ConfigESP->getLastStatusSupla());
  summary.replace("{v}", Supla::Channel::reg_dev.SoftVer);
  summary.replace("{g}", ConfigManager->get(KEY_SUPLA_GUID)->getValueHex(SUPLA_GUID_SIZE));
  summary.replace("{m}", ConfigESP->getMacAddress(true));
  httpServer.sendContent(summary);

  // httpServer.send(200, "text/html", "");
  for (int i = 0; i < fileSize; i++) {
    _buffer += content[i];
    bufferCounter++;

    if (bufferCounter >= bufferSize) {
      httpServer.sendContent(_buffer);
      yield();
      bufferCounter = 0;
      _buffer = "";
    }
  }
  if (bufferCounter > 0) {
    httpServer.sendContent(_buffer);
    yield();
    bufferCounter = 0;
    _buffer = "";
  }
  httpServer.sendContent_P(HTTP_COPYRIGHT);

  httpServer.chunkedResponseFinalize();
}

void SuplaWebServer::redirectToIndex() {
  httpServer.sendHeader("Location", "/", true);
  httpServer.send(302, "text/plain", "");
  httpServer.client().stop();
}
