
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

#ifndef _analog_reding_map_h
#define _analog_reding_map_h

#include <Arduino.h>
#include <supla/storage/storage.h>

#include "humidity_meter.h"

namespace Supla {
namespace Sensor {

class AnalogRedingMap : public HumidityMeter {
 public:
  AnalogRedingMap(uint8_t pin);

  void onInit();
  int16_t readValuesFromDevice();
  virtual double getValue();

  void iterateAlways();

  void onSaveState();
  void onLoadState();

  double mapDouble(
      double x, double in_min, double in_max, double out_min, double out_max);

  void calibrateMinValue();
  void calibrateMaxValue();

  void setMinValue(int value);
  int getMinValue();

  void setMaxValue(int value);
  int getMaxValue();

  void setMinDesiredValue(int value);
  int getMinDesiredValue();

  void setMaxDesiredValue(int value);
  int getMaxDesiredValue();

 protected:
  uint8_t pin;

  int min;
  int max;
  int minDesired;
  int maxDesired;
};

};  // namespace Sensor
};  // namespace Supla

#endif
