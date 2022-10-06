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

#include "RFBridgeReceive.h"

namespace Supla {
namespace Control {
RFBridgeReceive::RFBridgeReceive(int receivePin) : RFBridge(), Button(99) {
  mySwitch->enableReceive(receivePin);
}

int RFBridgeReceive::update() {
  unsigned long curMillis = millis();

  if (mySwitch->available()) {
    int code = mySwitch->getReceivedValue();

    if (code == codeON) {
      Serial.print("Received code ON ");
      Serial.println(code);
      currentState = HIGH;

      if (monostable) {
        durationTimestamp = curMillis;
      } else {
        runAction(ON_PRESS);
      }

      mySwitch->resetAvailable();
    } else if (code == codeOFF) {
      Serial.print("Received code OFF ");
      Serial.println(code);
      currentState = LOW;

      if (monostable) {
        durationTimestamp = curMillis;
      } else {
        runAction(ON_RELEASE);
      }

      mySwitch->resetAvailable();
    }
  }

  if (monostable && curMillis - durationTimestamp > debounceDelayMs) {
    if (codeON)
      currentState = LOW;
    else
      currentState = HIGH;
  }

  if (debounceDelayMs == 0 || curMillis - debounceTimeMs > debounceDelayMs) {
    if (currentState != prevState) {
      // If status is changed, then make sure that it will be kept at
      // least swNoiseFilterDelayMs ms to avoid noise
      if (swNoiseFilterDelayMs != 0 && currentState != newStatusCandidate) {
        newStatusCandidate = currentState;
        filterTimeMs = curMillis;
      } else if (curMillis - filterTimeMs > swNoiseFilterDelayMs) {
        // If new status is kept at least swNoiseFilterDelayMs ms, then apply
        // change of status
        debounceTimeMs = curMillis;
        prevState = currentState;

        if (currentState) {
          return TO_PRESSED;
        } else {
          return TO_RELEASED;
        }
      }
    } else {
      // If current status is the same as prevState, then reset
      // new status candidate
      newStatusCandidate = prevState;
    }
  }
  if (prevState) {
    return PRESSED;
  } else {
    return RELEASED;
  }
}

void RFBridgeReceive::onTimer() {
  unsigned int timeDelta = millis() - lastStateChangeMs;
  bool stateChanged = false;
  int stateResult = update();

  if (stateResult == TO_PRESSED) {
    stateChanged = true;
    runAction(ON_PRESS);
    runAction(ON_CHANGE);
  } else if (stateResult == TO_RELEASED) {
    stateChanged = true;
    runAction(ON_RELEASE);
    runAction(ON_CHANGE);
  }

  if (stateChanged) {
    lastStateChangeMs = millis();
    if (stateResult == TO_PRESSED || bistable) {
      clickCounter++;
    }
  }

  if (!stateChanged) {
    if (!bistable && stateResult == PRESSED) {
      if (clickCounter <= 1 && holdTimeMs > 0 &&
          timeDelta > (holdTimeMs + holdSend * repeatOnHoldMs) &&
          (repeatOnHoldMs == 0 ? !holdSend : true)) {
        runAction(ON_HOLD);
        ++holdSend;
      }
    } else if ((bistable || stateResult == RELEASED)) {
      if (multiclickTimeMs == 0) {
        holdSend = 0;
        clickCounter = 0;
      }
      if (multiclickTimeMs > 0 && timeDelta > multiclickTimeMs) {
        if (holdSend == 0) {
          switch (clickCounter) {
            case 1:
              runAction(ON_CLICK_1);
              break;
            case 2:
              runAction(ON_CLICK_2);
              break;
            case 3:
              runAction(ON_CLICK_3);
              break;
            case 4:
              runAction(ON_CLICK_4);
              break;
            case 5:
              runAction(ON_CLICK_5);
              break;
            case 6:
              runAction(ON_CLICK_6);
              break;
            case 7:
              runAction(ON_CLICK_7);
              break;
            case 8:
              runAction(ON_CLICK_8);
              break;
            case 9:
              runAction(ON_CLICK_9);
              break;
            case 10:
              runAction(ON_CLICK_10);
              break;
          }
          if (clickCounter >= 10) {
            runAction(ON_CRAZY_CLICKER);
          }
        } else {
          switch (clickCounter) {
            // for LONG_CLICK counter was incremented once by ON_HOLD
            case 1:
              runAction(ON_LONG_CLICK_0);
              break;
            case 2:
              runAction(ON_LONG_CLICK_1);
              break;
            case 3:
              runAction(ON_LONG_CLICK_2);
              break;
            case 4:
              runAction(ON_LONG_CLICK_3);
              break;
            case 5:
              runAction(ON_LONG_CLICK_4);
              break;
            case 6:
              runAction(ON_LONG_CLICK_5);
              break;
            case 7:
              runAction(ON_LONG_CLICK_6);
              break;
            case 8:
              runAction(ON_LONG_CLICK_7);
              break;
            case 9:
              runAction(ON_LONG_CLICK_8);
              break;
            case 10:
              runAction(ON_LONG_CLICK_9);
              break;
            case 11:
              runAction(ON_LONG_CLICK_10);
              break;
          }
        }
        holdSend = 0;
        clickCounter = 0;
      }
    }
  }
}

void RFBridgeReceive::isMonostable() {
  monostable = true;
}

}  // namespace Control
}  // namespace Supla