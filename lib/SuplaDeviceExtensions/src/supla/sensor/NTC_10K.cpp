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
#include "NTC_10K.h"

namespace Supla {
namespace Sensor {
NTC10K::NTC10K(int8_t pin, uint32_t rs, double vcc)
    : pin(pin), rs(rs), vcc(vcc) {
}

double NTC10K::getTemp() {
  int val = 0;
  for (int i = 0; i < 10; i++) {
    val += analogRead(pin);
    delay(1);
  }
  val = val / 10;

  double V_NTC = (double)val / 1023;
  double R_NTC = (rs * V_NTC) / (vcc - V_NTC);

  R_NTC = log(R_NTC);
  double t = 1 / (0.001129148 +
                  (0.000234125 + (0.0000000876741 * R_NTC * R_NTC)) * R_NTC);
                  
  t = t - 276.15; //convert Kelvin to Celcius
  return t;
}

void NTC10K::iterateAlways() {
  if (lastReadTime + 10000 < millis()) {
    lastReadTime = millis();
    channel.setNewValue(getTemp());
  }
}

void NTC10K::onInit() {
  channel.setNewValue(getTemp());
}
}  // namespace Sensor
}  // namespace Supla