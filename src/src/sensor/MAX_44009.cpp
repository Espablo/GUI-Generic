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
#include "MAX_44009.h"

namespace Supla {
namespace Sensor {
MAX_44009::MAX_44009() {
  sensor = new Max44009(0x4A);
}

double MAX_44009::getValue() {
  int err = sensor->getError();

  if (err != MAX44009_OK) {
    Serial.print(F("MAX44009 [ERROR] Code #"));
    Serial.println(err);
    retryCount++;
    if (retryCount > 3) {
      retryCount = 0;
      lux = TEMPERATURE_NOT_AVAILABLE;
    }
  }
  else {
    retryCount = 0;
    lux = sensor->getLux() / 1000;
  }
  return lux;
}

void MAX_44009::onInit() {
  channel.setNewValue(getValue());
}
}  // namespace Sensor
}  // namespace Supla