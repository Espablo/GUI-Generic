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
#ifndef _VindriktningIkea_h
#define _VindriktningIkea_h

#include <Arduino.h>
#include <supla/sensor/thermometer.h>

#ifdef ARDUINO_ARCH_ESP8266
#include <SoftwareSerial.h>
#elif ARDUINO_ARCH_ESP32
#include <HardwareSerial.h>
#endif

#define VINDRIKTNING_IKEA_BAUDRATE 9600

struct particleSensorState_t {
  uint16_t avgPM25 = TEMPERATURE_NOT_AVAILABLE;
  uint16_t measurements[5] = {0, 0, 0, 0, 0};
  uint8_t measurementIdx = 0;
  boolean valid = false;
};

namespace Supla {
namespace Sensor {
class VindriktningIkea : public Thermometer {
 public:
#ifdef ARDUINO_ARCH_ESP32
  VindriktningIkea(HardwareSerial& serial);
#else
  VindriktningIkea(int8_t rxPin);
#endif

  double getValue();
  void iterateAlways();

 private:
#ifdef ARDUINO_ARCH_ESP8266
  SoftwareSerial* _serial = NULL;
#elif ARDUINO_ARCH_ESP32
  HardwareSerial* _serial = NULL;
#endif

  particleSensorState_t state;
  uint8_t serialRxBuf[255];
  uint8_t rxBufIdx = 0;

  void onInit();
  void clearRxBuf();
  void parseState(particleSensorState_t& state);
  bool isValidHeader();
  bool isValidChecksum();
  void handleUart(particleSensorState_t& state);
};
}  // namespace Sensor
}  // namespace Supla

#endif