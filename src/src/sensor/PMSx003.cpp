
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

SerialPM *pms;

namespace Supla {
namespace Sensor {
PMSx003::PMSx003(int8_t pin_rx, int8_t pin_tx) {
  pms = new SerialPM(PLANTOWER_AUTO, pin_rx, pin_tx);
  Serial.println(F("Booted"));

  Serial.println(F("PMS sensor on SWSerial"));
  Serial.print(F("  RX:"));
  Serial.println(pin_rx);
  Serial.print(F("  TX:"));
  Serial.println(pin_tx);
  pms->init();
}

void PMSx003::iterateAlways() {
  if (millis() - lastReadTime > 10000) {
    lastReadTime = millis();
    pms->read();

    switch (pms->status) {
      case pms->OK:  // should never come here
        break;       // included to compile without warnings
      case pms->ERROR_TIMEOUT:
        Serial.println(F(PMS_ERROR_TIMEOUT));
        break;
      case pms->ERROR_MSG_UNKNOWN:
        Serial.println(F(PMS_ERROR_MSG_UNKNOWN));
        break;
      case pms->ERROR_MSG_HEADER:
        Serial.println(F(PMS_ERROR_MSG_HEADER));
        break;
      case pms->ERROR_MSG_BODY:
        Serial.println(F(PMS_ERROR_MSG_BODY));
        break;
      case pms->ERROR_MSG_START:
        Serial.println(F(PMS_ERROR_MSG_START));
        break;
      case pms->ERROR_MSG_LENGTH:
        Serial.println(F(PMS_ERROR_MSG_LENGTH));
        break;
      case pms->ERROR_MSG_CKSUM:
        Serial.println(F(PMS_ERROR_MSG_CKSUM));
        break;
      case pms->ERROR_PMS_TYPE:
        Serial.println(F(PMS_ERROR_PMS_TYPE));
        break;
    }
  }
}

void PMSx003::onInit() {
  new PMS_PM01();
  new PMS_PM25();
  new PMS_PM10();
}

PMS_PM01::PMS_PM01() {
}

double PMS_PM01::getValue() {
  double value = TEMPERATURE_NOT_AVAILABLE;
  if (pms->status == pms->OK) {
    Serial.print(F("PM1.0 "));
    Serial.println(pms->pm01);
    value = pms->pm01;
  }
  return value;
}

PMS_PM25::PMS_PM25() {
}

double PMS_PM25::getValue() {
  double value = TEMPERATURE_NOT_AVAILABLE;
  if (pms->status == pms->OK) {
    Serial.print(F("PM2.5"));
    Serial.println(pms->pm25);
    value = pms->pm25;
  }
  return value;
}

PMS_PM10::PMS_PM10() {
}

double PMS_PM10::getValue() {
  double value = TEMPERATURE_NOT_AVAILABLE;
  if (pms->status == pms->OK) {
    Serial.print(F("PM1.0"));
    Serial.println(pms->pm10);
    value = pms->pm10;
  }
  return value;
}

};  // namespace Sensor
};  // namespace Supla