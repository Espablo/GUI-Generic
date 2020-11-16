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

#ifndef _distance_h
#define _distance_h

#include "supla/channel.h"
#include "supla/element.h"

#define DISTANCE_NOT_AVAILABLE -1

namespace Supla {
namespace Sensor {
class Distance : public Element {
 public:
  Distance() : lastReadTime(0) {
    channel.setType(SUPLA_CHANNELTYPE_DISTANCESENSOR);
    channel.setDefault(SUPLA_CHANNELFNC_DISTANCESENSOR);
    channel.setNewValue(DISTANCE_NOT_AVAILABLE);
  }

  virtual double getValue() {
    return DISTANCE_NOT_AVAILABLE;
  }

  void iterateAlways() {
    if (lastReadTime + 500 < millis()) {
      lastReadTime = millis();
      channel.setNewValue(getValue());
    }
  }

 protected:
  Channel *getChannel() {
    return &channel;
  }
  Channel channel;
  unsigned long lastReadTime;
};

};  // namespace Sensor
};  // namespace Supla

#endif
