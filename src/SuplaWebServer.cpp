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
#ifdef ARDUINO_ARCH_ESP8266
  httpServer = new ESP8266WebServer(80);
#ifdef SUPLA_OTA
  httpUpdater = new ESP8266HTTPUpdateServer();
  httpUpdater->setup(httpServer, ConfigManager->get(KEY_LOGIN)->getValue(), ConfigManager->get(KEY_LOGIN_PASS)->getValue());
#endif
#elif ARDUINO_ARCH_ESP32
  httpServer = new ESP32WebServer(80);
#ifdef SUPLA_OTA
  httpUpdater = new ESP32HTTPUpdateServer();
  httpUpdater->setup(httpServer, ConfigManager->get(KEY_LOGIN)->getValue(), ConfigManager->get(KEY_LOGIN_PASS)->getValue());
#endif
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
  createWebPageRelay();
#endif
#ifdef GUI_CONTROL
  createWebPageControl();
#endif
#ifdef SUPLA_LIMIT_SWITCH
  createWebPageLimitSwitch();
#endif
#ifdef SUPLA_CONFIG
  createWebPageConfig();
#endif

  createWebUpload();
  createWebTools();

#ifdef GUI_OTHER
  createWebPageOther();
#endif
  createWebPageSensors();
}

void SuplaWebServer::sendHeaderStart() {
  if (!chunkedSendHeader) {
    chunkedSendHeader = true;
#ifdef ARDUINO_ARCH_ESP8266
    tcpCleanup();
#endif
    httpServer->sendHeader(F("Cache-Control"), F("no-cache, no-store, must-revalidate"));
    httpServer->sendHeader(F("Pragma"), F("no-cache"));
    httpServer->sendHeader(F("Expires"), F("-1"));
    httpServer->setContentLength(CONTENT_LENGTH_UNKNOWN);
    httpServer->chunkedResponseModeStart(200, F("text/html"));

    httpServer->sendContent_P(HTTP_META);
    httpServer->sendContent_P(HTTP_FAVICON);
    httpServer->sendContent_P(HTTP_STYLE);
    httpServer->sendContent_P(HTTP_SCRIPT);
    httpServer->sendContent_P(HTTP_DIV_START);
    httpServer->sendContent_P(HTTP_IFRAMES);
    httpServer->sendContent_P(HTTP_LOGO);

    String summary = FPSTR(HTTP_SUMMARY);

    summary.replace(F("{h}"), ConfigManager->get(KEY_HOST_NAME)->getValue());
    summary.replace(F("{s}"), ConfigESP->getLastStatusMessageSupla());
    summary.replace(F("{v}"), Supla::Channel::reg_dev.SoftVer);
    summary.replace(F("{g}"), ConfigManager->get(KEY_SUPLA_GUID)->getValueHex(SUPLA_GUID_SIZE));
    summary.replace(F("{m}"), ConfigESP->getMacAddress(true));
    summary.replace(F("{f}"), String(ESP.getFreeHeap() / 1024.0));
    if (ConfigESP->configModeESP == NORMAL_MODE) {
      summary.replace(F("{c}"), "NORMAL");
    }
    else {
      summary.replace(F("{c}"), "CONFIG");
    }

    httpServer->sendContent(summary);
  }
}

void SuplaWebServer::sendHeader() {
  if (chunkedSendHeader) {
    if (!webContentBuffer.isEmpty()) {
      httpServer->sendContent(webContentBuffer);
      webContentBuffer.clear();
      webContentBuffer = String();
      delay(0);
    }
  }
}

void SuplaWebServer::sendHeaderEnd() {
  if (chunkedSendHeader) {
    sendHeader();
    addButton(webContentBuffer, S_RESTART, PATH_RESET_ESP);
    addButton(webContentBuffer, S_TOOLS, PATH_TOOLS);
    httpServer->sendContent_P(HTTP_DIV_END);
    httpServer->chunkedResponseFinalize();

#ifdef ARDUINO_ARCH_ESP8266
    tcpCleanup();
#endif
    httpServer->client().flush();
    httpServer->client().stop();
    chunkedSendHeader = false;

#ifdef DEBUG_MODE
    checkRAM();
#endif
  }
}

void SuplaWebServer::sendContent() {
  sendHeaderStart();
  sendHeader();
  sendHeaderEnd();
}

