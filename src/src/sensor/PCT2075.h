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

#ifndef SUPLA_SENSOR_PCT2075_H_
#define SUPLA_SENSOR_PCT2075_H_

// Dependency: Adafruit PCT2075 Library
// https://github.com/adafruit/Adafruit_PCT2075

#include <Adafruit_PCT2075.h>
#include <Wire.h>

#include <supla/sensor/thermometer.h>

namespace Supla {
namespace Sensor {
class PCT2075 : public Thermometer {
 public:
  explicit PCT2075(uint8_t i2c_addr = PCT2075_I2CADDR_DEFAULT, TwoWire *wire = &Wire) {
    pct = new Adafruit_PCT2075();

    if (pct->begin(i2c_addr, wire)) {
      Serial.println("Found PCT2075 chip");
    }
    else {
      Serial.println("Couldn't find PCT2075 chip");
    }

    pct->setFaultCount(PCT2075_FAULT_COUNT_4);  // since the loop timing and idle delay are in sync, it will take 4 loops to fault
  }

  double getValue() {
    Serial.print("Fault count set to: ");
    switch (pct->getFaultCount()) {
      case PCT2075_FAULT_COUNT_1:
        Serial.println("1");
        break;
      case PCT2075_FAULT_COUNT_2:
        Serial.println("2");
        break;
      case PCT2075_FAULT_COUNT_4:
        Serial.println("4");
        break;
      case PCT2075_FAULT_COUNT_6:
        Serial.println("6");
        break;
    }
    return pct->getTemperature();
  }

 protected:
  ::Adafruit_PCT2075 *pct;  // I2C
};

};  // namespace Sensor
};  // namespace Supla

#endif  // SRC_SUPLA_SENSOR_PCT2075_H_
