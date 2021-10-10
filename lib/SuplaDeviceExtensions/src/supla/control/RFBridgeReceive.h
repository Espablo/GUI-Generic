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

#ifndef _RF_Bridge_Receive_h
#define _RF_Bridge_Receive_h

#include <Arduino.h>
#include <RCSwitch.h>

#include <supla/action_handler.h>
#include <supla/actions.h>
#include <supla/channel_element.h>
#include <supla/control/RFBridge.h>
#include <supla/local_action.h>


namespace Supla {
namespace Control {

class RFBridgeReceive : public RFBridge, public Element, public LocalAction{
 public:
  RFBridgeReceive(int receivePin);
  void onTimer();
};

};  // namespace Control
};  // namespace Supla
#endif