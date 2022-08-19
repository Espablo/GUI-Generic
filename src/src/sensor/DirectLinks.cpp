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
      bool connected = root["connected"];

      if (!connected) {
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
      bool connected = root["connected"];

      if (!connected) {
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
      bool connected = root["connected"];

      if (!connected) {
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

double DirectLinksPressMeter::getPressure() {
  return press;
}

void DirectLinksPressMeter::onInit() {
  pressureChannel.setNewValue(getPressure());
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

};  // namespace Sensor
};  // namespace Supla