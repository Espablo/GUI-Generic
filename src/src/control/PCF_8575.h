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

#ifndef _S_PCF8575_H
#define _S_PCF8575_H

#include <Arduino.h>
#include <functional>

#include <supla/io.h>
#include <supla/element.h>
#include <PCF8575.h>

#include <Wire.h>

#define EXPENDER_SHIFT_PCF8575     144  // 80 + 64
#define EXPENDER_SHIFT_PIN_PCF8575 16

namespace Supla {
namespace Control {
class PCF_8575 : public Supla::Io, Supla::Element {
 public:
  PCF_8575();
  void onInit();
  void customDigitalWrite(int channelNumber, uint8_t pin, uint8_t val);
  int customDigitalRead(int channelNumber, uint8_t pin);
  void customPinMode(int channelNumber, uint8_t pin, uint8_t mode);
  TwoWire &getTwoWire(uint8_t address);

  PCF8575 *control1;
  PCF8575 *control2;
  PCF8575 *control3;
  PCF8575 *control4;
};

}  // namespace Control
}  // namespace Supla
#endif
