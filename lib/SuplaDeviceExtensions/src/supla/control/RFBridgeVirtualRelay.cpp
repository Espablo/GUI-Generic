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

#include "RFBridgeVirtualRelay.h"

namespace Supla {
namespace Control {
RFBridgeVirtualRelay::RFBridgeVirtualRelay(int transmitterPin,
                                           _supla_int_t functions)
    : RFBridge(), VirtualRelay(functions) {
  mySwitch->enableTransmit(transmitterPin);
}

void RFBridgeVirtualRelay::turnOn(_supla_int_t duration) {
  durationMs = duration;
  durationTimestamp = millis();
  if (keepTurnOnDurationMs) {
    durationMs = storedTurnOnDurationMs;
  }
  state = true;

  channel.setNewValue(state);

  Serial.println("send code on");
  delay(0);
  mySwitch->send(codeON, lengthCode);
  // Schedule save in 5 s after state change
  Supla::Storage::ScheduleSave(5000);
}

void RFBridgeVirtualRelay::turnOff(_supla_int_t duration) {
  durationMs = duration;
  durationTimestamp = millis();
  state = false;

  channel.setNewValue(state);

  Serial.println("send code off");
  delay(0);
  mySwitch->send(codeOFF, lengthCode);

  // Schedule save in 5 s after state change
  Supla::Storage::ScheduleSave(5000);
}

}  // namespace Control
}  // namespace Supla