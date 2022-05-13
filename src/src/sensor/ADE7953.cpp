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

#include "ADE7953.h"

namespace Supla {
namespace Sensor {

ADE7953::ADE7953(uint8_t pin_irq, uint8_t model) {
  Ade7953.model = model;
  pinMode(pin_irq, INPUT);  // Related to resetPins() - Must be set to input

  if (1 == Ade7953.model) {  // Shelly EM
    pinMode(16, OUTPUT);     // Reset pin ADE7953
    digitalWrite(16, 0);
    delay(1);
    digitalWrite(16, 1);
    pinMode(16, INPUT);
  }

  delay(100);  // Need 100mS to init ADE7953
}

void ADE7953::onInit() {
  Ade7953Write(0x102, 0x0004);  // Locking the communication interface (Clear bit COMM_LOCK), Enable HPF
  Ade7953Write(0x0FE, 0x00AD);  // Unlock register 0x120
  Ade7953Write(0x120, 0x0030);  // Configure optimum setting

  readValuesFromDevice();
  updateChannelValues();
}

void ADE7953::readValuesFromDevice() {
  int32_t reg[2][4];
  for (uint32_t i = 0; i < sizeof(Ade7953Registers) / sizeof(uint16_t); i++) {
    int32_t value = Ade7953Read(Ade7953Registers[i]);
    if (8 == i) {
      Ade7953.voltage_rms = value;  // RMS voltage (Both relays)
    }
    else if (9 == i) {
      Ade7953.period = value;  // Period
    }
    else if (10 == i) {
      Ade7953.acc_mode = value;  // Accumulation mode
                                 /*
                                       if (0 == Ade7953.model) {     // Shelly 2.5 - Swap channel B values due to hardware connection
                                 //        if (acc_mode & APSIGN[0]) { acc_mode &= ~APSIGN[0]; } else { acc_mode |= APSIGN[0]; }
                                 //        if (acc_mode & VARSIGN[0]) { acc_mode &= ~VARSIGN[0]; } else { acc_mode |= VARSIGN[0]; }
                                         acc_mode ^= (APSIGN[0] | VARSIGN[0]);
                                 //        acc_mode ^= 0xA00;
                                       }
                                 */
    }
    else {
      reg[i >> 2][i & 3] = value;
    }
  }
  // AddLog(LOG_LEVEL_DEBUG_MORE, PSTR("ADE: %d, %d, [%d, %d, %d, %d], [%d, %d, %d, %d]"), Ade7953.voltage_rms, Ade7953.period, reg[0][0], reg[0][1],
  //        reg[0][2], reg[0][3], reg[1][0], reg[1][1], reg[1][2], reg[1][3]);

  for (uint8_t channel = 0; channel < 2; channel++) {
    Ade7953.current_rms[channel] = reg[channel][0];

    if (Ade7953.current_rms[channel] < 2000) {  // No load threshold (20mA)
      Ade7953.current_rms[channel] = 0;
      Ade7953.active_power[channel] = 0;
      Ade7953.apparent_power[channel] = 0;
      Ade7953.reactive_power[channel] = 0;
    }
    else {
      Ade7953.active_power[channel] = abs(reg[channel][1]);
      Ade7953.apparent_power[channel] = abs(reg[channel][2]);
      Ade7953.reactive_power[channel] = abs(reg[channel][3]);
    }
  }

  // uint32_t current_rms_sum = Ade7953.current_rms[0] + Ade7953.current_rms[1];
  // AddLog(LOG_LEVEL_DEBUG_MORE, PSTR("ADE: U %d, C %d, I %d + %d = %d, P %d + %d"), Ade7953.voltage_rms, Ade7953.period, Ade7953.current_rms[0],
  //        Ade7953.current_rms[1], current_rms_sum, Ade7953.active_power[0], Ade7953.active_power[1]);
  bool activePower = false;
  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channelSupla = element->getChannel();

      if (channelSupla->getChannelType() == SUPLA_CHANNELTYPE_RELAY) {
        if (channelSupla->getValueBool()) {
          activePower = true;
        }
      }
    }
  }

  if (activePower) {
    energy += getEnergy(0) + getEnergy(1);
  }

  setVoltage(0, getVoltage() * 100);                                          // voltage in 0.01 V
  setCurrent(0, (getCurrent(0) + getCurrent(1)) * 10);                        // current in 0.001 A
  setPowerActive(0, (getActivePower(0) + getActivePower(1)) * 100000);        // power in 0.00001 kW
  setPowerApparent(0, (getApparentPower(0) + getApparentPower(1)) * 100000);  // power in 0.00001 kVA
  setPowerReactive(0, (getReactivePower(0) + getReactivePower(1)) * 100000);  // power in 0.00001 kvar
  setPowerFactor(0, (getPowerFactor(0) + getPowerFactor(1)) * 1000);          // power in 0.001
  setFwdActEnergy(0, energy);                                                 // energy in 0.00001 kWh
}

void ADE7953::onSaveState() {
  Supla::Storage::WriteState((unsigned char *)&energy, sizeof(energy));
  Supla::Storage::WriteState((unsigned char *)&Settings.energy_current_calibration, sizeof(Settings.energy_current_calibration));
  Supla::Storage::WriteState((unsigned char *)&Settings.energy_voltage_calibration, sizeof(Settings.energy_voltage_calibration));
  Supla::Storage::WriteState((unsigned char *)&Settings.energy_power_calibration, sizeof(Settings.energy_power_calibration));
}

