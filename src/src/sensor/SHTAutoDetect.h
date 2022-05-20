/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

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

#ifndef _shtautodetect_h
#define _shtautodetect_h

// Dependency: ClosedCube SHTAutoDetect library - use library manager to install it
// https://github.com/Sensirion/arduino-sht

#include <Wire.h>
#include <Arduino.h>
#include <SHTSensor.h>
#include <supla/sensor/therm_hygro_meter.h>

namespace Supla {
namespace Sensor {
class SHTAutoDetect : public ThermHygroMeter {
 public:
  SHTAutoDetect() {
  }

  double getTemp() {
    return temperature;
  }

  double getHumi() {
    return humidity;
  }

 private:
  void iterateAlways() {
    if (millis() - lastReadTime > 10000) {
      lastReadTime = millis();
      readValuesFromDevice();
      channel.setNewValue(getTemp(), getHumi());
    }
  }

  void onInit() {
    if (sht.init()) {
      Serial.println("SHT Auto init: success");
    }
    else {
      Serial.println("SHT Auto init: failed");
    }
    sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM);  // only supported by SHT3x

    readValuesFromDevice();
    channel.setNewValue(getTemp(), getHumi());
  }

  void readValuesFromDevice() {
    if (!sht.readSample()) {
      Serial.println(F("SHT Auto: ERROR"));
      retryCount++;
      if (retryCount > 3) {
        retryCount = 0;
        temperature = TEMPERATURE_NOT_AVAILABLE;
        humidity = HUMIDITY_NOT_AVAILABLE;
      }
    }
    else {
      retryCount = 0;
      temperature = sht.getTemperature();
      humidity = sht.getHumidity();
    }
  }

 protected:
  double temperature = TEMPERATURE_NOT_AVAILABLE;
  double humidity = HUMIDITY_NOT_AVAILABLE;
  int8_t retryCount = 0;
  ::SHTSensor sht;  // I2C
};

};  // namespace Sensor
};  // namespace Supla

#endif
