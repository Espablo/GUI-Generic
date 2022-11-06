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

#ifndef _hlw8012_h
#define _hlw8012_h

#include <Arduino.h>

// https://github.com/xoseperez/hlw8012
#include <HLW8012.h>
#include <supla/sensor/one_phase_electricity_meter.h>
#include <supla/storage/storage.h>

namespace Supla {
namespace Sensor {

class HLW_8012 : public OnePhaseElectricityMeter {
 public:
  HLW_8012(int8_t pinCF,
           int8_t pinCF1,
           int8_t pinSEL,
           bool useInterrupts = true);

  void onInit();
  void readValuesFromDevice();
  void onSaveState();
  void onLoadState();

  double getCurrentMultiplier();
  double getVoltageMultiplier();
  double getPowerMultiplier();
  bool getMode();
  _supla_int64_t getCounter();

  void setCurrentMultiplier(double value);
  void setVoltageMultiplier(double value);
  void setPowerMultiplier(double value);
  void setMode(bool value);
  void setCounter(_supla_int64_t value);
  int handleCalcfgFromServer(TSD_DeviceCalCfgRequest *request);

  static IRAM_ATTR void hjl01_cf1_interrupt();
  static IRAM_ATTR void hjl01_cf_interrupt();
  void calibrate(double calibPower, double calibVoltage);

 protected:
  static HLW8012 *sensor;
  int8_t pinCF;
  int8_t pinCF1;
  int8_t pinSEL;

  bool useInterrupts;
  double currentMultiplier;
  double voltageMultiplier;
  double powerMultiplier;
  bool currentWhen;

  unsigned _supla_int64_t energy = 0;
  unsigned _supla_int64_t _energy =
      0;  // energy value read from memory at startup
};

};  // namespace Sensor
};  // namespace Supla

#endif