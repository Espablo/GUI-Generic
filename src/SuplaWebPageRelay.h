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

#ifndef SuplaWebPageRelay_h
#define SuplaWebPageRelay_h

#include "SuplaDeviceGUI.h"

#ifdef SUPLA_PUSHOVER
#define INPUT_PUSHOVER_MESSAGE "pm"
#endif

#if defined(SUPLA_RELAY) || defined(GUI_SENSOR_I2C_EXPENDER)
#define GUI_RELAY
#endif

#define PATH_RELAY_SET        "setrelay"

#ifdef GUI_RELAY
#define PATH_RELAY     "relay"
#define PATH_RELAY_SET "setrelay"

#define INPUT_MAX_RELAY      "mrl"
#define INPUT_RELAY_GPIO     "rlg"
#define INPUT_RELAY_LEVEL    "irl"
#define INPUT_RELAY_MEMORY   "irm"
#define INPUT_RELAY_DURATION "ird"
#define INPUT_ROLLERSHUTTER  "irsr"

void createWebPageRelay();
void handleRelay(int save = 0);
void handleRelaySave();
#endif

enum _memory_relay
{
  MEMORY_OFF,
  MEMORY_ON,
  MEMORY_RESTORE
};

enum _conditions
{
  CONDITIONS_DS18B20 = 1,
  CONDITIONS_DHT_TEMP,
  CONDITIONS_DHT_HUMI,
};

#ifdef SUPLA_RELAY
void handleRelaySet(int save = 0);
void handleRelaySaveSet();
#endif

#ifdef GUI_SENSOR_I2C_EXPENDER
void handleRelaySetMCP23017(int save = 0);
void handleRelaySaveSetMCP23017();
#endif

#ifdef SUPLA_LED
#define PATH_LED        "led"
#define PATH_SAVE_LED   "saveled"
#define INPUT_LED       "led"
#define INPUT_LEVEL_LED "ill"
#endif

#ifdef SUPLA_CONDITIONS
#define INPUT_CONDITIONS_SENSOR_TYPE   "cst"
#define INPUT_CONDITIONS_TYPE          "ct"
#define INPUT_CONDITIONS_MIN           "cmi"
#define INPUT_CONDITIONS_MAX           "cma"
#define INPUT_CONDITIONS_SENSOR_NUMBER "csc"

void conditionsWebPage(int nr);
void conditionsWebPageSave(int nr);
#endif

#ifdef SUPLA_DIRECT_LINKS
#define INPUT_DIRECT_LINK_ON  "dlo"
#define INPUT_DIRECT_LINK_OFF "dlof"

void directLinksWebPage(int nr);
void directLinksWebPageSave(int nr);
#endif

#ifdef SUPLA_RF_BRIDGE
#define INPUT_RF_BRIDGE_TYPE           "irbt"
#define INPUT_RF_BRIDGE_PROTOCO        "irbp"
#define INPUT_RF_BRIDGE_PULSE_LENGTHIN "irbpl"
#define INPUT_RF_BRIDGE_LENGTH         "irbl"
#define INPUT_RF_BRIDGE_REPEAT         "irbr"
#define INPUT_RF_BRIDGE_CODE_ON        "irbcon"
#define INPUT_RF_BRIDGE_CODE_OFF       "irbcoff"

// void directLinksWebPage(int nr);
// void directLinksWebPageSave(int nr);
#endif

#endif  // SuplaWebPageRelay_h
