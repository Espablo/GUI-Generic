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

#include "SDM_120.h"

namespace Supla {
namespace Sensor {

#if defined(ESP8266)
SDM120::SDM120(int8_t pinRX, int8_t pinTX, long baud) : ReadValuesSDM(pinRX, pinTX, baud){};
#else
SDM120::SDM120(HardwareSerial& serial, int8_t pinRX, int8_t pinTX, long baud) : ReadValuesSDM(serial, pinRX, pinTX, baud){};
#endif

void SDM120::onInit() {
  readValuesFromDevice();
  updateChannelValues();
}

void SDM120::readValuesFromDevice() {
  clearSuccCount();
  clearErrCount();
  clearErrCode();

  setFwdActEnergy(0, ReadValuesSDM::getFwdActEnergy() * 100000);
  setFwdReactEnergy(0, ReadValuesSDM::getFwdReactEnergy() * 100000);

  setRvrActEnergy(0, ReadValuesSDM::getRvrActEnergy() * 100000);
  setRvrReactEnergy(0, ReadValuesSDM::getRvrReactEnergy() * 100000);

  setVoltage(0, ReadValuesSDM::getVoltage() * 100);
  setCurrent(0, ReadValuesSDM::getCurrent() * 1000);
  setPowerActive(0, ReadValuesSDM::getPowerActive() * 100000);

  setFreq(ReadValuesSDM::getFreq() * 100);
  setPowerReactive(0, ReadValuesSDM::getPowerReactive() * 10000);
  setPowerApparent(0, ReadValuesSDM::getPowerApparent() * 100000);
  setPhaseAngle(0, ReadValuesSDM::getPhaseAngle() * 10);
  delay(0);
}

};  // namespace Sensor
};  // namespace Supla
