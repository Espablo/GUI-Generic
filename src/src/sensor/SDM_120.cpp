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

  float energy = ReadValuesSDM::getFwdActEnergy();
  // float energyTotal = ReadValuesSDM::getFwdActEnergyTotal();

  float reactEnergy = ReadValuesSDM::getFwdReactEnergy();
  // float reactEnergyTotal = ReadValuesSDM::getFwdReactEnergyTotal();

  float rvrActEnergy = ReadValuesSDM::getRvrActEnergy();
  float rvrReactEnergy = ReadValuesSDM::getRvrReactEnergy();

  float voltage = ReadValuesSDM::getVoltage();
  float current = ReadValuesSDM::getCurrent();
  float powerActive = ReadValuesSDM::getPowerActive();
  float powerFactor = ReadValuesSDM::getPowerFactor();
  float freq = ReadValuesSDM::getFreq();
  float powerReactive = ReadValuesSDM::getPowerReactive();
  float powerApparent = ReadValuesSDM::getPowerApparent();
  float phaseAngle = ReadValuesSDM::getPhaseAngle();

  if (!isnan(energy))
    setFwdActEnergy(0, energy * 100000);
  // else if (!isnan(energyTotal))
  //   setFwdActEnergy(0, energyTotal * 100000);

  if (!isnan(reactEnergy))
    setFwdReactEnergy(0, reactEnergy * 100000);
  // else if (!isnan(reactEnergyTotal))
  //   setFwdReactEnergy(0, reactEnergyTotal * 100000);

  if (!isnan(rvrActEnergy))
    setRvrActEnergy(0, rvrActEnergy * 100000);
  if (!isnan(rvrReactEnergy))
    setRvrReactEnergy(0, rvrReactEnergy * 100000);

  if (!isnan(voltage))
    setVoltage(0, voltage * 100);
  if (!isnan(current))
    setCurrent(0, current * 1000);
  if (!isnan(powerActive))
    setPowerActive(0, powerActive * 100000);
  if (!isnan(powerFactor))
    setPowerFactor(0, powerFactor * 1000);
  if (!isnan(freq))
    setFreq(freq * 100);
  if (!isnan(powerReactive))
    setPowerReactive(0, powerReactive * 10000);
  if (!isnan(powerApparent))
    setPowerApparent(0, powerApparent * 100000);
  if (!isnan(phaseAngle))
    setPhaseAngle(0, phaseAngle * 10);

  delay(0);
}

};  // namespace Sensor
};  // namespace Supla
