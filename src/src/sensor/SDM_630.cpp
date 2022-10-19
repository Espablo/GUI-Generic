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

SDM630::SDM630(HardwareSerial& serial) : sdm(serial, 9600){};

void SDM630::onInit() {
  readValuesFromDevice();
  updateChannelValues();
}

void SDM630::readValuesFromDevice() {
  for (int i = 0; i < MAX_PHASES; i++) {
    setFwdActEnergy(i, sdm.getFwdActEnergy(i) * 100000);
    setRvrActEnergy(i, sdm.getRvrActEnergy(i) * 100000);
    setFwdReactEnergy(i, sdm.getFwdReactEnergy(i) * 100000);
    setRvrReactEnergy(i, sdm.getRvrReactEnergy(i) * 100000);

    setVoltage(i, sdm.getVoltage(i) * 100);
    setCurrent(i, sdm.getCurrent(i) * 1000);
    setFreq(sdm.getFreq() * 100);
    setPowerActive(i, sdm.getPowerActive(i) * 100000);
    setPowerReactive(i, sdm.getPowerReactive(i) * 10000);
    setPowerApparent(i, sdm.getPowerApparent(i) * 100000);
    setPowerFactor(i, sdm.getPowerFactor(i) * 1000);
    setPhaseAngle(i, sdm.getPhaseAngle(i) * 10);
  }
}

};  // namespace Sensor
};  // namespace Supla
