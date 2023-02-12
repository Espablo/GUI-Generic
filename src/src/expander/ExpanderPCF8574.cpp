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

#include "ExpanderPCF8574.h"

namespace Supla {
namespace Control {
ExpanderPCF8574::ExpanderPCF8574(TwoWire *wire, uint8_t address) : Supla::Io(false) {
#ifdef ARDUINO_ARCH_ESP32
  _control = new PCF8574(wire, address);
#else
  _control = new PCF8574(address);
#endif

  wire->beginTransmission(address);
  if (wire->endTransmission() == 0) {
    isConnected = true;
    Serial.print("PCF8574 is connected address: ");
    Serial.println(address, HEX);
  }
}

void ExpanderPCF8574::onInit() {
  _control->begin();
}

void ExpanderPCF8574::customPinMode(int channelNumber, uint8_t pin, uint8_t mode) {
  _control->pinMode(pin, mode);
}

int ExpanderPCF8574::customDigitalRead(int channelNumber, uint8_t pin) {
  uint8_t val = 0;
  if (isConnected) {
    val = _control->digitalRead(pin);
    delay(10);
  }
  return val;
}

void ExpanderPCF8574::customDigitalWrite(int channelNumber, uint8_t pin, uint8_t val) {
  _control->digitalWrite(pin, val);
}

}  // namespace Control
}  // namespace Supla