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

#include "SDM_ReadValues.h"

namespace Supla {
namespace Sensor {

ReadValuesSDM::ReadValuesSDM(int8_t pinRX, int8_t pinTX, long baud) : sdm(swSerSDM, baud, NOT_A_PIN, SWSERIAL_8N1, pinRX, pinTX) {
  sdm.begin();
}

// energy 1 == 0.00001 kWh
unsigned _supla_int64_t ReadValuesSDM::getFwdActEnergy(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_L1_EXPORT_ACTIVE_ENERGY;
      break;
    case 1:
      reg = SDM_L2_EXPORT_ACTIVE_ENERGY;
      break;
    case 3:
      reg = SDM_L3_EXPORT_ACTIVE_ENERGY;
      break;
  }

  return sdmRead(reg);
}

// energy 1 == 0.00001 kWh
unsigned _supla_int64_t ReadValuesSDM::getRvrActEnergy(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_L1_IMPORT_ACTIVE_ENERGY;
      break;
    case 1:
      reg = SDM_L2_IMPORT_ACTIVE_ENERGY;
      break;
    case 3:
      reg = SDM_L3_IMPORT_ACTIVE_ENERGY;
      break;
  }

  return sdmRead(reg);
}

// energy 1 == 0.00001 kWh
unsigned _supla_int64_t ReadValuesSDM::getFwdReactEnergy(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_L1_EXPORT_REACTIVE_ENERGY;
      break;
    case 1:
      reg = SDM_L2_EXPORT_REACTIVE_ENERGY;
      break;
    case 3:
      reg = SDM_L3_EXPORT_REACTIVE_ENERGY;
      break;
  }

  return sdmRead(reg);
}

// energy 1 == 0.00001 kWh
unsigned _supla_int64_t ReadValuesSDM::getRvrReactEnergy(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_L1_IMPORT_REACTIVE_ENERGY;
      break;
    case 1:
      reg = SDM_L2_IMPORT_REACTIVE_ENERGY;
      break;
    case 3:
      reg = SDM_L3_IMPORT_REACTIVE_ENERGY;
      break;
  }

  return sdmRead(reg);
}

// voltage 1 == 0.01 V
unsigned _supla_int16_t ReadValuesSDM::getVoltage(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_PHASE_1_VOLTAGE;
      break;
    case 1:
      reg = SDM_PHASE_2_VOLTAGE;
      break;
    case 3:
      reg = SDM_PHASE_3_VOLTAGE;
      break;
  }

  return sdmRead(reg);
}

// current 1 == 0.001 A
unsigned _supla_int_t ReadValuesSDM::getCurrent(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_PHASE_1_CURRENT;
      break;
    case 1:
      reg = SDM_PHASE_2_CURRENT;
      break;
    case 3:
      reg = SDM_PHASE_3_CURRENT;
      break;
  }

  return sdmRead(reg);
}

// Frequency 1 == 0.01 Hz
unsigned _supla_int16_t ReadValuesSDM::getFreq() {
  return sdmRead(SDM_FREQUENCY);
}

// power 1 == 0.00001 W
_supla_int_t ReadValuesSDM::getPowerActive(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_PHASE_1_POWER;
      break;
    case 1:
      reg = SDM_PHASE_2_POWER;
      break;
    case 3:
      reg = SDM_PHASE_3_POWER;
      break;
  }

  return sdmRead(reg);
}

// power 1 == 0.00001 var
_supla_int_t ReadValuesSDM::getPowerReactive(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_PHASE_1_REACTIVE_POWER;
      break;
    case 1:
      reg = SDM_PHASE_2_REACTIVE_POWER;
      break;
    case 3:
      reg = SDM_PHASE_3_REACTIVE_POWER;
      break;
  }

  return sdmRead(reg);
}

// power 1 == 0.00001 VA
_supla_int_t ReadValuesSDM::getPowerApparent(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_PHASE_1_APPARENT_POWER;
      break;
    case 1:
      reg = SDM_PHASE_2_APPARENT_POWER;
      break;
    case 3:
      reg = SDM_PHASE_3_APPARENT_POWER;
      break;
  }

  return sdmRead(reg);
}

// power 1 == 0.001
_supla_int_t ReadValuesSDM::getPowerFactor(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_PHASE_1_POWER_FACTOR;
      break;
    case 1:
      reg = SDM_PHASE_2_POWER_FACTOR;
      break;
    case 3:
      reg = SDM_PHASE_3_POWER_FACTOR;
      break;
  }

  return sdmRead(reg);
}

// phase angle 1 == 0.1 degree
_supla_int_t ReadValuesSDM::getPhaseAngle(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_PHASE_1_ANGLE;
      break;
    case 1:
      reg = SDM_PHASE_2_ANGLE;
      break;
    case 3:
      reg = SDM_PHASE_3_ANGLE;
      break;
  }

  return sdmRead(reg);
}

float ReadValuesSDM::sdmRead(uint16_t reg) {
  float tmpval = sdm.readVal(reg);

  if (isnan(tmpval))
    tmpval = 0.00;

  return tmpval;
}

};  // namespace Sensor
};  // namespace Supla