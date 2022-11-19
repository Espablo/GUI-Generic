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

#include "SDM_630.h"

namespace Supla {
namespace Sensor {

#if defined(ESP8266)
SDM630::SDM630(int8_t pinRX, int8_t pinTX) : ReadValuesSDM(pinRX, pinTX, 9600){};
#else
SDM630::SDM630(HardwareSerial& serial, int8_t pinRX, int8_t pinTX) : ReadValuesSDM(serial, pinRX, pinTX, 9600){};
#endif

void SDM630::onInit() {
  readValuesFromDevice();
  updateChannelValues();
}

void SDM630::readValuesFromDevice() {
  clearSuccCount();
  clearErrCount();
  clearErrCode();

  for (int i = 0; i < MAX_PHASES; i++) {
    setFwdActEnergy(i, ReadValuesSDM::getFwdActEnergy(i) * 100000); //energia czynna pobrana
    setRvrActEnergy(i, ReadValuesSDM::getRvrActEnergy(i) * 100000); //energia czynna zwrócona

     setFwdReactEnergy(i, ReadValuesSDM::getFwdReactEnergy(i) * 100000);
     setRvrReactEnergy(i, ReadValuesSDM::getRvrReactEnergy(i) * 100000);

    setVoltage(i, ReadValuesSDM::getVoltage(i) * 100);
    setCurrent(i, ReadValuesSDM::getCurrent(i) * 1000);
    setPowerActive(i, ReadValuesSDM::getPowerActive(i) * 100000);

    setFreq(ReadValuesSDM::getFreq() * 100);
    setPowerReactive(i, ReadValuesSDM::getPowerReactive(i) * 10000);
    setPowerApparent(i, ReadValuesSDM::getPowerApparent(i) * 100000);
    setPowerFactor(i, ReadValuesSDM::getPowerFactor(i) * 1000);
    setPhaseAngle(i, ReadValuesSDM::getPhaseAngle(i) * 10);
    delay(0);
  }
}

};  // namespace Sensor
};  // namespace Supla
