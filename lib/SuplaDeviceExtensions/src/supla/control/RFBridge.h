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

#ifndef _RF_Bridge_h
#define _RF_Bridge_h

#include <Arduino.h>
#include <RCSwitch.h>
#include <supla/channel_element.h>

#define TIME_REPEAT_SENDING_SEK 600  // 10mim

namespace Supla {
namespace Control {

class RFBridge : public Element {
 public:
  RFBridge();

  void setPulseLengthint(int pulseLength);
  void setRepeatTransmit(int repeatTransmit);
  void setRepeatProtocol(int protocol);

  void setCodeLength(int length);
  void setCodeON(int code);
  void setCodeOFF(int code);

  void sendCodeON();
  void sendCodeOFF();

  void setRepeatSending(bool value);

  void onTimer();

 protected:
  RCSwitch *mySwitch;

  bool stateCode;
  unsigned long repeatMs;
  bool repeatSending;

  int codeON;
  int codeOFF;
  uint8_t lengthCode;
};

};  // namespace Control
};  // namespace Supla
#endif