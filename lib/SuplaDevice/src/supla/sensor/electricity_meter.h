/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

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

#ifndef _electricity_meter_h
#define _electricity_meter_h

#include <string.h>
#include <Arduino.h>

#include "../channel_extended.h"
#include "../element.h"
#include <supla-common/srpc.h>

#define MAX_PHASES 3

namespace Supla {
namespace Sensor {
class ElectricityMeter : public Element {
 public:
  ElectricityMeter() : valueChanged(false), lastReadTime(0) {
    extChannel.setType(SUPLA_CHANNELTYPE_ELECTRICITY_METER);
    extChannel.setDefault(SUPLA_CHANNELFNC_ELECTRICITY_METER);
    memset(&emValue, 0, sizeof(emValue));
    emValue.period = 5;
    for (int i = 0; i < MAX_PHASES; i++) {
      rawCurrent[i] = 0;
    }
    currentMeasurementAvailable = false;
  }

  virtual void updateChannelValues() {
    if (!valueChanged) {
      return;
    }
    valueChanged = false;

    emValue.m_count = 1;

    // Update current messurement precision based on last updates
    if (currentMeasurementAvailable) {
      bool over65A = false;
      for (int i = 0; i < MAX_PHASES; i++) {
        if (rawCurrent[i] > 65000) {
          over65A = true;
        }
      }

      for (int i = 0; i < MAX_PHASES; i++) {
        if (over65A) {
          emValue.m[0].current[i] = rawCurrent[i] / 10;
        } else {
          emValue.m[0].current[i] = rawCurrent[i];
        }
      }

      if (over65A) {
        emValue.measured_values ^= (!EM_VAR_CURRENT);
        emValue.measured_values |= EM_VAR_CURRENT_OVER_65A;
      } else {
        emValue.measured_values ^= (!EM_VAR_CURRENT_OVER_65A);
        emValue.measured_values |= EM_VAR_CURRENT;
      }
    }

    // Prepare extended channel value
    srpc_evtool_v2_emextended2extended(&emValue, extChannel.getExtValue());
    extChannel.setNewValue(emValue);
  }

  // energy in 0.00001 kWh
  void setFwdActEnergy(int phase, unsigned _supla_int64_t energy) {
    if (phase >= 0 && phase < MAX_PHASES) {
      if (emValue.total_forward_active_energy[phase] != energy) {
        valueChanged = true;
      }
      emValue.total_forward_active_energy[phase] = energy;
      emValue.measured_values |= EM_VAR_FORWARD_ACTIVE_ENERGY;
    }
  }

  // energy in 0.00001 kWh
  void setRvrActEnergy(int phase, unsigned _supla_int64_t energy) {
    if (phase >= 0 && phase < MAX_PHASES) {
      if (emValue.total_reverse_active_energy[phase] != energy) {
        valueChanged = true;
      }
      emValue.total_reverse_active_energy[phase] = energy;
      emValue.measured_values |= EM_VAR_REVERSE_ACTIVE_ENERGY;
    }
  }

  // energy in 0.00001 kWh
  void setFwdReactEnergy(int phase, unsigned _supla_int64_t energy) {
    if (phase >= 0 && phase < MAX_PHASES) {
      if (emValue.total_forward_reactive_energy[phase] != energy) {
        valueChanged = true;
      }
      emValue.total_forward_reactive_energy[phase] = energy;
      emValue.measured_values |= EM_VAR_FORWARD_REACTIVE_ENERGY;
    }
  }

  // energy in 0.00001 kWh
  void setRvrReactEnergy(int phase, unsigned _supla_int64_t energy) {
    if (phase >= 0 && phase < MAX_PHASES) {
      if (emValue.total_reverse_reactive_energy[phase] != energy) {
        valueChanged = true;
      }
      emValue.total_reverse_reactive_energy[phase] = energy;
      emValue.measured_values |= EM_VAR_REVERSE_REACTIVE_ENERGY;
    }
  }

  // voltage in 0.01 V
  void setVoltage(int phase, unsigned _supla_int16_t voltage) {
    if (phase >= 0 && phase < MAX_PHASES) {
      if (emValue.m[0].voltage[phase] != voltage) {
        valueChanged = true;
      }
      emValue.m[0].voltage[phase] = voltage;
      emValue.measured_values |= EM_VAR_VOLTAGE;
    }
  }

