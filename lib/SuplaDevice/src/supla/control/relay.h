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

/* Relay class
 * This class is used to control any type of relay that can be controlled
 * by setting LOW or HIGH output on selected GPIO.
 */

#ifndef _relay_h
#define _relay_h

#include <Arduino.h>

#include "../actions.h"
#include "../channel.h"
#include "../element.h"
#include "../io.h"
#include "../storage/storage.h"
#include "../triggerable.h"

#define STATE_ON_INIT_RESTORED_OFF -3
#define STATE_ON_INIT_RESTORED_ON -2
#define STATE_ON_INIT_RESTORE -1
#define STATE_ON_INIT_OFF 0
#define STATE_ON_INIT_ON 1

namespace Supla {
namespace Control {
class Relay : public Element, public Triggerable {
 public:
  Relay(int pin,
        bool highIsOn = true,
        _supla_int_t functions = (0xFF ^
                                  SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER));

  virtual Relay &setDefaultStateOn();
  virtual Relay &setDefaultStateOff();
  virtual Relay &setDefaultStateRestore();
  virtual Relay &keepTurnOnDuration(bool keep = true);

  virtual uint8_t pinOnValue();
  virtual uint8_t pinOffValue();
  virtual void turnOn(_supla_int_t duration = 0);
  virtual void turnOff(_supla_int_t duration = 0);
  virtual bool isOn();
  virtual void toggle(_supla_int_t duration = 0);

  void runAction(int event, int action);

  void onInit();
  void onLoadState();
  void onSaveState();
  void iterateAlways();
  int handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue);

 protected:
  Channel *getChannel();
  Channel channel;
  int pin;
  bool highIsOn;

  int8_t stateOnInit;

  unsigned _supla_int_t durationMs;
  unsigned _supla_int_t storedTurnOnDurationMs;
  unsigned long durationTimestamp;
  bool keepTurnOnDurationMs;

};

};  // namespace Control
};  // namespace Supla

#endif
