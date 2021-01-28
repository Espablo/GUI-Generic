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

#ifndef _button_h
#define _button_h

#include <Arduino.h>
#include "simple_button.h"

namespace Supla {
namespace Control {

class Button : public SimpleButton {
 public:
  Button(int pin, bool pullUp = false, bool invertLogic = false);

  void onTimer();
  void setHoldTime(unsigned int timeMs);
  void setMulticlickTime(unsigned int timeMs, bool bistableButton = false);

 protected:
  unsigned int holdTimeMs;
  unsigned int multiclickTimeMs;
  unsigned long lastStateChangeMs;
  uint8_t clickCounter;
  bool holdSend;
  bool bistable;
};

};  // namespace Control
};  // namespace Supla

#endif