  // current in 0.001 A
  void setCurrent(int phase, unsigned _supla_int_t current) {
    if (phase >= 0 && phase < MAX_PHASES) {
      if (rawCurrent[phase] != current) {
        valueChanged = true;
      }
      rawCurrent[phase] = current;
      currentMeasurementAvailable = true;
    }
  }

  // Frequency in 0.01 Hz
  void setFreq(unsigned _supla_int16_t freq) {
    if (emValue.m[0].freq != freq) {
      valueChanged = true;
    }
    emValue.m[0].freq = freq;
    emValue.measured_values |= EM_VAR_FREQ;
  }

  // power in 0.00001 kW
  void setPowerActive(int phase, _supla_int_t power) {
    if (phase >= 0 && phase < MAX_PHASES) {
      if (emValue.m[0].power_active[phase] != power) {
        valueChanged = true;
      }
      emValue.m[0].power_active[phase] = power;
      emValue.measured_values |= EM_VAR_POWER_ACTIVE;
    }
  }

  // power in 0.00001 kvar
  void setPowerReactive(int phase, _supla_int_t power) {
    if (phase >= 0 && phase < MAX_PHASES) {
      if (emValue.m[0].power_reactive[phase] != power) {
        valueChanged = true;
      }
      emValue.m[0].power_reactive[phase] = power;
      emValue.measured_values |= EM_VAR_POWER_REACTIVE;
    }
  }

  // power in 0.00001 kVA
  void setPowerApparent(int phase, _supla_int_t power) {
    if (phase >= 0 && phase < MAX_PHASES) {
      if (emValue.m[0].power_apparent[phase] != power) {
        valueChanged = true;
      }
      emValue.m[0].power_apparent[phase] = power;
      emValue.measured_values |= EM_VAR_POWER_APPARENT;
    }
  }

  // power in 0.001
  void setPowerFactor(int phase, _supla_int_t powerFactor) {
    if (phase >= 0 && phase < MAX_PHASES) {
      if (emValue.m[0].power_factor[phase] != powerFactor) {
        valueChanged = true;
      }
      emValue.m[0].power_factor[phase] = powerFactor;
      emValue.measured_values |= EM_VAR_POWER_FACTOR;
    }
  }

  // phase angle in 0.1 degree
  void setPhaseAngle(int phase, _supla_int_t phaseAngle) {
    if (phase >= 0 && phase < MAX_PHASES) {
      if (emValue.m[0].phase_angle[phase] != phaseAngle) {
        valueChanged = true;
      }
      emValue.m[0].phase_angle[phase] = phaseAngle;
      emValue.measured_values |= EM_VAR_PHASE_ANGLE;
    }
  }

  void resetReadParameters() {
    if (emValue.measured_values != 0) {
      emValue.measured_values = 0;
      memset(&emValue.m[0], 0, sizeof(TElectricityMeter_Measurement));
      valueChanged = true;
    }
  }

  // Please implement this class for reading value from elecricity meter device.
  // It will be called every 5 s. Use set methods defined above in order to
  // set values on channel. Don't use any other method to modify channel values.
  virtual void readValuesFromDevice() {
  }

  // Put here initialization code for electricity meter device.
  // It will be called within SuplaDevce.begin method.
  // It should also read first data set, so at the end it should call those two
  // methods:
  // readValuesFromDevice();
  // updateChannelValues();
  void onInit() {
  }

  void iterateAlways() {
    if (lastReadTime + 5000 < millis()) {
      lastReadTime = millis();
      readValuesFromDevice();
      updateChannelValues();
    }
  }

  // Implement this method to reset stored energy value (i.e. to set energy
  // counter back to 0 kWh
  virtual void resetStorage() {
  }

 protected:
  Channel *getChannel() {
    return &extChannel;
  }
  TElectricityMeter_ExtendedValue_V2 emValue;
  ChannelExtended extChannel;
  unsigned _supla_int_t rawCurrent[MAX_PHASES];
  bool valueChanged;
  bool currentMeasurementAvailable;
  unsigned long lastReadTime;
};

};  // namespace Sensor
};  // namespace Supla

#endif
