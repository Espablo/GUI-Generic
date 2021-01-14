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

#ifndef _element_h
#define _element_h

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
#include <cstddef>
#endif
#include "channel.h"

namespace Supla {

class Element {
 public:
  Element();
  static Element *begin();
  static Element *last();
  static Element *getElementByChannelNumber(int channelNumber);
  Element *next();

  // method called during SuplaDevice initialization. I.e. load initial state,
  // initialize pins etc.
  virtual void onInit();

  // method called during Config initialization (i.e. read from EEPROM, FRAM).
  // Called only if Storage class is configured
  virtual void onLoadState();

  // method called during periodically during SuplaDevice iteration
  // Called only if Storage class is configured
  virtual void onSaveState();

  // method called on each SuplaDevice iteration (before Network layer
  // iteration). When Device is connected, both iterateAlways() and
  // iterateConnected() are called.
  virtual void iterateAlways();

  // method called on each Supla::Device iteration when Device is connected and
  // registered to Supla server
  virtual bool iterateConnected(void *srpc);

  // method called on timer interupt
  // Include all actions that have to be executed periodically regardless of
  // other SuplaDevice activities
  virtual void onTimer();

  // method called on fast timer interupt
  // Include all actions that have to be executed periodically regardless of
  // other SuplaDevice activities
  virtual void onFastTimer();

  // return value:
  //  -1 - don't send reply to server
  //  0 - success==false
  //  1 - success==true
  virtual int handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue);

  // Handles "get channel state" request from server
  // channelState is prefilled with network and device status informations
  virtual void handleGetChannelState(TDSC_ChannelState &channelState);

  virtual int handleCalcfgFromServer(TSD_DeviceCalCfgRequest *request);

  int getChannelNumber();
  virtual Channel *getChannel();
  virtual Channel *getSecondaryChannel();

  Element &disableChannelState();

 protected:
  static Element *firstPtr;
  Element *nextPtr;
};

};  // namespace Supla

#endif
