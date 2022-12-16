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
#include "deepSleep.h"
#include "../../SuplaDeviceGUI.h"

namespace Supla {
namespace Control {

DeepSleep::DeepSleep(unsigned _supla_int_t sleepTimeSec, unsigned _supla_int_t iterateTimeSec)
    : sleepTimeSec(sleepTimeSec), iterateTimeSec(iterateTimeSec), lastReadTime(0) {
}

void DeepSleep::iterateAlways() {
  if (millis() - lastReadTime > 5000 && ConfigESP->configModeESP == Supla::DEVICE_MODE_NORMAL &&
      !Supla::Protocol::ProtocolLayer::IsAnyUpdatePending()) {
    Serial.println(F("ESP to Sleep mode"));
    lastReadTime = millis();

    delay(200);
    ESP.deepSleep(sleepTimeSec * 1000000);
  }
}

void DeepSleep::onInit() {
  SuplaDevice.addFlags(SUPLA_DEVICE_FLAG_SLEEP_MODE_ENABLED);
  SuplaDevice.setActivityTimeout(sleepTimeSec * 2);

  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();
      channel->setValidityTimeSec(sleepTimeSec * 2);
    }

    if (element->getSecondaryChannel()) {
      auto channel = element->getSecondaryChannel();
      channel->setValidityTimeSec(sleepTimeSec * 2);
    }
  }
}

}  // namespace Control
}  // namespace Supla
