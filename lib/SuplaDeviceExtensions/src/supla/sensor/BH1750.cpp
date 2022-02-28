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
#include "BH1750.h"

namespace Supla {
namespace Sensor {
BH1750::BH1750(int8_t address) {
  myBH1750 = new BH1750_WE(address);
}

double BH1750::getValue() {
  return myBH1750->getLux() / 1000;
}

void BH1750::onInit() {
  myBH1750->init();

  channel.setNewValue(getValue());
}
}  // namespace Sensor
}  // namespace Supla