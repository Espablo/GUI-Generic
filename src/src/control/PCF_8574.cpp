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

#include "PCF_8574.h"

namespace Supla {
namespace Control {

PCF_8574::PCF_8574() {
  control1 = new PCF8574();
  control2 = new PCF8574();
  control3 = new PCF8574();
  control4 = new PCF8574();

  control1->setAddress(0x20);
  if (control1->begin()) {
    Serial.println(F("PCF8574 1 found!"));
  }
  else {
    control1->setAddress(0x38);
    if (control1->begin())
      Serial.println(F("PCF8574A 1 found!"));
  }

  control2->setAddress(0x21);
  if (control2->begin()) {
    Serial.println(F("PCF8574 2 found!"));
  }
  else {
    control2->setAddress(0x39);
    if (control2->begin())
      Serial.println(F("PCF8574A 2 found!"));
  }

  control3->setAddress(0x22);
  if (control3->begin()) {
    Serial.println(F("PCF8574 3 found!"));
  }
  else {
    control3->setAddress(0x3A);
    if (control3->begin())
      Serial.println(F("PCF8574A 3 found!"));
  }

  control4->setAddress(0x23);
  if (control4->begin()) {
    Serial.println(F("PCF8574 4 found!"));
  }
  else {
    control4->setAddress(0x3B);
    if (control4->begin())
      Serial.println(F("PCF8574A 4 found!"));
  }
}

void PCF_8574::customDigitalWrite(int channelNumber, uint8_t pin, uint8_t val) {
  if ((pin >= EXPENDER_SHIFT_PCF8574) && (pin < EXPENDER_SHIFT_PIN_PCF8574 + EXPENDER_SHIFT_PCF8574)) {
    control1->write(pin - EXPENDER_SHIFT_PCF8574, val);
    return;
  }
  if ((pin >= EXPENDER_SHIFT_PIN_PCF8574 + EXPENDER_SHIFT_PCF8574) && (pin < (EXPENDER_SHIFT_PIN_PCF8574 * 2) + EXPENDER_SHIFT_PCF8574)) {
    control2->write(pin - EXPENDER_SHIFT_PCF8574 - EXPENDER_SHIFT_PIN_PCF8574, val);
    return;
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_PCF8574 * 2) + EXPENDER_SHIFT_PCF8574) && (pin < (EXPENDER_SHIFT_PIN_PCF8574 * 3) + EXPENDER_SHIFT_PCF8574)) {
    control3->write(pin - EXPENDER_SHIFT_PCF8574 - (EXPENDER_SHIFT_PIN_PCF8574 * 2), val);
    return;
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_PCF8574 * 3) + EXPENDER_SHIFT_PCF8574) && (pin < (EXPENDER_SHIFT_PIN_PCF8574 * 4) + EXPENDER_SHIFT_PCF8574)) {
    control4->write(pin - EXPENDER_SHIFT_PCF8574 - (EXPENDER_SHIFT_PIN_PCF8574 * 3), val);
    return;
  }

  return ::digitalWrite(pin, val);
}

int PCF_8574::customDigitalRead(int channelNumber, uint8_t pin) {
  if ((pin >= EXPENDER_SHIFT_PCF8574) && (pin < EXPENDER_SHIFT_PIN_PCF8574 + EXPENDER_SHIFT_PCF8574)) {
    return control1->read(pin - EXPENDER_SHIFT_PCF8574);
  }
  if ((pin >= EXPENDER_SHIFT_PIN_PCF8574 + EXPENDER_SHIFT_PCF8574) && (pin < (EXPENDER_SHIFT_PIN_PCF8574 * 2) + EXPENDER_SHIFT_PCF8574)) {
    return control2->read(pin - EXPENDER_SHIFT_PCF8574 - EXPENDER_SHIFT_PIN_PCF8574);
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_PCF8574 * 2) + EXPENDER_SHIFT_PCF8574) && (pin < (EXPENDER_SHIFT_PIN_PCF8574 * 3) + EXPENDER_SHIFT_PCF8574)) {
    return control3->read(pin - EXPENDER_SHIFT_PCF8574 - (EXPENDER_SHIFT_PIN_PCF8574 * 2));
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_PCF8574 * 3) + EXPENDER_SHIFT_PCF8574) && (pin < (EXPENDER_SHIFT_PIN_PCF8574 * 4) + EXPENDER_SHIFT_PCF8574)) {
    return control4->read(pin - EXPENDER_SHIFT_PCF8574 - (EXPENDER_SHIFT_PIN_PCF8574 * 3));
  }

  return ::digitalRead(pin);
}
}  // namespace Control
}  // namespace Supla