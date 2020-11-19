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

#include "light_relay.h"

using namespace Supla;
using namespace Control;

#pragma pack(push, 1)
struct LightRelayStateData {
  unsigned short lifespan;
  _supla_int_t turnOnSecondsCumulative;
};
#pragma pack(pop)

LightRelay::LightRelay(int pin, bool highIsOn)
    : Relay(pin,
            highIsOn,
            SUPLA_BIT_FUNC_LIGHTSWITCH | SUPLA_BIT_FUNC_STAIRCASETIMER),
      lifespan(10000),
      turnOnSecondsCumulative(0) {
  channel.setFlag(SUPLA_CHANNEL_FLAG_LIGHTSOURCELIFESPAN_SETTABLE);
}

void LightRelay::handleGetChannelState(TDSC_ChannelState &channelState) {
  channelState.Fields |= SUPLA_CHANNELSTATE_FIELD_LIGHTSOURCELIFESPAN |
                         SUPLA_CHANNELSTATE_FIELD_LIGHTSOURCEOPERATINGTIME;
  channelState.LightSourceLifespan = lifespan;
  channelState.LightSourceOperatingTime = turnOnSecondsCumulative;
}

int LightRelay::handleCalcfgFromServer(TSD_DeviceCalCfgRequest *request) {
  if (request &&
      request->Command == SUPLA_CALCFG_CMD_SET_LIGHTSOURCE_LIFESPAN) {
    if (request->DataSize == sizeof(TCalCfg_LightSourceLifespan)) {
      TCalCfg_LightSourceLifespan *config =
          reinterpret_cast<TCalCfg_LightSourceLifespan *>(request->Data);

      if (config->SetTime) {
        lifespan = config->LightSourceLifespan;
      }
      if (config->ResetCounter) {
        turnOnSecondsCumulative = 0;
      }

      return SUPLA_CALCFG_RESULT_DONE;
    }
  }

  return SUPLA_CALCFG_RESULT_NOT_SUPPORTED;
}

void LightRelay::onLoadState() {
  LightRelayStateData data;
  if (Supla::Storage::ReadState((unsigned char *)&data, sizeof(data))) {
    lifespan = data.lifespan;
    turnOnSecondsCumulative = data.turnOnSecondsCumulative;

    Serial.print(F("LightRelay["));
    Serial.print(channel.getChannelNumber());
    Serial.print(F("] settings restored from storage. Total lifespan: "));
    Serial.print(lifespan);
    Serial.print(F(" h; current operating time: "));
    Serial.print(turnOnSecondsCumulative);
    Serial.println(F(" s"));
  }
  Relay::onLoadState();
}

void LightRelay::onSaveState() {
  LightRelayStateData data;

  data.lifespan = lifespan;
  data.turnOnSecondsCumulative = turnOnSecondsCumulative;
  Supla::Storage::WriteState((unsigned char *)&data, sizeof(data));
  Relay::onSaveState();
}

void LightRelay::turnOn(_supla_int_t duration) {
  turnOnTimestamp = millis();
  Relay::turnOn(duration);
}

void LightRelay::iterateAlways() {
  if (isOn()) {
    unsigned long currentMillis = millis();
    int seconds = (currentMillis - turnOnTimestamp) / 1000;
    if (seconds > 0) {
      turnOnTimestamp =
          currentMillis - ((currentMillis - turnOnTimestamp) % 1000);
      turnOnSecondsCumulative += seconds;
    }
  }

  Relay::iterateAlways();
}
