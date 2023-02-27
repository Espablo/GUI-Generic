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
SDM630::SDM630(int8_t pinRX, int8_t pinTX, long baud) : ReadValuesSDM(pinRX, pinTX, baud){};
#else
SDM630::SDM630(HardwareSerial& serial, int8_t pinRX, int8_t pinTX, long baud) : ReadValuesSDM(serial, pinRX, pinTX, baud){};
#endif

void SDM630::onInit() {
  readValuesFromDevice();
  updateChannelValues();
}

void SDM630::readValuesFromDevice() {
  clearSuccCount();
  clearErrCount();
  clearErrCode();
  
  // float energyTotal = ReadValuesSDM::getFwdActEnergyTotal();
  // float reactEnergyTotal = ReadValuesSDM::getFwdReactEnergyTotal();
  float freq = ReadValuesSDM::getFreq();

  for (int i = 0; i < MAX_PHASES; i++) {
    float energy = ReadValuesSDM::getFwdActEnergy(i);
    float reactEnergy = ReadValuesSDM::getFwdReactEnergy(i);

    float rvrActEnergy = ReadValuesSDM::getRvrActEnergy(i);
    float rvrReactEnergy = ReadValuesSDM::getRvrReactEnergy(i);
    float voltage = ReadValuesSDM::getVoltage(i);
    float current = ReadValuesSDM::getCurrent(i);
    float powerActive = ReadValuesSDM::getPowerActive(i);
    float powerFactor = ReadValuesSDM::getPowerFactor(i);
    float powerReactive = ReadValuesSDM::getPowerReactive(i);
    float powerApparent = ReadValuesSDM::getPowerApparent(i);
    float phaseAngle = ReadValuesSDM::getPhaseAngle(i);

    if (!isnan(energy))
      setFwdActEnergy(i, energy * 100000);
    // else if (!isnan(energyTotal))
    //   setFwdActEnergy(0, energyTotal * 100000);

    if (!isnan(reactEnergy))
      setFwdReactEnergy(i, reactEnergy * 100000);
    // else if (!isnan(reactEnergyTotal))
    //   setFwdReactEnergy(0, reactEnergyTotal * 100000);

    if (!isnan(rvrActEnergy))
      setRvrActEnergy(i, rvrActEnergy * 100000);
    if (!isnan(rvrReactEnergy))
      setRvrReactEnergy(i, rvrReactEnergy * 100000);

    if (!isnan(voltage))
      setVoltage(i, voltage * 100);
    if (!isnan(current))
      setCurrent(i, current * 1000);
    if (!isnan(powerActive))
      setPowerActive(i, powerActive * 100000);
    if (!isnan(powerFactor))
      setPowerFactor(i, powerFactor * 1000);
    if (!isnan(freq))
      setFreq(freq * 100);
    if (!isnan(powerReactive))
      setPowerReactive(i, powerReactive * 10000);
    if (!isnan(powerApparent))
      setPowerApparent(i, powerApparent * 100000);
    if (!isnan(phaseAngle))
      setPhaseAngle(i, phaseAngle * 10);

    delay(0);
  }
}

};  // namespace Sensor
};  // namespace Supla
