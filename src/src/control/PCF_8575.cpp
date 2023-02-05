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
#include "../../SuplaDeviceGUI.h"

namespace Supla {
namespace Control {

PCF_8575::PCF_8575() {
#ifdef ARDUINO_ARCH_ESP32
  control1 = new PCF8575(&getTwoWire(0), 0x20);
  control2 = new PCF8575(&getTwoWire(1), 0x21);
  control3 = new PCF8575(&getTwoWire(2), 0x22);
  control4 = new PCF8575(&getTwoWire(3), 0x23);
#else
  control1 = new PCF8575(0x20);
  control2 = new PCF8575(0x21);
  control3 = new PCF8575(0x22);
  control4 = new PCF8575(0x23);
#endif
}

void PCF_8575::onInit() {
  control1->begin();
  control2->begin();
  control3->begin();
  control4->begin();
}

void PCF_8575::customDigitalWrite(int channelNumber, uint8_t pin, uint8_t val) {
  if ((pin >= EXPENDER_SHIFT_PCF8575) && (pin < EXPENDER_SHIFT_PIN_PCF8575 + EXPENDER_SHIFT_PCF8575)) {
    control1->digitalWrite(pin - EXPENDER_SHIFT_PCF8575, val);
    return;
  }
  if ((pin >= EXPENDER_SHIFT_PIN_PCF8575 + EXPENDER_SHIFT_PCF8575) && (pin < (EXPENDER_SHIFT_PIN_PCF8575 * 2) + EXPENDER_SHIFT_PCF8575)) {
    control2->digitalWrite(pin - EXPENDER_SHIFT_PCF8575 - EXPENDER_SHIFT_PIN_PCF8575, val);
    return;
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_PCF8575 * 2) + EXPENDER_SHIFT_PCF8575) && (pin < (EXPENDER_SHIFT_PIN_PCF8575 * 3) + EXPENDER_SHIFT_PCF8575)) {
    control3->digitalWrite(pin - EXPENDER_SHIFT_PCF8575 - (EXPENDER_SHIFT_PIN_PCF8575 * 2), val);
    return;
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_PCF8575 * 3) + EXPENDER_SHIFT_PCF8575) && (pin < (EXPENDER_SHIFT_PIN_PCF8575 * 4) + EXPENDER_SHIFT_PCF8575)) {
    control4->digitalWrite(pin - EXPENDER_SHIFT_PCF8575 - (EXPENDER_SHIFT_PIN_PCF8575 * 3), val);
    return;
  }

  return ::digitalWrite(pin, val);
}

int PCF_8575::customDigitalRead(int channelNumber, uint8_t pin) {
  if ((pin >= EXPENDER_SHIFT_PCF8575) && (pin < EXPENDER_SHIFT_PIN_PCF8575 + EXPENDER_SHIFT_PCF8575)) {
    return control1->digitalRead(pin - EXPENDER_SHIFT_PCF8575);
  }
  if ((pin >= EXPENDER_SHIFT_PIN_PCF8575 + EXPENDER_SHIFT_PCF8575) && (pin < (EXPENDER_SHIFT_PIN_PCF8575 * 2) + EXPENDER_SHIFT_PCF8575)) {
    return control2->digitalRead(pin - EXPENDER_SHIFT_PCF8575 - EXPENDER_SHIFT_PIN_PCF8575);
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_PCF8575 * 2) + EXPENDER_SHIFT_PCF8575) && (pin < (EXPENDER_SHIFT_PIN_PCF8575 * 3) + EXPENDER_SHIFT_PCF8575)) {
    return control3->digitalRead(pin - EXPENDER_SHIFT_PCF8575 - (EXPENDER_SHIFT_PIN_PCF8575 * 2));
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_PCF8575 * 3) + EXPENDER_SHIFT_PCF8575) && (pin < (EXPENDER_SHIFT_PIN_PCF8575 * 4) + EXPENDER_SHIFT_PCF8575)) {
    return control4->digitalRead(pin - EXPENDER_SHIFT_PCF8575 - (EXPENDER_SHIFT_PIN_PCF8575 * 3));
  }

  return ::digitalRead(pin);
}

void PCF_8575::customPinMode(int channelNumber, uint8_t pin, uint8_t mode) {
  if ((pin >= EXPENDER_SHIFT_PCF8575) && (pin < EXPENDER_SHIFT_PIN_PCF8575 + EXPENDER_SHIFT_PCF8575)) {
    control1->pinMode(pin - EXPENDER_SHIFT_PCF8575, mode);
  }
  if ((pin >= EXPENDER_SHIFT_PIN_PCF8575 + EXPENDER_SHIFT_PCF8575) && (pin < (EXPENDER_SHIFT_PIN_PCF8575 * 2) + EXPENDER_SHIFT_PCF8575)) {
    control2->pinMode(pin - EXPENDER_SHIFT_PCF8575 - EXPENDER_SHIFT_PIN_PCF8575, mode);
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_PCF8575 * 2) + EXPENDER_SHIFT_PCF8575) && (pin < (EXPENDER_SHIFT_PIN_PCF8575 * 3) + EXPENDER_SHIFT_PCF8575)) {
    control3->pinMode(pin - EXPENDER_SHIFT_PCF8575 - (EXPENDER_SHIFT_PIN_PCF8575 * 2), mode);
  }
  if ((pin >= (EXPENDER_SHIFT_PIN_PCF8575 * 3) + EXPENDER_SHIFT_PCF8575) && (pin < (EXPENDER_SHIFT_PIN_PCF8575 * 4) + EXPENDER_SHIFT_PCF8575)) {
    control4->pinMode(pin - EXPENDER_SHIFT_PCF8575 - (EXPENDER_SHIFT_PIN_PCF8575 * 3), mode);
  }
}

TwoWire& PCF_8575::getTwoWire(uint8_t address) {
#ifdef ARDUINO_ARCH_ESP32
  if (ConfigESP->getAdressMCP23017(0, FUNCTION_RELAY) == address &&
      ConfigManager->get(KEY_ACTIVE_EXPENDER)->getElement(FUNCTION_RELAY).toInt() == EXPENDER_PCF8575_I2C2) {
    Serial.print(F("Add Wire1 for PCF8575 address: "));
    Serial.println(address);
    return Wire1;
  }
  if (ConfigESP->getAdressMCP23017(0, FUNCTION_BUTTON) == address &&
      ConfigManager->get(KEY_ACTIVE_EXPENDER)->getElement(FUNCTION_BUTTON).toInt() == EXPENDER_PCF8575_I2C2) {
    Serial.print(F("Add Wire1 for PCF8575 address: "));
    Serial.println(address);
    return Wire1;
  }
  if (ConfigESP->getAdressMCP23017(0, FUNCTION_LIMIT_SWITCH) == address &&
      ConfigManager->get(KEY_ACTIVE_EXPENDER)->getElement(FUNCTION_LIMIT_SWITCH).toInt() == EXPENDER_PCF8575_I2C2) {
    Serial.print(F("Add Wire1 for PCF8575 address: "));
    Serial.println(address);
    return Wire1;
  }
#endif

  return Wire;
}

}  // namespace Control
}  // namespace Supla