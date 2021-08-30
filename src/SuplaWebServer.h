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

#include "GUI-Generic_Config.h"
#include "SuplaTemplateBoard.h"

#ifdef ARDUINO_ARCH_ESP8266
#ifdef SUPLA_OTA
#include "SuplaHTTPUpdateServer.h"
#endif
#include <ESP8266WebServer.h>

#elif ARDUINO_ARCH_ESP32
#ifdef SUPLA_OTA
#include "SuplaHTTPUpdateServerESP32.h"
#endif
#include <supla/ESP32WebServer/ESP32WebServer.h>
#endif

#include <supla/element.h>

#define ARG_PARM_URL    "url"
#define ARG_PARM_NUMBER "number"

#define PATH_START "/"

extern String webContentBuffer;

class SuplaWebServer : public Supla::Element {
 public:
  SuplaWebServer();
  void begin();
  void sendHeaderStart();
  void sendHeader();
  void sendHeaderEnd();

  void sendContent();

#ifdef ARDUINO_ARCH_ESP8266
  ESP8266WebServer* httpServer;

#ifdef SUPLA_OTA
  ESP8266HTTPUpdateServer* httpUpdater;
#endif
#elif ARDUINO_ARCH_ESP32
  ESP32WebServer* httpServer;

#ifdef SUPLA_OTA
  ESP32HTTPUpdateServer* httpUpdater;
#endif
#endif

  bool isLoggedIn();
  bool saveGPIO(const String& _input, uint8_t function, uint8_t nr = 0, const String& input_max = "\n");
#ifdef SUPLA_MCP23017
  bool saveGpioMCP23017(const String& _input, uint8_t function, uint8_t nr = 0, const String& input_max = "\n");
#endif

 private:
  void iterateAlways();
  void createWebServer();
  void handleNotFound();

  bool chunkedSendHeader = false;
};

#if defined(ESP8266)
#include <md5.h>
#endif
#if defined(ESP8266)

struct tcp_pcb;
extern struct tcp_pcb* tcp_tw_pcbs;
extern "C" void tcp_abort(struct tcp_pcb* pcb);

void tcpCleanup();
#endif
#endif  // SuplaWebServer_h