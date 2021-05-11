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

#ifndef _deepSleep_h
#define _deepSleep_h

#include <Arduino.h>
#include <supla/channel_element.h>

namespace Supla {
namespace Control {

class DeepSleep : public Element {
 public:
  DeepSleep(unsigned _supla_int_t sleepTimeSec,
            unsigned _supla_int_t iterateTimeSec)
      : sleepTimeSec(sleepTimeSec),
        iterateTimeSec(iterateTimeSec),
        lastReadTime(0) {
  }

  void iterateAlways() {
    if (millis() - lastReadTime > iterateTimeSec * 1000 &&
        ConfigESP->configModeESP == NORMAL_MODE) {
      Serial.println(F("ESP8266 to Sleep mode"));
      lastReadTime = millis();
      ESP.deepSleep(sleepTimeSec * 1000000);
      delay(100);
    }
  }

  void onInit() {
    for (auto element = Supla::Element::begin(); element != nullptr;
         element = element->next()) {
      if (element->getChannel()) {
        auto channel = element->getChannel();
        channel->setValidityTimeSec(sleepTimeSec);
      }

      if (element->getSecondaryChannel()) {
        auto channel = element->getSecondaryChannel();
        channel->setValidityTimeSec(sleepTimeSec);
      }
    }
  }

 protected:
  unsigned long lastReadTime;
  unsigned _supla_int_t sleepTimeSec;    // sleep seconds
  unsigned _supla_int_t iterateTimeSec;  // sleep seconds
};

};  // namespace Control
};  // namespace Supla

#endif
