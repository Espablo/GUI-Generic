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

#if defined(ESP8266)
ReadValuesSDM::ReadValuesSDM(int8_t pinRX, int8_t pinTX, long baud)
    : swSerSDM(pinRX, pinTX), sdm(swSerSDM, baud, NOT_A_PIN, SWSERIAL_8N1, pinRX, pinTX) {
  sdm.begin();
}
#else
ReadValuesSDM::ReadValuesSDM(HardwareSerial& serial, int8_t pinRX, int8_t pinTX, long baud) : sdm(serial, baud, NOT_A_PIN, SERIAL_8N1, pinRX, pinTX) {
  sdm.begin();
}
#endif

// energy 1 == 0.00001 kWh - one phase
float ReadValuesSDM::getFwdActEnergy() {
  return sdmRead(SDM_IMPORT_ACTIVE_ENERGY);
}

// energy 1 == 0.00001 kWh - one phase
float ReadValuesSDM::getRvrActEnergy() {
  return sdmRead(SDM_EXPORT_ACTIVE_ENERGY);
}

// energy 1 == 0.00001 kWh - one phase
float ReadValuesSDM::getFwdReactEnergy() {
  return sdmRead(SDM_IMPORT_REACTIVE_ENERGY);
}

// energy 1 == 0.00001 kWh - one phase
float ReadValuesSDM::getRvrReactEnergy() {
  return sdmRead(SDM_EXPORT_REACTIVE_ENERGY);
}

// energy 1 == 0.00001 kWh
float ReadValuesSDM::getFwdActEnergy(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_L1_IMPORT_ACTIVE_ENERGY;
      break;
    case 1:
      reg = SDM_L2_IMPORT_ACTIVE_ENERGY;
      break;
    case 2:
      reg = SDM_L3_IMPORT_ACTIVE_ENERGY;
      break;
  }

  return sdmRead(reg);
}

// energy 1 == 0.00001 kWh
float ReadValuesSDM::getFwdActEnergyTotal() {
  return sdmRead(SDM_TOTAL_ACTIVE_ENERGY);
}

// energy 1 == 0.00001 kWh
float ReadValuesSDM::getRvrActEnergy(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_L1_EXPORT_ACTIVE_ENERGY;
      break;
    case 1:
      reg = SDM_L2_EXPORT_ACTIVE_ENERGY;
      break;
    case 2:
      reg = SDM_L3_EXPORT_ACTIVE_ENERGY;
      break;
  }

  return sdmRead(reg);
}

// energy 1 == 0.00001 kWh
float ReadValuesSDM::getFwdReactEnergy(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_L1_IMPORT_REACTIVE_ENERGY;
      break;
    case 1:
      reg = SDM_L2_IMPORT_REACTIVE_ENERGY;
      break;
    case 2:
      reg = SDM_L3_IMPORT_REACTIVE_ENERGY;
      break;
  }

  return sdmRead(reg);
}

// energy 1 == 0.00001 kWh
float ReadValuesSDM::getFwdReactEnergyTotal() {
  return sdmRead(SDM_TOTAL_REACTIVE_ENERGY);
}

// energy 1 == 0.00001 kWh
float ReadValuesSDM::getRvrReactEnergy(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_L1_EXPORT_REACTIVE_ENERGY;
      break;
    case 1:
      reg = SDM_L2_EXPORT_REACTIVE_ENERGY;
      break;
    case 2:
      reg = SDM_L3_EXPORT_REACTIVE_ENERGY;
      break;
  }

  return sdmRead(reg);
}

// voltage 1 == 0.01 V
float ReadValuesSDM::getVoltage(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_PHASE_1_VOLTAGE;
      break;
    case 1:
      reg = SDM_PHASE_2_VOLTAGE;
      break;
    case 2:
      reg = SDM_PHASE_3_VOLTAGE;
      break;
  }

  return sdmRead(reg);
}

// current 1 == 0.001 A
float ReadValuesSDM::getCurrent(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_PHASE_1_CURRENT;
      break;
    case 1:
      reg = SDM_PHASE_2_CURRENT;
      break;
    case 2:
      reg = SDM_PHASE_3_CURRENT;
      break;
  }

  return sdmRead(reg);
}

// Frequency 1 == 0.01 Hz
float ReadValuesSDM::getFreq() {
  return sdmRead(SDM_FREQUENCY);
}

// power 1 == 0.00001 W
float ReadValuesSDM::getPowerActive(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_PHASE_1_POWER;
      break;
    case 1:
      reg = SDM_PHASE_2_POWER;
      break;
    case 2:
      reg = SDM_PHASE_3_POWER;
      break;
  }

  return sdmRead(reg);
}

// power 1 == 0.00001 var
float ReadValuesSDM::getPowerReactive(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_PHASE_1_REACTIVE_POWER;
      break;
    case 1:
      reg = SDM_PHASE_2_REACTIVE_POWER;
      break;
    case 2:
      reg = SDM_PHASE_3_REACTIVE_POWER;
      break;
  }

  return sdmRead(reg);
}

// power 1 == 0.00001 VA
float ReadValuesSDM::getPowerApparent(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_PHASE_1_APPARENT_POWER;
      break;
    case 1:
      reg = SDM_PHASE_2_APPARENT_POWER;
      break;
    case 2:
      reg = SDM_PHASE_3_APPARENT_POWER;
      break;
  }

  return sdmRead(reg);
}

// power 1 == 0.001
float ReadValuesSDM::getPowerFactor(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_PHASE_1_POWER_FACTOR;
      break;
    case 1:
      reg = SDM_PHASE_2_POWER_FACTOR;
      break;
    case 2:
      reg = SDM_PHASE_3_POWER_FACTOR;
      break;
  }

  return sdmRead(reg);
}

// phase angle 1 == 0.1 degree
float ReadValuesSDM::getPhaseAngle(int phase) {
  uint16_t reg = SDM_ERR_NO_ERROR;

  switch (phase) {
    case 0:
      reg = SDM_PHASE_1_ANGLE;
      break;
    case 1:
      reg = SDM_PHASE_2_ANGLE;
      break;
    case 2:
      reg = SDM_PHASE_3_ANGLE;
      break;
  }

  return sdmRead(reg);
}

float ReadValuesSDM::sdmRead(uint16_t reg) {
  float tmpval = NAN;

  tmpval = sdm.readVal(reg);

  if (isnan(tmpval))
    tmpval = 0.0f;

  return tmpval;
  // uint8_t retry_count = 3;
  // bool success = false;
  // float tmpval = NAN;

  // while (retry_count > 0 && !success) {
  //   sdm.clearErrCode();
  //   tmpval = sdm.readVal(reg);
  //   --retry_count;
  //   if (sdm.getErrCode() == SDM_ERR_NO_ERROR) {
  //     success = true;
  //   }
  // }

  // if (isnan(tmpval))
  //   tmpval = 0.00;

  // return tmpval;
}

uint16_t ReadValuesSDM::getErrCode(bool _clear) {
  return sdm.getErrCode(_clear);
}

uint32_t ReadValuesSDM::getErrCount(bool _clear) {
  return sdm.getErrCount(_clear);
}

uint32_t ReadValuesSDM::getSuccCount(bool _clear) {
  return sdm.getSuccCount(_clear);
}

void ReadValuesSDM::clearErrCode() {
  sdm.clearErrCode();
}

void ReadValuesSDM::clearErrCount() {
  sdm.clearErrCount();
}

void ReadValuesSDM::clearSuccCount() {
  sdm.clearSuccCount();
}

};  // namespace Sensor
};  // namespace Supla