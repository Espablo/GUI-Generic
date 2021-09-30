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

#include "RFBridgeRelay.h"

namespace Supla {
namespace Control {
RFBridgeRelay::RFBridgeRelay(int transmitterPin,
                             int pin,
                             bool highIsOn,
                             _supla_int_t functions)
    : RFBridge(), Relay(pin, highIsOn, functions) {
  mySwitch->enableTransmit(transmitterPin);
}

void RFBridgeRelay::turnOn(_supla_int_t duration) {
  durationMs = duration;
  durationTimestamp = millis();
  if (keepTurnOnDurationMs) {
    durationMs = storedTurnOnDurationMs;
  }
  Supla::Io::digitalWrite(channel.getChannelNumber(), pin, pinOnValue());

  Serial.print("send code on: ");
  Serial.println(codeON);

  mySwitch->send(codeON, 24);

  channel.setNewValue(true);

  // Schedule save in 5 s after state change
  Supla::Storage::ScheduleSave(5000);
}

void RFBridgeRelay::turnOff(_supla_int_t duration) {
  durationMs = duration;
  durationTimestamp = millis();
  Supla::Io::digitalWrite(channel.getChannelNumber(), pin, pinOffValue());

  Serial.print("send code off: ");
  Serial.println(codeOFF);

  mySwitch->send(codeOFF, 24);

  channel.setNewValue(false);

  // Schedule save in 5 s after state change
  Supla::Storage::ScheduleSave(5000);
}

}  // namespace Control
}  // namespace Supla