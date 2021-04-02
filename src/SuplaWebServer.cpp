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

String webContentBuffer;

SuplaWebServer::SuplaWebServer() {
  httpServer = new ESP8266WebServer(80);
#ifdef SUPLA_OTA
  httpUpdater = new ESP8266HTTPUpdateServer();
#endif
}

void SuplaWebServer::begin() {
  this->createWebServer();
  httpServer->onNotFound(std::bind(&SuplaWebServer::handleNotFound, this));
  httpServer->begin();
}

void SuplaWebServer::iterateAlways() {
  httpServer->handleClient();
}

void SuplaWebServer::createWebServer() {
  createWebPageHome();
  createWebPageDeviceSettings();

#ifdef GUI_RELAY
  WebPageRelay->createWebPageRelay();
#endif
#ifdef GUI_CONTROL
  WebPageControl->createWebPageControl();
#endif
#ifdef SUPLA_LIMIT_SWITCH
  createWebPageLimitSwitch();
#endif
#ifdef SUPLA_CONFIG
  WebPageConfig->createWebPageConfig();
#endif
#ifdef SUPLA_OTA
  httpUpdater->setup(httpServer, ConfigManager->get(KEY_LOGIN)->getValue(), ConfigManager->get(KEY_LOGIN_PASS)->getValue());
#endif

  createWebUpload();
  createWebTools();
  createWebPageOther();
  createWebPageSensors();
}

void SuplaWebServer::sendHeaderStart() {
  chunkedSendHeader = true;
  tcpCleanup();
  httpServer->sendHeader(F("Cache-Control"), F("no-cache, no-store, must-revalidate"));
  httpServer->sendHeader(F("Pragma"), F("no-cache"));
  httpServer->sendHeader(F("Expires"), F("-1"));
  httpServer->setContentLength(CONTENT_LENGTH_UNKNOWN);
  httpServer->chunkedResponseModeStart(200, F("text/html"));

  httpServer->sendContent_P(HTTP_META);
  httpServer->sendContent_P(HTTP_FAVICON);
  httpServer->sendContent_P(HTTP_STYLE);
  httpServer->sendContent_P(HTTP_LOGO);

  String summary = FPSTR(HTTP_SUMMARY);

  summary.replace(F("{h}"), ConfigManager->get(KEY_HOST_NAME)->getValue());
  summary.replace(F("{s}"), ConfigESP->getLastStatusMessageSupla());
  summary.replace(F("{v}"), Supla::Channel::reg_dev.SoftVer);
  summary.replace(F("{g}"), ConfigManager->get(KEY_SUPLA_GUID)->getValueHex(SUPLA_GUID_SIZE));
  summary.replace(F("{m}"), ConfigESP->getMacAddress(true));
  summary.replace(F("{f}"), String(ESP.getFreeHeap() / 1024.0));

  httpServer->sendContent(summary);
  httpServer->sendContent_P(HTTP_COPYRIGHT);
}

void SuplaWebServer::sendHeader() {
  if (!chunkedSendHeader)
    return;

  if (!webContentBuffer.isEmpty()) {
    httpServer->sendContent(webContentBuffer);
    webContentBuffer.clear();
    webContentBuffer = String();
    delay(0);
  }

#ifdef DEBUG_MODE
  Serial.printf_P(PSTR("Content size=%d\n"), webContentBuffer.length());
  Serial.printf_P(PSTR("Sent INDEX...Free mem=%d\n"), ESP.getFreeHeap());
#endif
}

void SuplaWebServer::sendHeaderEnd() {
  if (!chunkedSendHeader)
    return;

  sendHeader();
  httpServer->sendContent_P(HTTP_RBT);
  httpServer->chunkedResponseFinalize();

  tcpCleanup();
  httpServer->client().flush();
  httpServer->client().stop();
  chunkedSendHeader = false;

#ifdef DEBUG_MODE
  checkRAM();
#endif
}

void SuplaWebServer::sendContent() {
  sendHeaderStart();
  sendHeader();
  sendHeaderEnd();
}

void SuplaWebServer::handleNotFound() {
  httpServer->sendHeader("Location", PATH_START, true);
  handlePageHome(2);
  ConfigESP->rebootESP();
}

