/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

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

#include <supla/io.h>
#include "PinStatusLedGUI.h"

Supla::Control::PinStatusLedGUI::PinStatusLedGUI(uint8_t srcPin, uint8_t outPin, bool invert)
    : srcPin(srcPin), outPin(outPin), invert(invert), status(-1) {
}

void Supla::Control::PinStatusLedGUI::onInit() {
  updatePin();
  Supla::Io::pinMode(outPin, OUTPUT);
}

void Supla::Control::PinStatusLedGUI::iterateAlways() {
  updatePin();
}

void Supla::Control::PinStatusLedGUI::setInvertedLogic(bool invertedLogic) {
  invert = invertedLogic;
  updatePin();
}

void Supla::Control::PinStatusLedGUI::updatePin() {
  int value = Supla::Io::digitalRead(srcPin);
  value = invert ? !value : value;
  if (value != status) {
    status = value;
    if (value != Supla::Io::digitalRead(outPin)) {
      Supla::Io::digitalWrite(outPin, value);
    }
  }
}
