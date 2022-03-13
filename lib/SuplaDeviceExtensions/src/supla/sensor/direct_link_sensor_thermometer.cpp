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

#include "direct_link_sensor_thermometer.h"

#include <Arduino.h>

namespace Supla {
namespace Sensor {

DirectLinksSensorThermometer::DirectLinksSensorThermometer(const char *host,
                                                           bool isSecured)
    : client(nullptr), _temp(TEMPERATURE_NOT_AVAILABLE), retryCount(0) {
  String server(host);
  auto npos = server.indexOf(":");

  String suplaServer = server.substring(0, npos);
  setHost(suplaServer.c_str());

  if (npos > 0) {
    String portServer = server.substring(npos + 1);
    setPort(portServer.toInt());
  } else {
    setPort(isSecured ? 443 : 80);
  }

  enableSSL(isSecured);
}

DirectLinksSensorThermometer::~DirectLinksSensorThermometer() {
  delete[] client;
}

void DirectLinksSensorThermometer::onInit() {
  channel.setNewValue(_temp);
}
void DirectLinksSensorThermometer::setHost(const char *host) {
  if (host) {
    strncpy(_host, host, MAX_HOST_SIZE);
  }
}

void DirectLinksSensorThermometer::setPort(uint16_t port) {
  _port = port;
}

void DirectLinksSensorThermometer::setUrl(const char *url) {
  if (url) {
    strncpy(_urlSensor, url, MAX_DIRECT_LINKS_SIZE);
  }
}

void DirectLinksSensorThermometer::enableSSL(bool isSecured) {
  _isSecured = isSecured;
}

bool DirectLinksSensorThermometer::openConnection() {
  return client->connect(_host, _port) ? true : false;
}

bool DirectLinksSensorThermometer::closeConnection() {
  client->stop();
  return checkConnection();
}

bool DirectLinksSensorThermometer::checkConnection() {
  return client->connected() == 1 ? true : false;
}

void DirectLinksSensorThermometer::toggleConnection() {
  if (client == NULL) {
    if (_isSecured) {
      client = new WiFiClientSecure();
      ((WiFiClientSecure *)client)->setInsecure();
#ifdef ARDUINO_ARCH_ESP8266
      ((WiFiClientSecure *)client)->setBufferSizes(256, 256);
#endif
      ((WiFiClientSecure *)client)->setTimeout(10000);
    } else {
      client = new WiFiClient();
    }
  }

  if (checkConnection()) {
    closeConnection();
  } else {
    openConnection();
  }
}

void DirectLinksSensorThermometer::sendRequest(const char *url) {
  if (client) {
    client->print(String("GET /direct/") + url + " HTTP/1.1\r\n" +
                  "Host: " + _host + "\r\n" +
                  "User-Agent: BuildFailureDetectorESP8266\r\n" +
                  "Connection: close\r\n\r\n");

    while (client->connected() || client->available()) {
      String line = client->readStringUntil('\n');
      if (line == "\r") {
        Serial.println(F("Direct links - Headers received"));
        break;
      }
    }

    String line = client->readStringUntil('}');
    line = line + "}";
    Serial.println(line);
    DynamicJsonBuffer jsonBuffer(200);
    JsonObject &root = jsonBuffer.parseObject(line);

    if (root.success()) {
      bool connected = root["connected"];
      // double Humi = root["humidity"];

      // Serial.print("connected ");
      // Serial.println(connected);
      // Serial.print("temperature ");
      // Serial.println(_temp);
      // Serial.print("humidity ");
      // Serial.println(Humi);

      if (!connected) {
        Serial.println(F("no connected sensor"));
        retryCount++;
        if (retryCount > 3) {
          retryCount = 0;
          _temp = TEMPERATURE_NOT_AVAILABLE;
        }
      } else {
        retryCount = 0;
        _temp = root["temperature"];
      }

    } else {
      Serial.println(F("parseObject - failed"));
      _temp = TEMPERATURE_NOT_AVAILABLE;
    }
  }
}

void DirectLinksSensorThermometer::send(const char *url) {
  toggleConnection();
  sendRequest(url);
  toggleConnection();

  if (client) {
    delete client;
    client = nullptr;
  }
}

double DirectLinksSensorThermometer::getValue() {
  send(_urlSensor);

  return _temp;
}

void DirectLinksSensorThermometer::iterateAlways() {
  if (millis() - lastReadTime > 20000) {
    lastReadTime = millis();
    channel.setNewValue(getValue());
  }
}

};  // namespace Sensor
};  // namespace Supla
