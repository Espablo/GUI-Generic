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

#ifndef _cse7766_h
#define _cse7766_h

#include <Arduino.h>
#include <EEPROM.h>

// https://github.com/ingeniuske/CSE7766
#include <CSE7766.h>
#include <supla/element.h>
#include <supla/sensor/one_phase_electricity_meter.h>
#include <supla/storage/storage.h>

#define CSE7766_CURRENT_RATIO 16030
#define CSE7766_VOLTAGE_RATIO 190770
#define CSE7766_POWER_RATIO   5195000

namespace Supla {
namespace Sensor {

class CSE_7766 : public OnePhaseElectricityMeter, public Element {
 public:
  CSE_7766(int8_t pinRX);

  void onInit();
  void readValuesFromDevice();
  void onSaveState();
  void onLoadState();

  double getCurrentMultiplier();
  double getVoltageMultiplier();
  double getPowerMultiplier();
  _supla_int64_t getCounter();

  void setCurrentMultiplier(double current_multiplier);
  void setVoltageMultiplier(double voltage_multiplier);
  void setPowerMultiplier(double power_multiplier);
  void setCounter(_supla_int64_t new_energy);

  void calibrate(double calibPower, double calibVoltage);

 protected:
  static CSE7766 *sensor;
  int8_t pinRX;

  unsigned _supla_int64_t energy = 0;
  unsigned _supla_int64_t _energy = 0;  // energy value read from memory at startup
};

};  // namespace Sensor
};  // namespace Supla

#endif