void SuplaWebServer::handleNotFound() {
  httpServer->sendHeader("Location", PATH_START, true);
  handlePageHome(2);
}

bool SuplaWebServer::isLoggedIn(bool force) {
  if (ConfigESP->configModeESP == NORMAL_MODE || force) {
    if (strcmp(ConfigManager->get(KEY_LOGIN)->getValue(), "") != 0 && strcmp(ConfigManager->get(KEY_LOGIN_PASS)->getValue(), "") != 0 &&
        !httpServer->authenticate(ConfigManager->get(KEY_LOGIN)->getValue(), ConfigManager->get(KEY_LOGIN_PASS)->getValue())) {
      httpServer->requestAuthentication();
      return false;
    }
  }
  return true;
}

bool SuplaWebServer::saveGPIO(const String& _input, uint8_t function, uint8_t nr, const String& input_max) {
  uint8_t gpio, _gpio, _function, _nr, current_value, key;
  String input;
  input = _input + nr;

  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") == 0) {
    return true;
  }

#ifdef GUI_SENSOR_I2C_EXPENDER
  ConfigManager->setElement(KEY_ACTIVE_EXPENDER, function, WebServer->httpServer->arg(INPUT_EXPENDER_TYPE).toInt());
#endif

  gpio = ConfigESP->getGpio(nr, function);
  _gpio = WebServer->httpServer->arg(input).toInt();

  // VIRTUAL RELAY
  if (function == FUNCTION_RELAY && _gpio == GPIO_VIRTUAL_RELAY) {
    if (gpio != GPIO_VIRTUAL_RELAY) {
      ConfigManager->setElement(KEY_VIRTUAL_RELAY, nr, false);
      ConfigESP->clearGpio(gpio, function);
    }

    ConfigManager->setElement(KEY_VIRTUAL_RELAY, nr, true);
    ConfigManager->setElement(KEY_NUMBER_BUTTON, nr, nr);

    if (input_max != "\n") {
      current_value = WebServer->httpServer->arg(input_max).toInt();
      if (nr >= current_value) {
        ConfigManager->setElement(KEY_VIRTUAL_RELAY, nr, false);
      }
    }

    return true;
  }

#ifdef ARDUINO_ARCH_ESP8266
  // ANALOG BUTTON
  if (function == FUNCTION_BUTTON && _gpio == A0) {
    if (gpio != A0) {
      ConfigManager->setElement(KEY_ANALOG_BUTTON, nr, false);
      ConfigESP->clearGpio(gpio, function);
    }

    ConfigManager->setElement(KEY_ANALOG_BUTTON, nr, true);

    if (input_max != "\n") {
      current_value = WebServer->httpServer->arg(input_max).toInt();
      if (nr >= current_value) {
        ConfigManager->setElement(KEY_ANALOG_BUTTON, nr, false);
      }
    }
    return true;
  }
#endif

  key = KEY_GPIO + _gpio;

  if (function == FUNCTION_CFG_BUTTON) {
    _function = ConfigManager->get(key)->getElement(CFG).toInt();
  }
  else {
    _function = ConfigManager->get(key)->getElement(FUNCTION).toInt();
    _nr = ConfigManager->get(key)->getElement(NR).toInt() - 1;
  }

  if (_gpio == OFF_GPIO) {
    ConfigESP->clearGpio(gpio, function);
    if (gpio == GPIO_VIRTUAL_RELAY) {
      ConfigManager->setElement(KEY_VIRTUAL_RELAY, nr, false);
    }
#ifdef ARDUINO_ARCH_ESP8266
    if (gpio == A0) {
      ConfigManager->setElement(KEY_ANALOG_BUTTON, nr, false);
    }
#endif
  }

  if (_gpio != OFF_GPIO) {
    if (_function == FUNCTION_OFF) {
      ConfigESP->clearGpio(gpio, function);
      ConfigESP->clearGpio(_gpio, function);
      ConfigESP->setGpio(_gpio, nr, function);
      if (gpio == GPIO_VIRTUAL_RELAY) {
        ConfigManager->setElement(KEY_VIRTUAL_RELAY, nr, false);
      }
      if (function == FUNCTION_BUTTON)
        ConfigManager->setElement(KEY_NUMBER_BUTTON, nr, nr);

#ifdef SUPLA_ROLLERSHUTTER
      if (ConfigManager->get(KEY_MAX_ROLLERSHUTTER)->getValueInt() * 2 > nr) {
        // if (nr % 2 == 0) {
        ConfigESP->setEvent(_gpio, Supla::Event::ON_PRESS);
        ConfigESP->setAction(_gpio, Supla::GUI::ActionRolleShutter::OPEN_OR_CLOSE);
        //  }
      }
#endif
    }
    else if (function == FUNCTION_CFG_BUTTON) {
      ConfigESP->setGpio(_gpio, FUNCTION_CFG_BUTTON);
    }
    else if (gpio == _gpio && _function == function && _nr == nr) {
      ConfigESP->setGpio(_gpio, nr, function);
    }
    else {
      return false;
    }
  }

  if (input_max != "\n") {
    current_value = WebServer->httpServer->arg(input_max).toInt();
    if ((ConfigManager->get(key)->getElement(NR).toInt() - 1) >= current_value) {
      ConfigESP->clearGpio(gpio, function);
    }
  }

  return true;
}

