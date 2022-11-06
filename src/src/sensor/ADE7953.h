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

#ifndef _ade7953_h
#define _ade7953_h

#include <Arduino.h>
#include <Wire.h>
#include <supla/sensor/one_phase_electricity_meter.h>
#include <supla/storage/storage.h>

#define ADE7953_PREF 155
#define ADE7953_UREF 26000
#define ADE7953_IREF 1000

#define ADE7953_ADDR 0x38

// 24-bit data registers
const uint16_t Ade7953Registers[]{
    0x31B,  // IRMSB - RMS current channel B (Relay 1)
    0x313,  // BWATT - Active power channel B
    0x311,  // BVA - Apparent power channel B
    0x315,  // BVAR - Reactive power channel B
    0x31A,  // IRMSA - RMS current channel A (Relay 2)
    0x312,  // AWATT - Active power channel A
    0x310,  // AVA - Apparent power channel A
    0x314,  // AVAR - Reactive power channel A
    0x31C,  // VRMS - RMS voltage (Both relays)
    0x10E,  // Period - 16-bit unsigned period register
    0x301,  // ACCMODE - Accumulation mode
};

// Active power
const uint16_t APSIGN[]{
    0x800,  // Bit 10 (21 bits) in ACCMODE Register for channel A (0 - positive, 1 - negative)
    0x400   // Bit 11 (21 bits) in ACCMODE Register for channel B (0 - positive, 1 - negative)
};
// Reactive power
const uint16_t VARSIGN[]{
    0x200,  // Bit 12 (21 bits) in ACCMODE Register for channel A (0 - positive, 1 - negative)
    0x100   // Bit 13 (21 bits) in ACCMODE Register for channel B (0 - positive, 1 - negative)
};

namespace Supla {
namespace Sensor {

class ADE7953 : public OnePhaseElectricityMeter {
 public:
  ADE7953(uint8_t pin_irq, uint8_t model = 0);

  void onInit();
  void readValuesFromDevice();
  void onSaveState();
  void onLoadState();

  int Ade7953RegSize(uint16_t reg);
  void Ade7953Write(uint16_t reg, uint32_t val);
  int32_t Ade7953Read(uint16_t reg);
  void Ade7953EnergyEverySecond(void);

  _supla_int64_t getCounter();

  void setMode(bool value);
  void setCounter(_supla_int64_t value);
  int handleCalcfgFromServer(TSD_DeviceCalCfgRequest* request);

  void calibrate(double calibPower, double calibVoltage);

  double getVoltage();
  double getCurrent(uint8_t channel);
  double getActivePower(uint8_t channel);
  double getApparentPower(uint8_t channel);
  double getPowerFactor(uint8_t channel);
  double getReactivePower(uint8_t channel);
  unsigned long getEnergy(uint8_t channel);

  double getCurrentMultiplier() {
    return Settings.energy_current_calibration;
  };
  double getVoltageMultiplier() {
    return Settings.energy_voltage_calibration;
  };
  double getPowerMultiplier() {
    return Settings.energy_power_calibration;
  };

  void setCurrentMultiplier(double current_multiplier) {
    Settings.energy_current_calibration = current_multiplier;
  };
  void setVoltageMultiplier(double voltage_multiplier) {
    Settings.energy_voltage_calibration = voltage_multiplier;
  };
  void setPowerMultiplier(double power_multiplier) {
    Settings.energy_power_calibration = power_multiplier;
  };

  void expectedCurrent(double current);
  void expectedVoltage(double voltage);
  void expectedActivePower(double power);

 protected:
  unsigned _supla_int64_t energy = 0;

  struct Ade7953 {
    uint32_t voltage_rms = 0;
    uint32_t period = 0;
    uint32_t current_rms[2] = {0, 0};
    uint32_t active_power[2] = {0, 0};
    uint32_t apparent_power[2] = {0, 0};
    uint32_t reactive_power[2] = {0, 0};
    uint8_t init_step = 0;
    uint8_t model = 0;  // 0 = Shelly 2.5, 1 = Shelly EM
    uint32_t acc_mode = 0;
  } Ade7953;

  struct Settings {
    int energy_power_calibration = ADE7953_PREF;
    int energy_voltage_calibration = ADE7953_UREF;
    int energy_current_calibration = ADE7953_IREF;
  } Settings;
};

};  // namespace Sensor
};  // namespace Supla

#endif