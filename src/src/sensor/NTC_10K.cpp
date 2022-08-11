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
NTC10K::NTC10K(int8_t pin) : pin(pin) {
}

double NTC10K::getValue() {
  int adc = analogRead(pin);

  double Rt = (adc * ANALOG_NTC_BRIDGE_RESISTANCE) / (1024.0 * ANALOG_V33 - (double)adc);
  double BC = (double)ANALOG_NTC_B_COEFFICIENT;
  double T = BC / (BC / ANALOG_T0 + TaylorLog(Rt / (double)ANALOG_NTC_RESISTANCE));

  return (double)TO_CELSIUS(T);
}

double NTC10K::TaylorLog(double x) {
  double z = (x + 1) / (x - 1);                             // We start from power -1, to make sure we get the right power in each iteration;
  double step = ((x - 1) * (x - 1)) / ((x + 1) * (x + 1));  // Store step to not have to calculate it each time
  double totalValue = 0;
  double powe = 1;
  double y;
  for (uint32_t count = 0; count < 10; count++) {  // Experimental number of 10 iterations
    z *= step;
    y = (1 / powe) * z;
    totalValue = totalValue + y;
    powe = powe + 2;
  }
  totalValue *= 2;
  return totalValue;
}

void NTC10K::onInit() {
  channel.setNewValue(getValue());
}
}  // namespace Sensor
}  // namespace Supla