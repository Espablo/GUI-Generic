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
                   bool useInterrupts)
    : pinCF(pinCF),
      pinCF1(pinCF1),
      pinSEL(pinSEL),
      useInterrupts(useInterrupts),
      currentMultiplier(18388),
      voltageMultiplier(247704),
      powerMultiplier(2586583),
      currentWhen(LOW) {
  sensor = new HLW8012();
}

void HLW_8012::onInit() {
  extChannel.setFlag(SUPLA_CHANNEL_FLAG_CALCFG_RESET_COUNTERS);

  sensor->begin(pinCF, pinCF1, pinSEL, currentWhen, useInterrupts);
  sensor->setCurrentMultiplier(currentMultiplier);
  sensor->setVoltageMultiplier(voltageMultiplier);
  sensor->setPowerMultiplier(powerMultiplier);

  attachInterrupt(pinCF, hjl01_cf_interrupt, FALLING);
  attachInterrupt(pinCF1, hjl01_cf1_interrupt, FALLING);

  readValuesFromDevice();
  updateChannelValues();
}

void HLW_8012::readValuesFromDevice() {
  bool currentChanelRelay = false;

  for (auto element = Supla::Element::begin(); element != nullptr;
       element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_RELAY) {
        if (channel->getValueBool()) currentChanelRelay = true;
      }
    }
  }

  if (currentChanelRelay) {
    energy = _energy + (sensor->getEnergy() /
                        36);  // current energy value = value at start
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
  Supla::Storage::WriteState((unsigned char *)&energy, sizeof(energy));
  Supla::Storage::WriteState((unsigned char *)&currentMultiplier,
                             sizeof(currentMultiplier));
  Supla::Storage::WriteState((unsigned char *)&voltageMultiplier,
                             sizeof(voltageMultiplier));
  Supla::Storage::WriteState((unsigned char *)&powerMultiplier,
                             sizeof(powerMultiplier));
  Supla::Storage::WriteState((unsigned char *)&currentWhen,
                             sizeof(currentWhen));
}

void HLW_8012::onLoadState() {
  if (Supla::Storage::ReadState((unsigned char *)&energy, sizeof(energy))) {
    setCounter(energy);
  }
  Supla::Storage::ReadState((unsigned char *)&currentMultiplier,
                            sizeof(currentMultiplier));
  Supla::Storage::ReadState((unsigned char *)&voltageMultiplier,
                            sizeof(voltageMultiplier));
  Supla::Storage::ReadState((unsigned char *)&powerMultiplier,
                            sizeof(powerMultiplier));
  Supla::Storage::ReadState((unsigned char *)&currentWhen, sizeof(currentWhen));
}

double HLW_8012::getCurrentMultiplier() {
  return currentMultiplier;
}

double HLW_8012::getVoltageMultiplier() {
  return voltageMultiplier;
}

double HLW_8012::getPowerMultiplier() {
  return powerMultiplier;
}

bool HLW_8012::getMode() {
  return currentWhen;
}

_supla_int64_t HLW_8012::getCounter() {
  return energy;
}

void HLW_8012::setCurrentMultiplier(double value) {
  currentMultiplier = value;
  sensor->setCurrentMultiplier(value);
}

void HLW_8012::setVoltageMultiplier(double value) {
  voltageMultiplier = value;
  sensor->setVoltageMultiplier(value);
}

void HLW_8012::setPowerMultiplier(double value) {
  powerMultiplier = value;
  sensor->setPowerMultiplier(value);
}

void HLW_8012::setMode(bool value) {
  currentWhen = value;
  sensor->setMode((hlw8012_mode_t)value);
}

void HLW_8012::setCounter(_supla_int64_t newEnergy) {
  _energy = newEnergy;  // ------- energy value read from memory at startup
  energy = newEnergy;
  setFwdActEnergy(0, newEnergy);
  Supla::Storage::ScheduleSave(1000);
}

int HLW_8012::handleCalcfgFromServer(TSD_DeviceCalCfgRequest *request) {
  if (request && request->Command == SUPLA_CALCFG_CMD_RESET_COUNTERS) {
    setCounter(0);
    return SUPLA_CALCFG_RESULT_DONE;
  }
  return SUPLA_CALCFG_RESULT_NOT_SUPPORTED;
}

// When using interrupts we have to call the library entry point
// whenever an interrupt is triggered
void IRAM_ATTR HLW_8012::hjl01_cf1_interrupt() {
  sensor->cf1_interrupt();
}

void IRAM_ATTR HLW_8012::hjl01_cf_interrupt() {
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

  currentMultiplier = sensor->getCurrentMultiplier();
  voltageMultiplier = sensor->getVoltageMultiplier();
  powerMultiplier = sensor->getPowerMultiplier();

  Serial.print(F("[HLW] New current multiplier : "));
  Serial.println(currentMultiplier);
  Serial.print(F("[HLW] New voltage multiplier : "));
  Serial.println(voltageMultiplier);
  Serial.print(F("[HLW] New power multiplier   : "));
  Serial.println(powerMultiplier);
  Supla::Storage::ScheduleSave(2000);
  delay(0);
}

HLW8012 *HLW_8012::sensor = nullptr;
};  // namespace Sensor
};  // namespace Supla