bool SuplaWebServer::isLoggedIn() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!httpServer->authenticate(ConfigManager->get(KEY_LOGIN)->getValue(), ConfigManager->get(KEY_LOGIN_PASS)->getValue()))
      httpServer->requestAuthentication();
    return true;
  }
  return true;
}

bool SuplaWebServer::saveGPIO(const String& _input, uint8_t function, uint8_t nr, const String& input_max) {
  uint8_t gpio, _gpio, _function, _nr, current_value, key;
  String input;
  input = _input;

  if (nr != 0) {
    input += nr;
  }
  else {
    nr = 1;
  }

  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") == 0) {
    return true;
  }

  gpio = ConfigESP->getGpio(nr, function);
  _gpio = WebServer->httpServer->arg(input).toInt();

  key = KEY_GPIO + _gpio;
  _function = ConfigManager->get(key)->getElement(FUNCTION).toInt();
  _nr = ConfigManager->get(key)->getElement(NR).toInt();

  if (_gpio == OFF_GPIO)
    ConfigESP->clearGpio(gpio, function);

  if (_gpio != OFF_GPIO) {
    if (_function == FUNCTION_OFF && _nr == FUNCTION_OFF) {
      ConfigESP->clearGpio(gpio, function);
      ConfigESP->clearGpio(_gpio, function);
      ConfigESP->setGpio(_gpio, nr, function);
    }
    else if (gpio == _gpio && _function == function && _nr == nr) {
      ConfigESP->setGpio(_gpio, nr, function);
    }
    else if (function == FUNCTION_CFG_BUTTON) {
      ConfigESP->setGpio(_gpio, FUNCTION_CFG_BUTTON);
    }
    else {
      return false;
    }
  }

  if (input_max != "\n") {
    current_value = WebServer->httpServer->arg(input_max).toInt();
    if (ConfigManager->get(key)->getElement(NR).toInt() > current_value) {
      ConfigESP->clearGpio(gpio, function);
    }
  }
  return true;
}

bool SuplaWebServer::saveGpioMCP23017(const String& _input, uint8_t function, uint8_t nr, const String& input_max) {
  uint8_t key, address, _address, gpio, _gpio, _function, _nr;
  String input = _input + nr;

  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") == 0) {
    return true;
  }

  address = ConfigESP->getAdressMCP23017(nr, function);
  if (nr <= 16)
    _address = WebServer->httpServer->arg(String(INPUT_ADRESS_MCP23017) + 1).toInt();
  if (nr >= 17)
    _address = WebServer->httpServer->arg(String(INPUT_ADRESS_MCP23017) + 17).toInt();

  gpio = ConfigESP->getGpioMCP23017(nr, function);
  _gpio = WebServer->httpServer->arg(input).toInt();

  key = KEY_GPIO + _gpio;
  _function = ConfigManager->get(key)->getElement(ConfigESP->getFunctionMCP23017(_address)).toInt();
  _nr = ConfigManager->get(key)->getElement(ConfigESP->getNrMCP23017(_address)).toInt();

  if (_gpio == OFF_GPIO || _address == OFF_MCP23017)
    ConfigESP->clearGpioMCP23017(gpio, nr, function);

  if (_gpio != OFF_GPIO && _address != OFF_MCP23017) {
    if (_function == FUNCTION_OFF && _nr == FUNCTION_OFF) {
      ConfigESP->clearGpioMCP23017(gpio, nr, function);
      ConfigESP->clearGpioMCP23017(_gpio, nr, function);
      ConfigESP->setGpioMCP23017(_gpio, _address, nr, function);
    }
    else if (gpio == _gpio && function == _function && nr == _nr) {
      ConfigESP->setGpioMCP23017(_gpio, _address, nr, function);
    }
    else {
      return false;
    }
  }
  return true;
}

#if defined(ESP8266)

struct tcp_pcb;
extern struct tcp_pcb* tcp_tw_pcbs;
extern "C" void tcp_abort(struct tcp_pcb* pcb);

void tcpCleanup() {
  while (tcp_tw_pcbs != NULL) {
    tcp_abort(tcp_tw_pcbs);
  }
}
#endif
