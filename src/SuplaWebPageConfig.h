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

#ifndef SuplaWebPageConfig_h
#define SuplaWebPageConfig_h

#include "SuplaDeviceGUI.h"

#ifdef SUPLA_CONFIG
#define PATH_CONFIG "config"

#define INPUT_CFG_LED_GPIO      "cfgl"
#define INPUT_CFG_BTN_GPIO      "cfgb"
#define INPUT_CFG_LED_LEVEL     "icll"
#define INPUT_CFG_MODE          "cfgm"
#define INPUT_CFG_SSL           "ics"
#define INPUT_CFG_AVAILABLE_GUI "icag"

#ifdef SUPLA_DEEP_SLEEP
#define INPUT_DEEP_SLEEP_TIME    "idst"
#endif

void createWebPageConfig();
void handleConfig(int save = 0);
void handleConfigSave();
#endif

#endif  // SuplaWebPageConfig_h
