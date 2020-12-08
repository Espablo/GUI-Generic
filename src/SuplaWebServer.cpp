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
#include "SuplaTemplateBoard.h"
#include "Markup.h"

SuplaWebServer::SuplaWebServer() {
}

void SuplaWebServer::begin() {
  this->createWebServer();

  strcpy(this->www_username, ConfigManager->get(KEY_LOGIN)->getValue());
  strcpy(this->www_password, ConfigManager->get(KEY_LOGIN_PASS)->getValue());

  httpServer.onNotFound(std::bind(&SuplaWebServer::handleNotFound, this));
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
#if defined(SUPLA_DS18B20) || defined(SUPLA_DHT11) || defined(SUPLA_DHT22) || defined(SUPLA_SI7021_SONOFF) || defined(SUPLA_BME280) || \
    defined(SUPLA_SHT3x) || defined(SUPLA_SI7021) || defined(SUPLA_MAX6675) || defined(SUPLA_HC_SR04) || defined(SUPLA_IMPULSE_COUNTER)
  WebPageSensor->createWebPageSensor();
#endif
#ifdef SUPLA_CONFIG
  WebPageConfig->createWebPageConfig();
#endif
#ifdef SUPLA_OTA
  httpUpdater.setup(&httpServer, this->www_username, this->www_password);
#endif

  createWebDownload();
  createWebUpload();
  createWebTools();
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
    ConfigESP->rebootESP();
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
  if (strcmp(WebServer->httpServer.arg(INPUT_ROLLERSHUTTER).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_ROLLERSHUTTER, httpServer.arg(INPUT_ROLLERSHUTTER).c_str());
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      //      Serial.println(F("E_CONFIG_OK: Dane zapisane"));
      if (ConfigESP->configModeESP == NORMAL_MODE) {
        this->sendContent(supla_webpage_start(1));
        ConfigESP->rebootESP();
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

  addFormHeader(content, S_SETTING_WIFI_SSID);
  addTextBox(content, INPUT_WIFI_SSID, S_WIFI_SSID, KEY_WIFI_SSID, 0, MAX_SSID, true);
  addTextBoxPassword(content, INPUT_WIFI_PASS, S_WIFI_PASS, KEY_WIFI_PASS, MIN_PASSWORD, MAX_PASSWORD, true);
  addTextBox(content, INPUT_HOSTNAME, S_HOST_NAME, KEY_HOST_NAME, 0, MAX_HOSTNAME, true);
  addFormHeaderEnd(content);

  addFormHeader(content, S_SETTING_SUPLA);
  addTextBox(content, INPUT_SERVER, S_SUPLA_SERVER, KEY_SUPLA_SERVER, DEFAULT_SERVER, 0, MAX_SUPLA_SERVER, true);
  addTextBox(content, INPUT_EMAIL, S_SUPLA_EMAIL, KEY_SUPLA_EMAIL, DEFAULT_EMAIL, 0, MAX_EMAIL, true);
  addFormHeaderEnd(content);

  addFormHeader(content, S_SETTING_ADMIN);
  addTextBox(content, INPUT_MODUL_LOGIN, S_LOGIN, KEY_LOGIN, 0, MAX_MLOGIN, true);
  addTextBoxPassword(content, INPUT_MODUL_PASS, S_LOGIN_PASS, KEY_LOGIN_PASS, MIN_PASSWORD, MAX_MPASSWORD, true);
  addFormHeaderEnd(content);

#ifdef SUPLA_ROLLERSHUTTER
  uint8_t maxrollershutter = ConfigManager->get(KEY_MAX_RELAY)->getValueInt();
  if (maxrollershutter >= 2) {
    content += F("<div class='w'>");
    content += F("<h3>");
    content += S_ROLLERSHUTTERS;
    content += F("</h3>");
    content += F("<i><label>");
    content += S_QUANTITY;
    content += F("</label><input name='");
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

  content += F("<button type='submit'>");
  content += S_SAVE;
  content += F("</button></form> ");
  content += F("<br>");

  addButton(content, S_DEVICE_SETTINGS, PATH_DEVICE_SETTINGS);
  addButton(content, "Tools", PATH_TOOLS);

  content += F("<form method='post' action='");
  content += PATH_REBOT;
  content += F("'>");
  content += F("<button type='submit'>");
  content += S_RESTART;
  content += F("</button></form></div>");
  return content;
}

void SuplaWebServer::supla_webpage_reboot() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!httpServer.authenticate(www_username, www_password))
      return httpServer.requestAuthentication();
  }
  this->sendContent(supla_webpage_start(2));
  ConfigESP->rebootESP();
}

