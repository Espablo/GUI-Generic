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

#include <Arduino.h>
#include "DirectLinksConnect.h"

namespace Supla {
namespace Sensor {

DirectLinksConnect::DirectLinksConnect(const char *url, const char *host, bool isSecured) : client(nullptr), lastReadTime(0) {
  this->setUrl(url);

  String server(host);
  auto npos = server.indexOf(":");

  String suplaServer = server.substring(0, npos);
  setHost(suplaServer.c_str());

  if (npos > 0) {
    String portServer = server.substring(npos + 1);
    setPort(portServer.toInt());
  }
  else {
    setPort(isSecured ? 443 : 80);
  }

  enableSSL(isSecured);
}

DirectLinksConnect::~DirectLinksConnect() {
  delete[] client;
}

void DirectLinksConnect::setHost(const char *host) {
  if (host) {
    strncpy(_host, host, MAX_HOST_SIZE);
  }
}

void DirectLinksConnect::setPort(uint16_t port) {
  _port = port;
}

void DirectLinksConnect::setUrl(const char *url) {
  if (url) {
    strncpy(_url, url, MAX_DIRECT_LINKS_SIZE);
  }
}

void DirectLinksConnect::enableSSL(bool isSecured) {
  _isSecured = isSecured;
}

bool DirectLinksConnect::openConnection() {
  return client->connect(_host, _port) ? true : false;
}

bool DirectLinksConnect::closeConnection() {
  client->stop();
  return checkConnection();
}

bool DirectLinksConnect::checkConnection() {
  return client->connected() == 1 ? true : false;
}

void DirectLinksConnect::toggleConnection() {
  if (client == NULL) {
    if (_isSecured) {
      client = new WiFiClientSecure();
      ((WiFiClientSecure *)client)->setInsecure();
#ifdef ARDUINO_ARCH_ESP8266
      ((WiFiClientSecure *)client)->setBufferSizes(1024, 512);
#endif
      ((WiFiClientSecure *)client)->setTimeout(10000);
    }
    else {
      client = new WiFiClient();
    }
  }

  if (checkConnection()) {
    closeConnection();
  }
  else {
    openConnection();
  }
}

void DirectLinksConnect::send() {
  toggleConnection();

  String request = String("GET /direct/") + _url + " HTTP/1.1\r\n" + "Host: " + _host + "\r\n" + "User-Agent: BuildFailureDetectorESP8266\r\n" +
                   "Connection: close\r\n\r\n";
  // Serial.println(request);
  sendRequest(request);

  toggleConnection();

  if (client) {
    delete client;
    client = nullptr;
  }
}

void DirectLinksConnect::sendRequest(const String &request) {
}

void DirectLinksConnect::iterateAlways() {
  if (millis() - lastReadTime > 30000) {
    lastReadTime = millis();
    send();
  }
}

};  // namespace Sensor
};  // namespace Supla
