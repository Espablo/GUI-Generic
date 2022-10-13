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

#ifndef _Read_Values_SDM_h
#define _Read_Values_SDM_h

#include <Arduino.h>
#include <SDM.h>
#include <supla/element.h>

#if defined(ESP8266)
#include <SoftwareSerial.h>
#endif

namespace Supla {
namespace Sensor {

class ReadValuesSDM {
 public:
  ReadValuesSDM(int8_t pinRX = SDM_RX_PIN, int8_t pinTX = SDM_TX_PIN, long baud = SDM_UART_BAUD);

  // energy 1 == 0.00001 kWh
  unsigned _supla_int64_t getFwdActEnergy(int phase = 0);

  // energy 1 == 0.00001 kWh
  unsigned _supla_int64_t getRvrActEnergy(int phase = 0);

  // energy 1 == 0.00001 kWh
  unsigned _supla_int64_t getFwdReactEnergy(int phase = 0);

  // energy 1 == 0.00001 kWh
  unsigned _supla_int64_t getRvrReactEnergy(int phase = 0);

  // voltage 1 == 0.01 V
  unsigned _supla_int16_t getVoltage(int phase = 0);

  // current 1 == 0.001 A
  unsigned _supla_int_t getCurrent(int phase = 0);

  // Frequency 1 == 0.01 Hz
  unsigned _supla_int16_t getFreq();

  // power 1 == 0.00001 W
  _supla_int_t getPowerActive(int phase = 0);

  // power 1 == 0.00001 var
  _supla_int_t getPowerReactive(int phase = 0);

  // power 1 == 0.00001 VA
  _supla_int_t getPowerApparent(int phase = 0);

  // power 1 == 0.001
  _supla_int_t getPowerFactor(int phase = 0);

  // phase angle 1 == 0.1 degree
  _supla_int_t getPhaseAngle(int phase = 0);

  float sdmRead(uint16_t reg);

 protected:
#if defined(ESP8266)
  SoftwareSerial swSerSDM;  // config SoftwareSerial
#endif
  SDM sdm;  // config SDM
};

};  // namespace Sensor
};  // namespace Supla

#endif
