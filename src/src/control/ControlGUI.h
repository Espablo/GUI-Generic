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

#ifndef _Control_GUI_h
#define _Control_GUI_h

#include <supla/control/button.h>
#include <supla/control/relay.h>
#include <supla/control/roller_shutter.h>
#include <supla/sensor/binary.h>

namespace Supla {
namespace Control {
namespace GUI {

Supla::Control::Button *Button(int pin, bool pullUp, bool invertLogic, uint8_t nr);
Supla::Control::Relay *Relay(int pin, bool highIsOn, uint8_t nr);
Supla::Control::RollerShutter *RollerShutter(int pinUp, int pinDown, bool highIsOn, uint8_t nr);
Supla::Sensor::Binary *Binary(int pin, bool pullUp, bool invertLogic, uint8_t nr);

};  // namespace GUI
};  // namespace Control
};  // namespace Supla

#endif
