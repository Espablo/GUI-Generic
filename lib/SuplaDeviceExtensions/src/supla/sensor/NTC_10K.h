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
#ifndef _ntc10k_h
#define _ntc10k_h

#include <Arduino.h>
#include <supla/sensor/thermometer.h>

namespace Supla {
namespace Sensor {
class NTC10K : public Thermometer {
 public:
  NTC10K(int8_t pin, uint32_t rs = 150000, double vcc = 3.3);
  double getValue();

 private:
  void onInit();

 protected:
  int8_t pin;
  uint32_t rs;
  double vcc;
};
}  // namespace Sensor
}  // namespace Supla

#endif