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
#include "VL_53L0X.h"

namespace Supla {
namespace Sensor {
VL_53L0X::VL_53L0X(int8_t address) : address(address), sensorStatus(false) {
  lox = new Adafruit_VL53L0X();
};

void VL_53L0X::onInit() {
  sensorStatus = lox->begin(address);
  if (!sensorStatus)
    Serial.println(F("Failed to detect and initialize sensor!"));
}

double VL_53L0X::getValue() {
  double value = DISTANCE_NOT_AVAILABLE;

  if (sensorStatus) {
    VL53L0X_RangingMeasurementData_t measure;

    lox->rangingTest(&measure, false);

    if (measure.RangeStatus != 4) {
      value = 0.001 * measure.RangeMilliMeter;
    }
  }

  return value;
}

void VL_53L0X::iterateAlways() {
  if (lastReadTime + 1000 < millis()) {
    lastReadTime = millis();
    channel.setNewValue(getValue());
  }
}

}  // namespace Sensor
}  // namespace Supla