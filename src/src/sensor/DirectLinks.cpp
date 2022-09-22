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
#include "DirectLinks.h"

namespace Supla {
namespace Sensor {

DirectLinksThermometer::DirectLinksThermometer(const char *url, const char *host, bool isSecured) : DirectLinksConnect(url, host, isSecured){};

void DirectLinksThermometer::sendRequest() {
  if (client) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(getRequest());

    if (root.success()) {
      if (!root["connected"] || root["temperature"]) {
        Serial.println(F("no connected sensor"));
        retryCount++;
        if (retryCount > 3) {
          retryCount = 0;
          temp = TEMPERATURE_NOT_AVAILABLE;
        }
      }
      else {
        retryCount = 0;
        temp = root["temperature"];
      }
    }
    else {
      Serial.println(F("parseObject - failed"));
      temp = TEMPERATURE_NOT_AVAILABLE;
    }
  }
}

double DirectLinksThermometer::getValue() {
  return temp;
}

void DirectLinksThermometer::onInit() {
  channel.setNewValue(getValue());
}

DirectLinksThermHygroMeter::DirectLinksThermHygroMeter(const char *url, const char *host, bool isSecured)
    : DirectLinksConnect(url, host, isSecured){};

void DirectLinksThermHygroMeter::sendRequest() {
  if (client) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(getRequest());

    if (root.success()) {
      if (!root["connected"] || !root["temperature"] || !root["humidity"]) {
        Serial.println(F("no connected sensor"));
        retryCount++;
        if (retryCount > 3) {
          retryCount = 0;
          temp = TEMPERATURE_NOT_AVAILABLE;
          humi = HUMIDITY_NOT_AVAILABLE;
        }
      }
      else {
        retryCount = 0;
        temp = root["temperature"];
        humi = root["humidity"];
      }
    }
    else {
      Serial.println(F("parseObject - failed"));
      temp = TEMPERATURE_NOT_AVAILABLE;
      humi = HUMIDITY_NOT_AVAILABLE;
    }
  }
}

double DirectLinksThermHygroMeter::getTemp() {
  return temp;
}

double DirectLinksThermHygroMeter::getHumi() {
  return humi;
}

void DirectLinksThermHygroMeter::onInit() {
  channel.setNewValue(getTemp(), getHumi());
}

DirectLinksPressMeter::DirectLinksPressMeter(const char *url, const char *host, bool isSecured) : DirectLinksConnect(url, host, isSecured){};

void DirectLinksPressMeter::sendRequest() {
  if (client) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(getRequest());

    if (root.success()) {
      if (!root["connected"] || !root["value"]) {
        Serial.println(F("no connected sensor"));
        retryCount++;
        if (retryCount > 3) {
          retryCount = 0;
          press = PRESSURE_NOT_AVAILABLE;
        }
      }
      else {
        retryCount = 0;
        press = root["value"];
      }
    }
    else {
      Serial.println(F("parseObject - failed"));
      press = PRESSURE_NOT_AVAILABLE;
    }
  }
}

double DirectLinksPressMeter::getValue() {
  return press;
}

void DirectLinksPressMeter::onInit() {
  channel.setNewValue(getValue());
}

DirectLinksOnePhaseElectricityMeter::DirectLinksOnePhaseElectricityMeter(const char *url, const char *host, bool isSecured)
    : DirectLinksConnect(url, host, isSecured){};

void DirectLinksOnePhaseElectricityMeter::sendRequest() {
  if (client) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(getRequest());

    if (root.success()) {
      bool connected = root["connected"];

      if (!connected) {
        Serial.println(F("no connected"));
        retryCount++;
        if (retryCount > 3) {
          retryCount = 0;
        }
      }
      else {
        JsonArray &phases = root["phases"];
        JsonObject &phases_0 = phases[0];

        setVoltage(0, (double)phases_0["voltage"] * 100);                           // voltage in 0.01 V
        setCurrent(0, (double)phases_0["current"] * 1000);                          // current in 0.001 A
        setPowerActive(0, (double)phases_0["powerActive"] * 100000);                // power in 0.00001 kW
        setPowerReactive(0, (double)phases_0["powerReactive"] * 100000);            // power in 0.00001 kvar
        setPowerApparent(0, (double)phases_0["powerApparent"] * 100000);            // power in 0.00001 kVA
        setPowerFactor(0, (double)phases_0["powerFactor"] * 1000);                  // power in 0.001
        setFwdActEnergy(0, (double)phases_0["totalForwardActiveEnergy"] * 100000);  // energy in 0.00001 kWh
        retryCount = 0;
      }
    }
    else {
      Serial.println(F("parseObject - failed"));
    }
  }
}

DirectLinksDistance::DirectLinksDistance(const char *url, const char *host, bool isSecured) : DirectLinksConnect(url, host, isSecured){};

void DirectLinksDistance::sendRequest() {
  if (client) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(getRequest());

    if (root.success()) {
      if (!root["connected"] || !root["distance"]) {
        Serial.println(F("no connected sensor"));
        retryCount++;
        if (retryCount > 3) {
          retryCount = 0;
          distance = DISTANCE_NOT_AVAILABLE;
        }
      }
      else {
        retryCount = 0;
        distance = root["distance"];
      }
    }
    else {
      Serial.println(F("parseObject - failed"));
      distance = DISTANCE_NOT_AVAILABLE;
    }
  }
}

double DirectLinksDistance::getValue() {
  return distance;
}

void DirectLinksDistance::onInit() {
  channel.setNewValue(getValue());
}

DirectLinksDepth::DirectLinksDepth(const char *url, const char *host, bool isSecured) : DirectLinksConnect(url, host, isSecured) {
  channel.setType(SUPLA_CHANNELTYPE_DISTANCESENSOR);
  channel.setDefault(SUPLA_CHANNELFNC_DEPTHSENSOR);
  channel.setNewValue(DISTANCE_NOT_AVAILABLE);
};

void DirectLinksDepth::sendRequest() {
  if (client) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(getRequest());

    if (root.success()) {
      if (!root["connected"] || !root["depth"]) {
        Serial.println(F("no connected sensor"));
        retryCount++;
        if (retryCount > 3) {
          retryCount = 0;
          depth = DISTANCE_NOT_AVAILABLE;
        }
      }
      else {
        retryCount = 0;
        depth = root["depth"];
      }
    }
    else {
      Serial.println(F("parseObject - failed"));
      depth = DISTANCE_NOT_AVAILABLE;
    }
  }
}

double DirectLinksDepth::getValue() {
  return depth;
}

void DirectLinksDepth::onInit() {
  channel.setNewValue(getValue());
}

};  // namespace Sensor
};  // namespace Supla