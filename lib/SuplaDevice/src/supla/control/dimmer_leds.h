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

#ifndef __dimmer_leds_h
#define __dimmer_leds_h

#include "dimmer_base.h"

namespace Supla {
namespace Control {
class DimmerLeds : public DimmerBase {
 public:
  DimmerLeds(int brightnessPin);

  void setRGBWValueOnDevice(uint8_t red,
                            uint8_t green,
                            uint8_t blue,
                            uint8_t colorBrightness,
                            uint8_t brightness);

  void onInit();

 protected:
  int brightnessPin;
};

};  // namespace Control
};  // namespace Supla

#endif

