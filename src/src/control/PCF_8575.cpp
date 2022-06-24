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

#include "PCF_8575.h"

namespace Supla {
namespace Control {

PCF_8575::PCF_8575() {
  control1 = new PCF8575(0x20);
  control2 = new PCF8575(0x21);
  control3 = new PCF8575(0x22);
  control4 = new PCF8575(0x23);

  if (!control1->begin()) {
    Serial.println(F("PCF8575 1 not found!"));
  }
  if (!control2->begin()) {
    Serial.println(F("PCF8575 2 not found!"));
  }
  if (!control3->begin()) {
    Serial.println(F("PCF8575 3 not found!"));
  }
  if (!control4->begin()) {
    Serial.println(F("PCF8575 4 not found!"));
  }
}

void PCF_8575::customDigitalWrite(int channelNumber, uint8_t pin, uint8_t val) {
  if ((pin >= EXPENDER_SHIFT_PCF8575) && (pin < EXPENDER_SHIFT_PIN_PCF8575 + EXPENDER_SHIFT_PCF8575)) {
    control1->write(pin - EXPENDER_SHIFT_PCF8575, val);
    return;
  }
  if ((pin >= EXPENDER_SHIFT_PIN_PCF8575 + EXPENDER_SHIFT_PCF8575) && (pin < (EXPENDER_SHIFT_PIN_PCF8575 * 2) + EXPENDER_SHIFT_PCF8575)) {
    control2->write(pin - EXPENDER_SHIFT_PCF8575 - EXPENDER_SHIFT_PIN_PCF8575, val);
    return;
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_PCF8575 * 2) + EXPENDER_SHIFT_PCF8575) && (pin < (EXPENDER_SHIFT_PIN_PCF8575 * 3) + EXPENDER_SHIFT_PCF8575)) {
    control3->write(pin - EXPENDER_SHIFT_PCF8575 - (EXPENDER_SHIFT_PIN_PCF8575 * 2), val);
    return;
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_PCF8575 * 3) + EXPENDER_SHIFT_PCF8575) && (pin < (EXPENDER_SHIFT_PIN_PCF8575 * 4) + EXPENDER_SHIFT_PCF8575)) {
    control4->write(pin - EXPENDER_SHIFT_PCF8575 - (EXPENDER_SHIFT_PIN_PCF8575 * 3), val);
    return;
  }

  return ::digitalWrite(pin, val);
}

int PCF_8575::customDigitalRead(int channelNumber, uint8_t pin) {
  if ((pin >= EXPENDER_SHIFT_PCF8575) && (pin < EXPENDER_SHIFT_PIN_PCF8575 + EXPENDER_SHIFT_PCF8575)) {
    return control1->read(pin - EXPENDER_SHIFT_PCF8575);
  }
  if ((pin >= EXPENDER_SHIFT_PIN_PCF8575 + EXPENDER_SHIFT_PCF8575) && (pin < (EXPENDER_SHIFT_PIN_PCF8575 * 2) + EXPENDER_SHIFT_PCF8575)) {
    return control2->read(pin - EXPENDER_SHIFT_PCF8575 - EXPENDER_SHIFT_PIN_PCF8575);
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_PCF8575 * 2) + EXPENDER_SHIFT_PCF8575) && (pin < (EXPENDER_SHIFT_PIN_PCF8575 * 3) + EXPENDER_SHIFT_PCF8575)) {
    return control3->read(pin - EXPENDER_SHIFT_PCF8575 - (EXPENDER_SHIFT_PIN_PCF8575 * 2));
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_PCF8575 * 3) + EXPENDER_SHIFT_PCF8575) && (pin < (EXPENDER_SHIFT_PIN_PCF8575 * 4) + EXPENDER_SHIFT_PCF8575)) {
    return control4->read(pin - EXPENDER_SHIFT_PCF8575 - (EXPENDER_SHIFT_PIN_PCF8575 * 3));
  }

  return ::digitalRead(pin);
}
}  // namespace Control
}  // namespace Supla