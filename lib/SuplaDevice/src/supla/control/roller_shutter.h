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

#ifndef _roller_shutter_h
#define _roller_shutter_h

#include <Arduino.h>

#include "../io.h"
#include "../channel.h"
#include "../element.h"
#include "../triggerable.h"
#include "../actions.h"

#define UNKNOWN_POSITION    -1
#define STOP_POSITION       -2
#define MOVE_UP_POSITION    -3
#define MOVE_DOWN_POSITION  -4

namespace Supla {
namespace Control {

enum Directions { STOP_DIR, DOWN_DIR, UP_DIR };

class RollerShutter : public Element, public Triggerable {
 public:
  RollerShutter(int pinUp, int pinDown, bool highIsOn = true);

  int handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue);
  void runAction(int trigger, int action);

  void close(); // Sets target position to 100%
  void open();  // Sets target position to 0%
  void stop();  // Stop motor
  void moveUp();   // start opening roller shutter regardless of its position (keep motor going up)
  void moveDown(); // starts closing roller shutter regardless of its position (keep motor going down)
  void setTargetPosition(int newPosition);

  void configComfortUpValue(uint8_t position);
  void configComfortDownValue(uint8_t position);


  void onInit();
  void onTimer();
  void onLoadState();
  void onSaveState();

 protected:
  virtual void stopMovement();
  virtual void relayDownOn();
  virtual void relayUpOn();
  virtual void relayDownOff();
  virtual void relayUpOff();
  virtual void startClosing();
  virtual void startOpening();
  virtual void switchOffRelays();
  void setOpenCloseTime(uint32_t newClosingTimeMs, uint32_t newOpeningTimeMs);

  bool lastDirectionWasOpen();
  bool lastDirectionWasClose();
  bool inMove();

  Channel *getChannel();

  Channel channel;

  uint32_t closingTimeMs;
  uint32_t openingTimeMs;
  bool calibrate; // set to true when new closing/opening time is given - calibration is done to sync roller shutter position

  uint8_t comfortDownValue;
  uint8_t comfortUpValue;

  bool newTargetPositionAvailable;

  bool highIsOn;

  uint8_t currentDirection; // stop, up, down 
  uint8_t lastDirection;

  int8_t currentPosition; // 0 - closed; 100 - opened
  int8_t targetPosition; // 0-100 
  int8_t lastPositionBeforeMovement; // 0-100

  int pinUp;
  int pinDown;

  unsigned long lastMovementStartTime;
  unsigned long doNothingTime;
  unsigned long calibrationTime;
  unsigned long operationTimeout;
};

};  // namespace Control
};  // namespace Supla

#endif


