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

class ReadValuesSDM : public Element {
 public:
#if defined(ESP8266)
  ReadValuesSDM(int8_t pinRX, int8_t pinTX, long baud);
#else
  ReadValuesSDM(HardwareSerial& serial, int8_t pinRX, int8_t pinTX, long baud);
#endif
  // energy 1 == 0.00001 kWh  - one phase
  float getFwdActEnergy();

  // energy 1 == 0.00001 kWh  - one phase
  float getRvrActEnergy();

  // energy 1 == 0.00001 kWh  - one phase
  float getFwdReactEnergy();

  // energy 1 == 0.00001 kWh  - one phase
  float getRvrReactEnergy();

  // energy 1 == 0.00001 kWh
  float getFwdActEnergy(int phase);

  // energy 1 == 0.00001 kWh
  float getFwdActEnergyTotal();

  // energy 1 == 0.00001 kWh
  float getRvrActEnergy(int phase);

  // energy 1 == 0.00001 kWh
  float getFwdReactEnergy(int phase);

  // energy 1 == 0.00001 kWh
  float getFwdReactEnergyTotal();

  // energy 1 == 0.00001 kWh
  float getRvrReactEnergy(int phase);

  // voltage 1 == 0.01 V
  float getVoltage(int phase = 0);

  // current 1 == 0.001 A
  float getCurrent(int phase = 0);

  // Frequency 1 == 0.01 Hz
  float getFreq();

  // power 1 == 0.00001 W
  float getPowerActive(int phase = 0);

  // power 1 == 0.00001 var
  float getPowerReactive(int phase = 0);

  // power 1 == 0.00001 VA
  float getPowerApparent(int phase = 0);

  // power 1 == 0.001
  float getPowerFactor(int phase = 0);

  // phase angle 1 == 0.1 degree
  float getPhaseAngle(int phase = 0);

  float sdmRead(uint16_t reg);

  uint16_t getErrCode(bool _clear = false) {
    return sdm.getErrCode(_clear);
  }

  uint32_t getErrCount(bool _clear = false) {
    return sdm.getErrCount(_clear);
  }

  uint32_t getSuccCount(bool _clear = false) {
    return sdm.getSuccCount(_clear);
  }

  void clearErrCode() {
    sdm.clearErrCode();
  }

  void clearErrCount() {
    sdm.clearErrCount();
  }

  void clearSuccCount() {
    sdm.clearSuccCount();
  }

 protected:
  SDM sdm;  // config SDM

#if defined(ESP8266)
  SoftwareSerial swSerSDM;  // config SoftwareSerial
#endif
};

};  // namespace Sensor
};  // namespace Supla

#endif
