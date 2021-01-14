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

#include <supla/storage/storage.h>

#include "roller_shutter.h"

namespace Supla {
namespace Control {

#pragma pack(push, 1)
struct RollerShutterStateData {
  uint32_t closingTimeMs;
  uint32_t openingTimeMs;
  int8_t currentPosition;  // 0 - closed; 100 - opened
};
#pragma pack(pop)

RollerShutter::RollerShutter(int pinUp, int pinDown, bool highIsOn)
    : closingTimeMs(0),
      openingTimeMs(0),
      calibrate(true),
      comfortDownValue(20),
      comfortUpValue(80),
      newTargetPositionAvailable(false),
      highIsOn(highIsOn),
      currentDirection(STOP_DIR),
      lastDirection(STOP_DIR),
      currentPosition(UNKNOWN_POSITION),
      targetPosition(STOP_POSITION),
      pinUp(pinUp),
      pinDown(pinDown),
      lastMovementStartTime(0),
      doNothingTime(0),
      calibrationTime(0),
      operationTimeout(0) {
  lastPositionBeforeMovement = currentPosition;
  channel.setType(SUPLA_CHANNELTYPE_RELAY);
  channel.setDefault(SUPLA_CHANNELFNC_CONTROLLINGTHEROLLERSHUTTER);
  channel.setFuncList(SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER);
}

void RollerShutter::onInit() {
  Supla::Io::digitalWrite(
      channel.getChannelNumber(), pinUp, highIsOn ? LOW : HIGH);
  Supla::Io::digitalWrite(
      channel.getChannelNumber(), pinDown, highIsOn ? LOW : HIGH);
  Supla::Io::pinMode(channel.getChannelNumber(), pinUp, OUTPUT);
  Supla::Io::pinMode(channel.getChannelNumber(), pinDown, OUTPUT);
}

/*
 * Protocol:
 * value[0]:
 *  0 - stop
 *  1 - down
 *  2 - up
 *  10 - 110 - 0% - 100%
 *
 * time is send in 0.1 s. i.e. 105 -> 10.5 s
 * time * 100 = gives time in ms
 *
 */

int RollerShutter::handleNewValueFromServer(
    TSD_SuplaChannelNewValue *newValue) {
  uint32_t newClosingTime = (newValue->DurationMS & 0xFFFF) * 100;
  uint32_t newOpeningTime = ((newValue->DurationMS >> 16) & 0xFFFF) * 100;

  setOpenCloseTime(newClosingTime, newOpeningTime);

  char task = newValue->value[0];
  Serial.print(F("RollerShutter["));
  Serial.print(channel.getChannelNumber());
  Serial.print(F("] new value from server: "));
  if (task == 0) {
    Serial.println(F("STOP"));
    stop();
  } else if (task == 1) {
    Serial.println(F("MOVE_DOWN"));
    moveDown();
  } else if (task == 2) {
    Serial.println(F("MOVE_UP"));
    moveUp();
  } else if (task >= 10 && task <= 110) {
    setTargetPosition(task - 10);
    Serial.println(static_cast<int>(task - 10));
  }

  return -1;
}

void RollerShutter::setOpenCloseTime(uint32_t newClosingTimeMs,
                                     uint32_t newOpeningTimeMs) {
  if (newClosingTimeMs == 0) {
    newClosingTimeMs = closingTimeMs;
  }
  if (newOpeningTimeMs == 0) {
    newOpeningTimeMs = openingTimeMs;
  }

  if (newClosingTimeMs != closingTimeMs || newOpeningTimeMs != openingTimeMs) {
    closingTimeMs = newClosingTimeMs;
    openingTimeMs = newOpeningTimeMs;
    calibrate = true;
    currentPosition = UNKNOWN_POSITION;
    Serial.print(F("RollerShutter["));
    Serial.print(channel.getChannelNumber());
    Serial.print(F("] new time settings received. Opening time: "));
    Serial.print(openingTimeMs);
    Serial.print(F(" ms; closing time: "));
    Serial.print(closingTimeMs);
    Serial.println(F(" ms. Starting calibration..."));
  }
}

void RollerShutter::runAction(int event, int action) {
  (void)(event);
  switch (action) {
    case CLOSE_OR_STOP: {
      if (inMove()) {
        stop();
      } else {
        close();
      }
      break;
    }

    case CLOSE: {
      close();
      break;
    }
    case OPEN_OR_STOP: {
      if (inMove()) {
        stop();
      } else {
        open();
      }
      break;
    }

    case OPEN: {
      open();
      break;
    }

    case COMFORT_DOWN_POSITION: {
      setTargetPosition(comfortDownValue);
      break;
    }

    case COMFORT_UP_POSITION: {
      setTargetPosition(comfortUpValue);
      break;
    }

    case STOP: {
      stop();
      break;
    }

    case STEP_BY_STEP: {
      if (inMove()) {
        stop();
      } else if (lastDirectionWasOpen()) {
        close();
      } else if (lastDirectionWasClose()) {
        open();
      } else if (currentPosition < 50) {
        close();
      } else {
        open();
      }
      break;
    }

    case MOVE_UP: {
      moveUp();
      break;
    }

    case MOVE_DOWN: {
      moveDown();
      break;
    }

    case MOVE_UP_OR_STOP: {
      if (inMove()) {
        stop();
      } else {
        moveUp();
      }
      break;
    }
    case MOVE_DOWN_OR_STOP: {
      if (inMove()) {
        stop();
      } else {
        moveDown();
      }
      break;
    }
  }
}

void RollerShutter::close() {
  setTargetPosition(100);
}

void RollerShutter::open() {
  setTargetPosition(0);
}

void RollerShutter::moveDown() {
  setTargetPosition(MOVE_DOWN_POSITION);
}

void RollerShutter::moveUp() {
  setTargetPosition(MOVE_UP_POSITION);
}

void RollerShutter::stop() {
  setTargetPosition(STOP_POSITION);
}

void RollerShutter::setTargetPosition(int newPosition) {
  targetPosition = newPosition;
  newTargetPositionAvailable = true;

  // Negative targetPosition is either unknown or stop command, so we
  // ignore it
  if (targetPosition == MOVE_UP_POSITION) {
    lastDirection = UP_DIR;
  } else if (targetPosition == MOVE_DOWN_POSITION) {
    lastDirection = DOWN_DIR;
  } else if (targetPosition >= 0) {
    if (targetPosition < currentPosition) {
      lastDirection = UP_DIR;
    } else if (targetPosition > currentPosition) {
      lastDirection = DOWN_DIR;
    }
  }
}

bool RollerShutter::lastDirectionWasOpen() {
  return lastDirection == UP_DIR;
}

bool RollerShutter::lastDirectionWasClose() {
  return lastDirection == DOWN_DIR;
}

bool RollerShutter::inMove() {
  return currentDirection != STOP_DIR;
}

void RollerShutter::stopMovement() {
  switchOffRelays();
  currentDirection = STOP_DIR;
  doNothingTime = millis();
  // Schedule save in 5 s after stop movement of roller shutter
  Supla::Storage::ScheduleSave(5000);
}

void RollerShutter::relayDownOn() {
  Supla::Io::digitalWrite(
      channel.getChannelNumber(), pinDown, highIsOn ? HIGH : LOW);
}

void RollerShutter::relayUpOn() {
  Supla::Io::digitalWrite(
      channel.getChannelNumber(), pinUp, highIsOn ? HIGH : LOW);
}

void RollerShutter::relayDownOff() {
  Supla::Io::digitalWrite(
      channel.getChannelNumber(), pinDown, highIsOn ? LOW : HIGH);
}

void RollerShutter::relayUpOff() {
  Supla::Io::digitalWrite(
      channel.getChannelNumber(), pinUp, highIsOn ? LOW : HIGH);
}

void RollerShutter::startClosing() {
  currentDirection = DOWN_DIR;
  relayUpOff();  // just to make sure
  relayDownOn();
}

void RollerShutter::startOpening() {
  currentDirection = UP_DIR;
  relayDownOff();  // just to make sure
  relayUpOn();
}

void RollerShutter::switchOffRelays() {
  relayUpOff();
  relayDownOff();
}

void RollerShutter::onTimer() {
  if (millis() - doNothingTime <
      300) {  // doNothingTime time is used when we change
              // direction of roller - to stop for a moment
              // before enabling opposite direction
    return;
  }

  if (operationTimeout != 0 &&
      millis() - lastMovementStartTime > operationTimeout) {
    setTargetPosition(STOP_POSITION);
    operationTimeout = 0;
  }

  if (targetPosition == STOP_POSITION && inMove()) {
    stopMovement();
    calibrationTime = 0;
  }

  if (calibrate && targetPosition == STOP_POSITION) {
    return;
  } else if (calibrate) {
    // If calibrationTime is not set, then it means we should start calibration
    if (calibrationTime == 0) {
      // If roller shutter wasn't in move when calibration is requested, we
      // select direction based on requested targetPosition
      operationTimeout = 0;
      if (targetPosition > 50 || targetPosition == MOVE_DOWN_POSITION) {
        if (currentDirection == UP_DIR) {
          stopMovement();
        } else if (currentDirection == STOP_DIR) {
          Serial.println(F("Calibration: closing"));
          calibrationTime = closingTimeMs;
          lastMovementStartTime = millis();
          if (calibrationTime == 0) {
            operationTimeout = 60000;
          }
          startClosing();
        }
      } else {
        if (currentDirection == DOWN_DIR) {
          stopMovement();
        } else if (currentDirection == STOP_DIR) {
          Serial.println(F("Calibration: opening"));
          calibrationTime = openingTimeMs;
          lastMovementStartTime = millis();
          if (calibrationTime == 0) {
            operationTimeout = 60000;
          }
          startOpening();
        }
      }
      //
      // Time used for calibaration is 10% higher then requested by user
      calibrationTime *= 1.1;
      if (calibrationTime > 0) {
        Serial.print(F("Calibration time: "));
        Serial.println(calibrationTime);
      }
    }

    if (calibrationTime != 0 &&
        millis() - lastMovementStartTime > calibrationTime) {
      Serial.println(F("Calibration done"));
      calibrationTime = 0;
      calibrate = false;
      if (currentDirection == UP_DIR) {
        currentPosition = 0;
      } else {
        currentPosition = 100;
      }
      stopMovement();
    }

  } else if (!newTargetPositionAvailable &&
             currentDirection !=
                 STOP_DIR) {  // no new command available and it is moving,
                              // just handle roller movement/status
    if (currentDirection == UP_DIR && currentPosition > 0) {
      int movementDistance = lastPositionBeforeMovement;
      int timeRequired = (1.0 * openingTimeMs * movementDistance / 100.0);
      float fractionOfMovemendDone =
          (1.0 * (millis() - lastMovementStartTime) / timeRequired);
      if (fractionOfMovemendDone > 1) {
        fractionOfMovemendDone = 1;
      }
      currentPosition = lastPositionBeforeMovement -
                        movementDistance * fractionOfMovemendDone;
      if (targetPosition >= 0 && currentPosition <= targetPosition) {
        stopMovement();
      }
    } else if (currentDirection == DOWN_DIR && currentPosition < 100) {
      int movementDistance = 100 - lastPositionBeforeMovement;
      int timeRequired = (1.0 * closingTimeMs * movementDistance / 100.0);
      float fractionOfMovemendDone =
          (1.0 * (millis() - lastMovementStartTime) / timeRequired);
      if (fractionOfMovemendDone > 1) {
        fractionOfMovemendDone = 1;
      }
      currentPosition = lastPositionBeforeMovement +
                        movementDistance * fractionOfMovemendDone;
      if (targetPosition >= 0 && currentPosition >= targetPosition) {
        stopMovement();
      }
    }

    if (currentPosition > 100) {
      currentPosition = 100;
    } else if (currentPosition < 0) {
      currentPosition = 0;
    }

  } else if (newTargetPositionAvailable && targetPosition != STOP_POSITION) {
    // new target state was set, let's handle it
    int newDirection = STOP_DIR;
    if (targetPosition == MOVE_UP_POSITION) {
      newDirection = UP_DIR;
      operationTimeout = 60000;
    } else if (targetPosition == MOVE_DOWN_POSITION) {
      newDirection = DOWN_DIR;
      operationTimeout = 60000;
    } else {
      operationTimeout = 0;
      int newMovementValue = targetPosition - currentPosition;
      // 0 - 100 = -100 (move down); 50 -
      // 20 = 30 (move up 30%), etc
      if (newMovementValue > 0) {
        newDirection = DOWN_DIR;  // move down
      } else if (newMovementValue < 0) {
        newDirection = UP_DIR;  // move up
      }
    }
    // If new direction is the same as current move, then keep movin`
    if (newDirection == currentDirection) {
      newTargetPositionAvailable = false;
    } else if (currentDirection == STOP_DIR) {  // else start moving
      newTargetPositionAvailable = false;
      lastPositionBeforeMovement = currentPosition;
      lastMovementStartTime = millis();
      if (newDirection == DOWN_DIR) {
        startClosing();
      } else {
        startOpening();
      }
    } else {  // else stop before changing direction
      stopMovement();
    }
  }
  // if (newCurrentPosition != currentPosition) {
  // currentPosition = newCurrentPosition;
  channel.setNewValue(static_cast<_supla_int_t>(
      currentPosition));  // value set on channel will be send to server
                          // during iterateConnected() execution
                          // }
}

Channel *RollerShutter::getChannel() {
  return &channel;
}

void RollerShutter::configComfortUpValue(uint8_t position) {
  comfortUpValue = position;
  if (comfortUpValue > 100) {
    comfortUpValue = 100;
  }
}

void RollerShutter::configComfortDownValue(uint8_t position) {
  comfortDownValue = position;
  if (comfortDownValue > 100) {
    comfortDownValue = 100;
  }
}

void RollerShutter::onLoadState() {
  RollerShutterStateData data = RollerShutterStateData();
  if (Supla::Storage::ReadState((unsigned char *)&data, sizeof(data))) {
    closingTimeMs = data.closingTimeMs;
    openingTimeMs = data.openingTimeMs;
    currentPosition = data.currentPosition;
    if (currentPosition >= 0) {
      calibrate = false;
    }
    Serial.print(F("RollerShutter["));
    Serial.print(channel.getChannelNumber());
    Serial.print(F("] settings restored from storage. Opening time: "));
    Serial.print(openingTimeMs);
    Serial.print(F(" ms; closing time: "));
    Serial.print(closingTimeMs);
    Serial.print(F(" ms. Position: "));
    Serial.println(currentPosition);
  }
}

void RollerShutter::onSaveState() {
  RollerShutterStateData data;
  data.closingTimeMs = closingTimeMs;
  data.openingTimeMs = openingTimeMs;
  data.currentPosition = currentPosition;

  Supla::Storage::WriteState((unsigned char *)&data, sizeof(data));
}

int RollerShutter::getCurrentPosition() {
  return currentPosition;
}

};  // namespace Control
};  // namespace Supla
