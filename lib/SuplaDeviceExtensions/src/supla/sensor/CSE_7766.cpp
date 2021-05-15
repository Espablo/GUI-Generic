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

#include "CSE_7766.h"

namespace Supla {
namespace Sensor {

CSE_7766::CSE_7766(int8_t pinRX) : pinRX(pinRX) {
  sensor = new CSE7766();
}

void CSE_7766::onInit() {
  sensor->setRX(pinRX);
  sensor->begin();

  sensor->setCurrentRatio(currentMultiplier);
  sensor->setVoltageRatio(voltageMultiplier);
  sensor->setPowerRatio(powerMultiplier);

  readValuesFromDevice();
  updateChannelValues();
}

void CSE_7766::readValuesFromDevice() {
  bool currentChanelRelay = false;
  sensor->handle();

  for (auto element = Supla::Element::begin(); element != nullptr;
       element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_RELAY) {
        currentChanelRelay = true;
        if (channel->getValueBool()) {
          energy = _energy + (sensor->getEnergy() /
                              36);  // current energy value = value at start
        }
      }
    }
  }

  if (!currentChanelRelay) {
    energy = _energy + (sensor->getEnergy() /
                        36);  // current energy value = value at start
  }

  // voltage in 0.01 V
  setVoltage(0, sensor->getVoltage() * 100);
  // current in 0.001 A
  setCurrent(0, sensor->getCurrent() * 1000);
  // power in 0.00001 kW
  setPowerActive(0, sensor->getActivePower() * 100000);
  // energy in 0.00001 kWh
  setFwdActEnergy(0, energy);
  // power in 0.00001 kVA
  setPowerApparent(0, sensor->getApparentPower() * 100000);
  // power in 0.00001 kvar
  setPowerReactive(0, sensor->getReactivePower() * 100000);
  // power in 0.001
  setPowerFactor(0, sensor->getPowerFactor() * 1000);
}

void CSE_7766::onSaveState() {
  Supla::Storage::WriteState((unsigned char *)&energy, sizeof(energy));
  Supla::Storage::WriteState((unsigned char *)&currentMultiplier,
                             sizeof(currentMultiplier));
  Supla::Storage::WriteState((unsigned char *)&voltageMultiplier,
                             sizeof(voltageMultiplier));
  Supla::Storage::WriteState((unsigned char *)&powerMultiplier,
                             sizeof(powerMultiplier));
}

void CSE_7766::onLoadState() {
  if (Supla::Storage::ReadState((unsigned char *)&energy, sizeof(energy))) {
    setCounter(energy);
  }

  Supla::Storage::ReadState((unsigned char *)&currentMultiplier,
                            sizeof(currentMultiplier));

  Supla::Storage::ReadState((unsigned char *)&voltageMultiplier,
                            sizeof(voltageMultiplier));

  Supla::Storage::ReadState((unsigned char *)&powerMultiplier,
                            sizeof(powerMultiplier));
}

double CSE_7766::getCurrentMultiplier() {
  return currentMultiplier;
}

double CSE_7766::getVoltageMultiplier() {
  return voltageMultiplier;
}

double CSE_7766::getPowerMultiplier() {
  return powerMultiplier;
}

_supla_int64_t CSE_7766::getCounter() {
  return energy;
}

void CSE_7766::setCurrentMultiplier(double value) {
  currentMultiplier = value;
  sensor->setCurrentRatio(value);
}

void CSE_7766::setVoltageMultiplier(double value) {
  voltageMultiplier = value;
  sensor->setVoltageRatio(value);
}

void CSE_7766::setPowerMultiplier(double value) {
  powerMultiplier = value;
  sensor->setPowerRatio(value);
}

void CSE_7766::setCounter(_supla_int64_t value) {
  _energy = value;  // ------- energy value read from memory at startup
  energy = value;
  setFwdActEnergy(0, value);
}

void CSE_7766::calibrate(double calibPower, double calibVoltage) {
  sensor->handle();
  unsigned long timeout1 = millis();
  while ((millis() - timeout1) < 10000) {
    delay(10);
  }

  Serial.print(F("Active Power (W)    : "));
  Serial.println(sensor->getActivePower());
  Serial.print(F("Voltage (V)         : "));
  Serial.println(sensor->getVoltage());
  Serial.print(F("Current (A)         : "));
  Serial.println(sensor->getCurrent());

  sensor->handle();
  sensor->expectedPower(calibPower);
  sensor->expectedVoltage(calibVoltage);
  sensor->expectedCurrent(calibPower / calibVoltage);

  unsigned long timeout2 = millis();
  while ((millis() - timeout2) < 2000) {
    delay(10);
  }

  currentMultiplier = sensor->getCurrentRatio();
  voltageMultiplier = sensor->getVoltageRatio();
  powerMultiplier = sensor->getPowerRatio();

  Serial.print(F("New current multiplier : "));
  Serial.println(currentMultiplier);
  Serial.print(F("New voltage multiplier : "));
  Serial.println(voltageMultiplier);
  Serial.print(F("New power multiplier   : "));
  Serial.println(powerMultiplier);
  Supla::Storage::ScheduleSave(2000);
  delay(0);
}

CSE7766 *CSE_7766::sensor = nullptr;
};  // namespace Sensor
};  // namespace Supla
