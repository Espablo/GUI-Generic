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

#ifndef _vl_53l0x_h
#define _vl_53l0x_h

#include <Adafruit_VL53L0X.h>
#include <Arduino.h>

#include "supla/sensor/distance.h"

namespace Supla {
namespace Sensor {
class VL_53L0X : public Distance {
 public:
  VL_53L0X(int8_t address = VL53L0X_I2C_ADDR,
           Adafruit_VL53L0X::VL53L0X_Sense_config_t vl_config =
               Adafruit_VL53L0X::VL53L0X_SENSE_DEFAULT);

  void onInit();
  virtual double getValue();
  void iterateAlways();

 protected:
  int8_t address;
  bool sensorStatus;

  Adafruit_VL53L0X *lox;
};

};  // namespace Sensor
};  // namespace Supla

#endif
