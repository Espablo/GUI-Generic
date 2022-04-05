
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

#include "PMSx003.h"

namespace Supla {
namespace Sensor {
PMSx003::PMSx003(int8_t pin_rx, int8_t pin_tx) {
  sensor = new SerialPM(PLANTOWER_AUTO, pin_rx, pin_tx);
  sensor->init();
}

void PMSx003::iterateAlways() {
  if (millis() - lastReadTime > 60000) {
    lastReadTime = millis();
    sensor->read();

    switch (sensor->status) {
      case this->sensor->OK:  // should never come here
        break;                // included to compile without warnings
      case this->sensor->ERROR_TIMEOUT:
        Serial.println(F(PMS_ERROR_TIMEOUT));
        break;
      case this->sensor->ERROR_MSG_UNKNOWN:
        Serial.println(F(PMS_ERROR_MSG_UNKNOWN));
        break;
      case this->sensor->ERROR_MSG_HEADER:
        Serial.println(F(PMS_ERROR_MSG_HEADER));
        break;
      case this->sensor->ERROR_MSG_BODY:
        Serial.println(F(PMS_ERROR_MSG_BODY));
        break;
      case this->sensor->ERROR_MSG_START:
        Serial.println(F(PMS_ERROR_MSG_START));
        break;
      case this->sensor->ERROR_MSG_LENGTH:
        Serial.println(F(PMS_ERROR_MSG_LENGTH));
        break;
      case this->sensor->ERROR_MSG_CKSUM:
        Serial.println(F(PMS_ERROR_MSG_CKSUM));
        break;
      case this->sensor->ERROR_PMS_TYPE:
        Serial.println(F(PMS_ERROR_PMS_TYPE));
        break;
    }
  }
}

void PMSx003::onInit() {
  sensor->read();
}

PMS_PM01::PMS_PM01(PMSx003 *sensor) {
  pmsx003 = sensor;
}

double PMS_PM01::getValue() {
  double value = TEMPERATURE_NOT_AVAILABLE;
  if (pmsx003->sensor->status == pmsx003->sensor->OK) {
    Serial.print(F("PM1.0 "));
    Serial.println(pmsx003->sensor->pm01);
    value = pmsx003->sensor->pm01;
  }
  return value;
}

PMS_PM25::PMS_PM25(PMSx003 *sensor) {
  pmsx003 = sensor;
}

double PMS_PM25::getValue() {
  double value = TEMPERATURE_NOT_AVAILABLE;
  if (pmsx003->sensor->status == pmsx003->sensor->OK) {
    Serial.print(F("PM2.5"));
    Serial.println(pmsx003->sensor->pm25);
    value = pmsx003->sensor->pm25;
  }
  return value;
}

PMS_PM10::PMS_PM10(PMSx003 *sensor) {
  pmsx003 = sensor;
}

double PMS_PM10::getValue() {
  double value = TEMPERATURE_NOT_AVAILABLE;
  if (pmsx003->sensor->status == pmsx003->sensor->OK) {
    Serial.print(F("PM10"));
    Serial.println(pmsx003->sensor->pm10);
    value = pmsx003->sensor->pm10;
  }
  return value;
}

};  // namespace Sensor
};  // namespace Supla