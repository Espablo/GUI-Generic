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

#ifndef _max_31855_h
#define _max_31855_h

#include <Adafruit_MAX31855.h>
#include <Arduino.h>

namespace Supla {
namespace Sensor {
class MAX31855 : public Thermometer {
 public:
  MAX31855(uint8_t pin_CLK, uint8_t pin_CS, uint8_t pin_DO) {
    thermocouple = new Adafruit_MAX31855(pin_CLK, pin_CS, pin_DO);
  };

  double getValue() {
    double value = TEMPERATURE_NOT_AVAILABLE;
    bool retryDone = false;
    do {
      if (!sensorStatus || isnan(value)) {
        sensorStatus = thermocouple->begin();
        retryDone = true;
      }
      value = TEMPERATURE_NOT_AVAILABLE;
      if (sensorStatus) {
        value = thermocouple->readCelsius();
      }
    } while (isnan(value) && !retryDone);
    return value;
  }

 private:
  void onInit() {
    channel.setNewValue(getValue());
  }

 protected:
  bool sensorStatus;
  ::Adafruit_MAX31855 *thermocouple;
};

};  // namespace Sensor
};  // namespace Supla

#endif