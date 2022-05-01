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

#ifndef SuplaWebPageControl_h
#define SuplaWebPageControl_h

#include "SuplaDeviceGUI.h"

#if defined(SUPLA_BUTTON) || defined(SUPLA_LIMIT_SWITCH) || defined(SUPLA_MCP23017)
#define GUI_CONTROL
#endif

#ifdef GUI_CONTROL
#define PATH_CONTROL "control"

#define PATH_BUTTON_SET "setbutton"

#define INPUT_TRIGGER          "trs"
#define INPUT_BUTTON_SET       "bts"
#define INPUT_BUTTON_GPIO      "btg"
#define INPUT_BUTTON_GPIO_STOP "btgs"
#define INPUT_BUTTON_LEVEL     "ibl"
#define INPUT_BUTTON_INVERSED  "ibi"
#define INPUT_BUTTON_EVENT     "icl"
#define INPUT_BUTTON_ACTION    "bta"
#define INPUT_MAX_BUTTON       "mbt"
#define INPUT_ANALOG_EXPECTED  "iae"
#define INPUT_BUTTON_NUMBER    "ibn"

#define INPUT_AT_MULTICLICK_TIME "iamt"
#define INPUT_AT_HOLD_TIME       "iaht"

void createWebPageControl();
void handleControl(int save = 0);
void handleControlSave();
#endif

#if defined(SUPLA_BUTTON)
void handleButtonSet(int save = 0);
void handleButtonSaveSet();
#endif

#ifdef SUPLA_MCP23017
void handleButtonSetMCP23017(int save = 0);
void handleButtonSaveSetMCP23017();
#endif
#endif  // SuplaWebPageControl_h
