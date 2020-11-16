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

#ifndef SuplaDeviceGUI_h
#define SuplaDeviceGUI_h

#include "GUI-Generic_Config.h"

#include <SuplaDevice.h>

#include <supla/control/button.h>
#include <supla/control/relay.h>
#include <supla/control/roller_shutter.h>
#include "SuplaSensorDS18B20.h"

#ifdef DEBUG_MODE
#include <supla/sensor/esp_free_heap.h>
#endif

#include "SuplaConfigESP.h"
#include "SuplaConfigManager.h"
#include "SuplaWebServer.h"
#include "SuplaWebPageRelay.h"

#include <vector>

enum ConfigMode { CONFIG_MODE_10_ON_PRESSES, CONFIG_MODE_5SEK_HOLD };

namespace Supla {
namespace GUI {

void begin();

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
void addRelayButton(int pinRelay, int pinButton, bool highIsOn = true);

extern std::vector<Supla::Control::Relay *> relay;
extern std::vector<Supla::Control::Button *> button;
#endif

#ifdef SUPLA_DS18B20
void addDS18B20MultiThermometer(int pinNumber);

extern std::vector<DS18B20 *> sensorDS;
#endif

#ifdef SUPLA_CONFIG
void addConfigESP(int pinNumberConfig, int pinLedConfig, int modeConfigButton, bool highIsOn);
#endif

#ifdef SUPLA_ROLLERSHUTTER
void addRolleShutter(
    int pinRelayUp, int pinRelayDown, int pinButtonUp, int pinButtonDown, bool highIsOn = true);
void addRolleShutterMomentary(
    int pinRelayUp, int pinRelayDown, int pinButtonUp, int pinButtonDown, bool highIsOn = true);

extern std::vector<Supla::Control::RollerShutter *> RollerShutterRelay;
extern std::vector<Supla::Control::Button *> RollerShutterButtonOpen;
extern std::vector<Supla::Control::Button *> RollerShutterButtonClose;
#endif
};
};

extern SuplaConfigManager *ConfigManager;
extern SuplaConfigESP *ConfigESP;
extern SuplaWebServer *WebServer;

#endif  // SuplaDeviceGUI_h
