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

#include "RFBridge.h"

namespace Supla {
namespace Control {
RFBridge::RFBridge() {
  mySwitch = new RCSwitch();
}

void RFBridge::setPulseLengthint(int pulseLength) {
  // Optional set pulse length.
  mySwitch->setPulseLength(pulseLength);
}

void RFBridge::setRepeatTransmit(int repeatTransmit) {
  // Optional set number of transmission repetitions.
  mySwitch->setRepeatTransmit(repeatTransmit);
}

void RFBridge::setRepeatProtocol(int protocol) {
  // Optional set protocol (default is 1, will work for most outlets)
  mySwitch->setProtocol(protocol);
}

void RFBridge::setCodeON(int code) {
  codeON = code;
}

void RFBridge::setCodeOFF(int code) {
  codeOFF = code;
}

}  // namespace Control
}  // namespace Supla