String SuplaWebServer::deviceSettings(int save) {
  String content = "";

  content += WebServer->SuplaSaveResult(save);
  content += WebServer->SuplaJavaScript(PATH_DEVICE_SETTINGS);
  
  content += F("<form method='post' action='");
  content += PATH_SAVE_BOARD;
  content += F("'>");

  addFormHeader(content, S_TEMPLATE_BOARD);
  uint8_t selected = ConfigManager->get(KEY_BOARD)->getValueInt();
  addListBox(content, INPUT_BOARD, S_TYPE, BOARD_P, MAX_MODULE, selected);
  addFormHeaderEnd(content);

  content += F("<button type='submit'>");
  content += S_SAVE;
  content += F("</button></form><br><br>");

  addFormHeader(content, S_DEVICE_SETTINGS);
#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
  addButton(content, S_RELAYS, PATH_RELAY);
#endif

#ifdef SUPLA_BUTTON
  addButton(content, S_BUTTONS, PATH_CONTROL);
#endif

#if defined(SUPLA_DS18B20) || defined(SUPLA_DHT11) || defined(SUPLA_DHT22) || defined(SUPLA_SI7021_SONOFF)
  addButton(content, S_SENSORS_1WIRE, PATH_1WIRE);
#endif

#if defined(SUPLA_BME280) || defined(SUPLA_SHT30) || defined(SUPLA_SI7021)
  addButton(content, S_SENSORS_I2C, PATH_I2C);
#endif

#if defined(SUPLA_MAX6675)
  addButton(content, S_SENSORS_SPI, PATH_SPI);
#endif

#if defined(SUPLA_HC_SR04) || defined(SUPLA_IMPULSE_COUNTER)
  addButton(content, S_SENSORS_OTHER, PATH_OTHER);
#endif

#ifdef SUPLA_CONFIG
  addButton(content, S_LED_BUTTON_CFG, PATH_CONFIG);
#endif
  addFormHeaderEnd(content);
  addButton(content, S_RETURN, "");

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
    uint8_t key;
    for (nr = 0; nr <= 17; nr++) {
      key = KEY_GPIO + nr;
      ConfigManager->set(key, "0,0,0,0,0");
    }

    chooseTemplateBoard(WebServer->httpServer.arg(input).toInt());
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
#ifdef SUPLA_OTA
  java_script +=
      F("<script type='text/javascript'>if(window.top.location != window.location){"
        "window.top.location.href = window.location.href;}</script>\n");
#endif
  return java_script;
}

const String SuplaWebServer::SuplaSaveResult(int save) {
  if (save == 0)
    return F("");
  String saveresult = "";
  saveresult += F("<div id=\"msg\" class=\"c\">");
  if (save == 1) {
    saveresult += S_DATA_SAVED;
  }
  else if (save == 2) {
    saveresult += S_RESTART_MODULE;
  }
  else if (save == 3) {
    saveresult += S_DATA_ERASED_RESTART_DEVICE;
  }
  else if (save == 4) {
    saveresult += S_WRITE_ERROR_UNABLE_TO_READ_FILE_FS_PARTITION_MISSING;
  }
  else if (save == 5) {
    saveresult += S_DATA_SAVED_RESTART_MODULE;
  }
  else if (save == 6) {
    saveresult += S_WRITE_ERROR_BAD_DATA;
  }
  else if (save == 7) {
    saveresult += F("data saved");
  }
  saveresult += F("</div>");
  return saveresult;
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
  httpServer.sendContent_P(HTTP_COPYRIGHT);

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

  httpServer.chunkedResponseFinalize();
}

void SuplaWebServer::handleNotFound() {
  httpServer.sendHeader("Location", "/", true);
  httpServer.send(302, "text/plane", "");
}

bool SuplaWebServer::saveGPIO(const String& _input, uint8_t function, uint8_t nr, const String& input_max) {
  uint8_t current_value, key;
  String input;
  input = _input;
  if (nr != 0) {
    input += nr;
  }
  else {
    nr = 1;
  }

  key = KEY_GPIO + WebServer->httpServer.arg(input).toInt();

  if (ConfigESP->getGpio(nr, function) != WebServer->httpServer.arg(input).toInt() || WebServer->httpServer.arg(input).toInt() == OFF_GPIO) {
    ConfigESP->clearGpio(ConfigESP->getGpio(nr, function), function);
  }

  if (WebServer->httpServer.arg(input).toInt() != OFF_GPIO) {
    if (ConfigManager->get(key)->getElement(FUNCTION).toInt() == FUNCTION_OFF) {
      ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), nr, function, 1);
    }
    else if (ConfigESP->getGpio(nr, function) == WebServer->httpServer.arg(input).toInt() &&
             ConfigManager->get(key)->getElement(FUNCTION).toInt() == function) {
      ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), nr, function, ConfigESP->getLevel(nr, function));
    }
    else if (function == FUNCTION_CFG_BUTTON) {
      ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), FUNCTION_CFG_BUTTON);
    }
    else {
      return false;
    }
  }

  if (input_max != "\n") {
    current_value = WebServer->httpServer.arg(input_max).toInt();
    if (ConfigManager->get(key)->getElement(NR).toInt() > current_value) {
      ConfigESP->clearGpio(ConfigESP->getGpio(nr, function), function);
    }
  }
  return true;
}