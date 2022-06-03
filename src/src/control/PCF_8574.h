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

#ifndef _S_PCF8574_H
#define _S_PCF8574_H

#include <Arduino.h>
#include <functional>

#include <supla/io.h>
#include <PCF8574.h>

#define EXPENDER_SHIFT_PCF8574     208  // 80 + 64 + 64
#define EXPENDER_SHIFT_PIN_PCF8574 8

namespace Supla {
namespace Control {
class PCF_8574 : public Supla::Io {
 public:
  PCF_8574();
  void customDigitalWrite(int channelNumber, uint8_t pin, uint8_t val);
  int customDigitalRead(int channelNumber, uint8_t pin);

  PCF8574 *control1;
  PCF8574 *control2;
  PCF8574 *control3;
  PCF8574 *control4;
};

}  // namespace Control
}  // namespace Supla
#endif
