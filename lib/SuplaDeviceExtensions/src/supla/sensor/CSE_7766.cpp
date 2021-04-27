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

  sensor->setRX(pinRX);
  sensor->begin();
}

void CSE_7766::onInit() {
  readValuesFromDevice();
  updateChannelValues();
}

void CSE_7766::readValuesFromDevice() {
  sensor->handle();

  for (auto element = Supla::Element::begin(); element != nullptr;
       element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();
      if (channel->getChannelType() == SUPLA_CHANNELTYPE_RELAY) {
        if (channel->getValueBool()) {
          energy = _energy + (sensor->getEnergy() /
                              36);  // current energy value = value at start
        }
      }
    }
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
  double current_multiplier = getCurrentMultiplier();
  double voltage_multiplier = getVoltageMultiplier();
  double power_multiplier = getPowerMultiplier();

  Supla::Storage::WriteState((unsigned char *)&energy, sizeof(energy));
  Supla::Storage::WriteState((unsigned char *)&current_multiplier,
                             sizeof(current_multiplier));
  Supla::Storage::WriteState((unsigned char *)&voltage_multiplier,
                             sizeof(voltage_multiplier));
  Supla::Storage::WriteState((unsigned char *)&power_multiplier,
                             sizeof(power_multiplier));
}

void CSE_7766::onLoadState() {
  double current_multiplier;
  double voltage_multiplier;
  double power_multiplier;

  if (Supla::Storage::ReadState((unsigned char *)&energy, sizeof(energy))) {
    setCounter(energy);
  }

  if (Supla::Storage::ReadState((unsigned char *)&current_multiplier,
                                sizeof(current_multiplier))) {
    setCurrentMultiplier(current_multiplier);
  }

  if (Supla::Storage::ReadState((unsigned char *)&voltage_multiplier,
                                sizeof(voltage_multiplier))) {
    setVoltageMultiplier(voltage_multiplier);
  }

  if (Supla::Storage::ReadState((unsigned char *)&power_multiplier,
                                sizeof(power_multiplier))) {
    setPowerMultiplier(power_multiplier);
  }
}

double CSE_7766::getCurrentMultiplier() {
  return sensor->getCurrentRatio();
}

double CSE_7766::getVoltageMultiplier() {
  return sensor->getVoltageRatio();
}

double CSE_7766::getPowerMultiplier() {
  return sensor->getPowerRatio();
}

_supla_int64_t CSE_7766::getCounter() {
  return energy;
}

void CSE_7766::setCurrentMultiplier(double current_multiplier) {
  sensor->setCurrentRatio(current_multiplier);
}

void CSE_7766::setVoltageMultiplier(double voltage_multiplier) {
  sensor->setVoltageRatio(voltage_multiplier);
}

void CSE_7766::setPowerMultiplier(double power_multiplier) {
  sensor->setPowerRatio(power_multiplier);
}

void CSE_7766::setCounter(_supla_int64_t new_energy) {
  _energy = new_energy;  // ------- energy value read from memory at startup
  energy = new_energy;
  setFwdActEnergy(0, new_energy);
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

  double current_multi = getCurrentMultiplier();
  double voltage_multi = getVoltageMultiplier();
  double power_multi = getPowerMultiplier();

  Serial.print(F("New current multiplier : "));
  Serial.println(current_multi);
  Serial.print(F("New voltage multiplier : "));
  Serial.println(voltage_multi);
  Serial.print(F("New power multiplier   : "));
  Serial.println(power_multi);
  Supla::Storage::ScheduleSave(2000);
  delay(0);
}

CSE7766 *CSE_7766::sensor = nullptr;
};  // namespace Sensor
};  // namespace Supla
