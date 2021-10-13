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

#ifndef _hdc1080_h
#define _hdc1080_h

// Dependency: Adafruid HDC1080 library - use library manager to install it
// https://github.com/closedcube/ClosedCube_HDC1080_Arduino

#include <ClosedCube_HDC1080.h>
#include <Wire.h>
#include <supla/sensor/therm_hygro_meter.h>

namespace Supla {
namespace Sensor {
class HDC1080 : public ThermHygroMeter {
 public:
  HDC1080() {
  }

  double getTemp() {
    float value = TEMPERATURE_NOT_AVAILABLE;
    value = hdc1080.readTemperature();

    if (isnan(value)) {
      value = TEMPERATURE_NOT_AVAILABLE;
    }

    return value;
  }

  double getHumi() {
    float value = HUMIDITY_NOT_AVAILABLE;
    value = hdc1080.readHumidity();

    if (isnan(value)) {
      value = HUMIDITY_NOT_AVAILABLE;
    }

    return value;
  }

  void onInit() {
    hdc1080.begin(0x40);

    Serial.print("Manufacturer ID=0x");
    Serial.println(hdc1080.readManufacturerId(),
                   HEX);  // 0x5449 ID of Texas Instruments
    Serial.print("Device ID=0x");
    Serial.println(hdc1080.readDeviceId(), HEX);  // 0x1050 ID of the device

    Serial.print("Device Serial Number=");
    HDC1080_SerialNumber sernum = hdc1080.readSerialNumber();
    char format[15];
    sprintf(format,
            "%02X-%04X-%04X",
            sernum.serialFirst,
            sernum.serialMid,
            sernum.serialLast);
    Serial.println(format);

    channel.setNewValue(getTemp(), getHumi());
  }

 protected:
  ::ClosedCube_HDC1080 hdc1080;  // I2C
};

};  // namespace Sensor
};  // namespace Supla

#endif
