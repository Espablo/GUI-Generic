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

// https://github.com/arendst/Tasmota/blob/development/tasmota/tasmota_xsns_sensor/xsns_02_analog.ino

#ifndef _ntc10k_h
#define _ntc10k_h

#include <Arduino.h>
#include <supla/sensor/thermometer.h>

#ifdef ESP8266
#define ANALOG_RESOLUTION 10    // 12 = 4095, 11 = 2047, 10 = 1023
#define ANALOG_RANGE      1023  // 4095 = 12, 2047 = 11, 1023 = 10
#define ANALOG_PERCENT    10    // backward compatible div10 range
#endif                          // ESP8266
#ifdef ESP32
#undef ANALOG_RESOLUTION
#define ANALOG_RESOLUTION 12  // 12 = 4095, 11 = 2047, 10 = 1023
#undef ANALOG_RANGE
#define ANALOG_RANGE 4095  // 4095 = 12, 2047 = 11, 1023 = 10
#undef ANALOG_PERCENT
#define ANALOG_PERCENT ((ANALOG_RANGE + 50) / 100)  // approximation to 1% ADC range
#endif                                              // ESP32

#define TO_CELSIUS(x) ((x)-273.15)
#define TO_KELVIN(x)  ((x) + 273.15)

// Parameters for equation
#define ANALOG_V33 3.3              // ESP8266 / ESP32 Analog voltage
#define ANALOG_T0  TO_KELVIN(25.0)  // 25 degrees Celsius in Kelvin (= 298.15)

// Shelly 2.5 NTC Thermistor
// 3V3 --- ANALOG_NTC_BRIDGE_RESISTANCE ---v--- NTC --- Gnd
//                                         |
//                                        ADC0
#define ANALOG_NTC_BRIDGE_RESISTANCE 32000  // NTC Voltage bridge resistor
#define ANALOG_NTC_RESISTANCE        10000  // NTC Resistance
#define ANALOG_NTC_B_COEFFICIENT     3350   // NTC Beta Coefficient

namespace Supla {
namespace Sensor {
class NTC10K : public Thermometer {
 public:
  NTC10K(int8_t pin);
  double getValue();
  double TaylorLog(double x);
  uint16_t AdcRead(uint32_t pin, uint32_t factor);

 private:
  void onInit();

 protected:
  int8_t pin;
};
}  // namespace Sensor
}  // namespace Supla

#endif