void ADE7953::onLoadState() {
  if (Supla::Storage::ReadState((unsigned char *)&energy, sizeof(energy))) {
    setCounter(energy);
  }
  Supla::Storage::ReadState((unsigned char *)&Settings.energy_current_calibration, sizeof(Settings.energy_current_calibration));
  Supla::Storage::ReadState((unsigned char *)&Settings.energy_voltage_calibration, sizeof(Settings.energy_voltage_calibration));
  Supla::Storage::ReadState((unsigned char *)&Settings.energy_power_calibration, sizeof(Settings.energy_power_calibration));
}

int ADE7953::Ade7953RegSize(uint16_t reg) {
  int size = 0;
  switch ((reg >> 8) & 0x0F) {
    case 0x03:
      size++;
    case 0x02:
      size++;
    case 0x01:
      size++;
    case 0x00:
    case 0x07:
    case 0x08:
      size++;
  }
  return size;
}

void ADE7953::Ade7953Write(uint16_t reg, uint32_t val) {
  int size = Ade7953RegSize(reg);
  if (size) {
    Wire.beginTransmission(ADE7953_ADDR);
    Wire.write((reg >> 8) & 0xFF);
    Wire.write(reg & 0xFF);
    while (size--) {
      Wire.write((val >> (8 * size)) & 0xFF);  // Write data, MSB first
    }
    Wire.endTransmission();
    delayMicroseconds(5);  // Bus-free time minimum 4.7us
  }
}

int32_t ADE7953::Ade7953Read(uint16_t reg) {
  uint32_t response = 0;

  int size = Ade7953RegSize(reg);
  if (size) {
    Wire.beginTransmission(ADE7953_ADDR);
    Wire.write((reg >> 8) & 0xFF);
    Wire.write(reg & 0xFF);
    Wire.endTransmission(0);
    Wire.requestFrom(ADE7953_ADDR, size);
    if (size <= Wire.available()) {
      for (uint32_t i = 0; i < size; i++) {
        response = response << 8 | Wire.read();  // receive DATA (MSB first)
      }
    }
  }
  return response;
}

void ADE7953::Ade7953EnergyEverySecond(void) {
  if (Ade7953.init_step) {
    if (1 == Ade7953.init_step) {
      onInit();
    }
    Ade7953.init_step--;
  }
  else {
    readValuesFromDevice();
  }
}

_supla_int64_t ADE7953::getCounter() {
  return energy;
}

void ADE7953::setCounter(_supla_int64_t newEnergy) {
  energy = newEnergy;
  setFwdActEnergy(0, newEnergy);
}

double ADE7953::getVoltage() {
  return (double)Ade7953.voltage_rms / Settings.energy_voltage_calibration;
}

double ADE7953::getCurrent(uint8_t channel) {
  return (double)Ade7953.current_rms[channel] / Settings.energy_current_calibration;
}

double ADE7953::getActivePower(uint8_t channel) {
  double value = (double)Ade7953.active_power[channel] / Settings.energy_power_calibration;

  if (1 == Ade7953.model) {  // Shelly EM
    if ((Ade7953.acc_mode & APSIGN[channel]) != 0) {
      value = value * -1;
    }
  }

  return value;
}

double ADE7953::getApparentPower(uint8_t channel) {
  return (double)Ade7953.apparent_power[channel] / Settings.energy_power_calibration;
}

double ADE7953::getReactivePower(uint8_t channel) {
  double value = (double)Ade7953.reactive_power[channel] / Settings.energy_power_calibration;

  if (1 == Ade7953.model) {  // Shelly EM
    if ((Ade7953.acc_mode & VARSIGN[channel]) != 0) {
      value = value * -1;
    }
  }

  return value;
}

double ADE7953::getPowerFactor(uint8_t channel) {
  unsigned int active = getActivePower(channel);
  unsigned int apparent = getApparentPower(channel);
  if (active > apparent)
    return 1;
  if (apparent == 0)
    return 0;
  return (double)active / apparent;
}

unsigned long ADE7953::getEnergy(uint8_t channel) {
  return (double)(Ade7953Read(channel == 0 ? 0x31F : 0x31E)) * Settings.energy_power_calibration / 1000.0 / 36.0 * -1.0;
}

void ADE7953::expectedCurrent(double value) {
  Settings.energy_current_calibration *= (value / (getCurrent(0) + getCurrent(1)));
}

void ADE7953::expectedVoltage(double value) {
  Settings.energy_voltage_calibration *= (value / getVoltage());
}

void ADE7953::expectedActivePower(double value) {
  Settings.energy_power_calibration *= (value / (getActivePower(0) + getActivePower(1)));
}

void ADE7953::calibrate(double calibPower, double calibVoltage) {
  unsigned long timeout1 = millis();
  while ((millis() - timeout1) < 10000) {
    delay(10);
  }

  readValuesFromDevice();

  // Serial.print(F("[HLW] Active Power (W)    : "));
  // Serial.println(this->getActivePower());
  // Serial.print(F("[HLW] Voltage (V)         : "));
  // Serial.println(this->getVoltage());
  // Serial.print(F("[HLW] Current (A)         : "));
  // Serial.println(this->getCurrent());

  this->expectedActivePower(calibPower);
  this->expectedVoltage(calibVoltage);
  this->expectedCurrent(calibPower / calibVoltage);

  unsigned long timeout2 = millis();
  while ((millis() - timeout2) < 2000) {
    delay(10);
  }

  Serial.print(F("[HLW] New current multiplier : "));
  Serial.println(this->getCurrentMultiplier());
  Serial.print(F("[HLW] New voltage multiplier : "));
  Serial.println(this->getVoltageMultiplier());
  Serial.print(F("[HLW] New power multiplier   : "));
  Serial.println(this->getPowerMultiplier());
  Supla::Storage::ScheduleSave(2000);
  delay(0);
}

};  // namespace Sensor
};  // namespace Supla
