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

#include "HLW_8012.h"

namespace Supla {
namespace Sensor {

HLW_8012::HLW_8012(int8_t pinCF,
                   int8_t pinCF1,
                   int8_t pinSEL,
                   bool currentWhen,
                   bool use_interrupts)
    : pinCF(pinCF),
      pinCF1(pinCF1),
      pinSEL(pinSEL),
      currentWhen(currentWhen),
      use_interrupts(use_interrupts) {
  sensor = new HLW8012();

  setCurrentMultiplier(18388);
  setVoltageMultiplier(247704);
  setPowerMultiplier(2586583);

  sensor->begin(pinCF, pinCF1, pinSEL, currentWhen, use_interrupts);

  attachInterrupt(pinCF, hjl01_cf_interrupt, FALLING);
  attachInterrupt(pinCF1, hjl01_cf1_interrupt, FALLING);
}

void HLW_8012::onInit() {
  readValuesFromDevice();
  updateChannelValues();
}

void HLW_8012::readValuesFromDevice() {
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

  unsigned int _reactive = 0;
  double _pf = 0;
  double _current = sensor->getCurrent();
  unsigned int _voltage = sensor->getVoltage();
  unsigned int _active = sensor->getActivePower();
  unsigned int _apparent = _voltage * _current;
  if (_apparent > _active) {
    _reactive = sqrt(_apparent * _apparent - _active * _active);
  } else {
    _reactive = 0;
  }
  if (_active > _apparent) {
    _pf = 1;
  }
  if (_apparent == 0) {
    _pf = 0;
  } else {
    _pf = (double)_active / _apparent;
  }
  setVoltage(0, _voltage * 100);            // voltage in 0.01 V
  setCurrent(0, _current * 1000);           // current in 0.001 A
  setPowerActive(0, _active * 100000);      // power in 0.00001 kW
  setFwdActEnergy(0, energy);               // energy in 0.00001 kWh
  setPowerApparent(0, _apparent * 100000);  // power in 0.00001 kVA
  setPowerReactive(0, _reactive * 100000);  // power in 0.00001 kvar
  setPowerFactor(0, _pf * 1000);            // power in 0.001
}

void HLW_8012::onSaveState() {
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

void HLW_8012::onLoadState() {
  double current_multiplier;
  double voltage_multiplier;
  double power_multiplier;

  if (Supla::Storage::ReadState((unsigned char *)&energy, sizeof(energy))) {
    setCounter(energy);
  }

  if (Supla::Storage::ReadState((unsigned char *)&current_multiplier,
                                sizeof(current_multiplier))) {
    setCurrentMultiplier(current_multiplier);
  } else {
    setCurrentMultiplier(18388);
  }

  if (Supla::Storage::ReadState((unsigned char *)&voltage_multiplier,
                                sizeof(voltage_multiplier))) {
    setVoltageMultiplier(voltage_multiplier);
  } else {
    setVoltageMultiplier(247704);
  }

  if (Supla::Storage::ReadState((unsigned char *)&power_multiplier,
                                sizeof(power_multiplier))) {
    setPowerMultiplier(power_multiplier);
  } else {
    setPowerMultiplier(2586583);
  }
}

double HLW_8012::getCurrentMultiplier() {
  return sensor->getCurrentMultiplier();
};

double HLW_8012::getVoltageMultiplier() {
  return sensor->getVoltageMultiplier();
};

double HLW_8012::getPowerMultiplier() {
  return sensor->getPowerMultiplier();
};

_supla_int64_t HLW_8012::getCounter() {
  return energy;
}

void HLW_8012::setCurrentMultiplier(double current_multiplier) {
  sensor->setCurrentMultiplier(current_multiplier);
};
void HLW_8012::setVoltageMultiplier(double voltage_multiplier) {
  sensor->setVoltageMultiplier(voltage_multiplier);
};
void HLW_8012::setPowerMultiplier(double power_multiplier) {
  sensor->setPowerMultiplier(power_multiplier);
};

void HLW_8012::setCounter(_supla_int64_t new_energy) {
  _energy = new_energy;  // ------- energy value read from memory at startup
  energy = new_energy;
  setFwdActEnergy(0, new_energy);
}

// When using interrupts we have to call the library entry point
// whenever an interrupt is triggered
void ICACHE_RAM_ATTR HLW_8012::hjl01_cf1_interrupt() {
  sensor->cf1_interrupt();
}

void ICACHE_RAM_ATTR HLW_8012::hjl01_cf_interrupt() {
  sensor->cf_interrupt();
}

void HLW_8012::calibrate(double calibPower, double calibVoltage) {
  unsigned long timeout1 = millis();
  while ((millis() - timeout1) < 10000) {
    delay(10);
  }

  Serial.print(F("[HLW] Active Power (W)    : "));
  Serial.println(sensor->getActivePower());
  Serial.print(F("[HLW] Voltage (V)         : "));
  Serial.println(sensor->getVoltage());
  Serial.print(F("[HLW] Current (A)         : "));
  Serial.println(sensor->getCurrent());

  sensor->expectedActivePower(calibPower);
  sensor->expectedVoltage(calibVoltage);
  sensor->expectedCurrent(calibPower / calibVoltage);

  unsigned long timeout2 = millis();
  while ((millis() - timeout2) < 2000) {
    delay(10);
  }

  double current_multi = getCurrentMultiplier();
  double voltage_multi = getVoltageMultiplier();
  double power_multi = getPowerMultiplier();

  Serial.print(F("[HLW] New current multiplier : "));
  Serial.println(current_multi);
  Serial.print(F("[HLW] New voltage multiplier : "));
  Serial.println(voltage_multi);
  Serial.print(F("[HLW] New power multiplier   : "));
  Serial.println(power_multi);
  Supla::Storage::ScheduleSave(2000);
  yield();
}

HLW8012 *HLW_8012::sensor = nullptr;
};  // namespace Sensor
};  // namespace Supla
