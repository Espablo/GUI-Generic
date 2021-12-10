/*
 edition krycha88
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

#ifndef _button_analog_h
#define _button_analog_h

#include <Arduino.h>
#include <supla/control/button.h>
#include <supla/control/simple_button.h>
#include <supla/element.h>
#include <supla/events.h>
#include <supla/local_action.h>

namespace Supla {
namespace Control {

class ButtonAnalog : public Button {
 public:
  ButtonAnalog(int pin, int expected);

  void onTimer();
  int update();

 private:
  int pin;
  int expected;
  bool state;
};

};  // namespace Control
};  // namespace Supla

#endif
