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

#ifndef SUPLA_SENSOR_LM75_H_
#define SUPLA_SENSOR_LM75_H_

#define USE_TWO_WIRE_FOR_LM75A

// Dependency: QuentinCG LM75 Library
// https://github.com/QuentinCG/Arduino-LM75-Temperature-Sensor-Library

#include <LM75A.h>
#include <Wire.h>

#include <supla/sensor/thermometer.h>

namespace Supla {
namespace Sensor {
class LM75 : public Thermometer {
 public:
  explicit LM75(uint8_t i2c_addr = LM75A_BASE_ADDRESS, TwoWire *wire = &Wire) {
    lm = new LM75A(wire, i2c_addr);

    wire->beginTransmission(i2c_addr);
    if (wire->endTransmission()) {
      Serial.println("Found LM75 chip");
    }
    else {
      Serial.println("Couldn't find LM75 chip");
    }
  }

  double getValue() {
    double temp = lm->getTemperatureInDegrees();

    if (temp == INVALID_LM75A_TEMPERATURE) {
      temp = TEMPERATURE_NOT_AVAILABLE;
      Serial.println("[LM75] Error while getting temperature");
    }

    return temp;
  }

 protected:
  ::LM75A *lm;  // I2C
};

};  // namespace Sensor
};  // namespace Supla

#endif  // SRC_SUPLA_SENSOR_LM75_H_