#ifdef GUI_SENSOR_I2C_EXPENDER
bool SuplaWebServer::saveGpioMCP23017(const String& _input, uint8_t function, uint8_t nr, const String& input_max) {
  uint8_t key, _address, gpio, _gpio, _function, _nr, _type, shiftAddress;
  String input = _input + "mcp" + nr;

  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") == 0) {
    return true;
  }

  _type = WebServer->httpServer->arg(INPUT_EXPENDER_TYPE).toInt();
  ConfigManager->setElement(KEY_ACTIVE_EXPENDER, function, _type);

  if (_type == FUNCTION_OFF) {
    ConfigESP->clearFunctionGpio(function);
    return true;
  }

  if (_type == EXPENDER_PCF8574) {
    shiftAddress = 8;
  }
  else {
    shiftAddress = 16;
  }

  if (nr < shiftAddress)
    _address = WebServer->httpServer->arg(String(INPUT_ADRESS_MCP23017) + 0).toInt();
  else
    _address = WebServer->httpServer->arg(String(INPUT_ADRESS_MCP23017) + shiftAddress).toInt();

  gpio = ConfigESP->getGpioMCP23017(nr, function);
  _gpio = WebServer->httpServer->arg(input).toInt();

  //  if ((nr == 0 || nr == shiftAddress) && _gpio == OFF_GPIO_EXPENDER)
  //    return false;

  key = KEY_GPIO + _gpio;
  _function = ConfigManager->get(key)->getElement(ConfigESP->getFunctionMCP23017(_address)).toInt();
  _nr = ConfigManager->get(key)->getElement(ConfigESP->getNrMCP23017(_address)).toInt();

  if (_gpio == OFF_GPIO_EXPENDER || _address == OFF_ADDRESS_MCP23017) {
    ConfigESP->clearGpioMCP23017(gpio, nr, function);
  }
  else if (_function == FUNCTION_OFF) {
    ConfigESP->clearGpioMCP23017(gpio, nr, function);
    ConfigESP->clearGpioMCP23017(_gpio, nr, function);
    ConfigESP->setGpioMCP23017(_gpio, _address, nr, function);
#ifdef SUPLA_ROLLERSHUTTER
    if (ConfigManager->get(KEY_MAX_ROLLERSHUTTER)->getValueInt() * 2 > nr) {
      if (nr % 2 == 0) {
        ConfigESP->setEvent(_gpio, Supla::Event::ON_PRESS);
        ConfigESP->setAction(_gpio, Supla::GUI::ActionRolleShutter::OPEN_OR_CLOSE);
      }
    }
#endif
  }
  else if (gpio == _gpio && function == _function && nr == _nr) {
    ConfigESP->setGpioMCP23017(_gpio, _address, nr, function);
  }
  else {
    return false;
  }
  return true;
}
#endif

#ifdef ARDUINO_ARCH_ESP8266

struct tcp_pcb;
extern struct tcp_pcb* tcp_tw_pcbs;
extern "C" void tcp_abort(struct tcp_pcb* pcb);

void tcpCleanup() {
  while (tcp_tw_pcbs != NULL) {
    tcp_abort(tcp_tw_pcbs);
  }
}
#endif
