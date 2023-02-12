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

#include "ExpanderPCF8575.h"

namespace Supla {
namespace Control {
#ifdef ARDUINO_ARCH_ESP32
ExpanderPCF8575::ExpanderPCF8575(TwoWire *wire, uint8_t address) : Supla::Io(false) {
  _control = new PCF8575(wire, address);

  wire->beginTransmission(address);
  if (wire->endTransmission() == 0) {
    isConnected = true;
    Serial.print("PCF8575 is connected address: ");
    Serial.println(address, HEX);
  }
}
#else
ExpanderPCF8575::ExpanderPCF8575(uint8_t address) : Supla::Io(false) {
  _control = new PCF8575(&Wire, address);

  wire->beginTransmission(address);
  if (wire->endTransmission() == 0) {
    isConnected = true;
    Serial.print("PCF8575 is connected address: ");
    Serial.println(address, HEX);
  }
}
#endif

void ExpanderPCF8575::onInit() {
  _control->begin();
}

void ExpanderPCF8575::customPinMode(int channelNumber, uint8_t pin, uint8_t mode) {
  _control->pinMode(pin, mode);
}

int ExpanderPCF8575::customDigitalRead(int channelNumber, uint8_t pin) {
  uint8_t val = 0;
  if (isConnected) {
    val = _control->digitalRead(pin);
    delay(10);
  }
  return val;
}

void ExpanderPCF8575::customDigitalWrite(int channelNumber, uint8_t pin, uint8_t val) {
  _control->digitalWrite(pin, val);
}

}  // namespace Control
}  // namespace Supla