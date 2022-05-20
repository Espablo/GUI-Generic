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
  if ((pin > 99) && (pin < 116)) {
    control1->write(pin - 100, val);
    return;
  }
  if ((pin > 115) && (pin < 132)) {
    control2->write(pin - 116, val);
    return;
  }
  if ((pin > 131) && (pin < 148)) {
    control3->write(pin - 132, val);
    return;
  }
  if ((pin > 147) && (pin < 164)) {
    control4->write(pin - 148, val);
    return;
  }

  return ::digitalWrite(pin, val);
}
int PCF_8575::customDigitalRead(int channelNumber, uint8_t pin) {
  if ((pin > 99) && (pin < 116)) {
    return control1->read(pin - 100);
  }
  if ((pin > 115) && (pin < 132)) {
    return control2->read(pin - 116);
  }
  if ((pin > 131) && (pin < 148)) {
    return control3->read(pin - 132);
  }
  if ((pin > 147) && (pin < 164)) {
    return control4->read(pin - 148);
  }

  return ::digitalRead(pin);
}
// void PCF_8575::customPinMode(int channelNumber, uint8_t pin, uint8_t mode) {
//   (void)(channelNumber);

//   if ((pin > 99) && (pin < 116)) {
//     control1->pinMode(pin - 100, mode);
//   }
//   if ((pin > 115) && (pin < 132)) {
//     control2->pinMode(pin - 116, mode);
//   }
//   if ((pin > 131) && (pin < 148)) {
//     control3->pinMode(pin - 132, mode);
//   }
//   if ((pin > 147) && (pin < 164)) {
//     control4->pinMode(pin - 148, mode);
//   }

//   return ::pinMode(pin, mode);
// }

// void PCF_8575::setPullup(uint8_t pin, bool pullup, bool inverse) {
//   if ((pin > 99) && (pin < 116)) {
//     control1->setPullup(pin - 100, pullup, inverse);
//   }
//   if ((pin > 115) && (pin < 132)) {
//     control2->setPullup(pin - 116, pullup, inverse);
//   }
//   if ((pin > 131) && (pin < 148)) {
//     control3->setPullup(pin - 132, pullup, inverse);
//   }
//   if ((pin > 147) && (pin < 164)) {
//     control4->setPullup(pin - 148, pullup, inverse);
//   }
// }
}  // namespace Control
}  // namespace Supla