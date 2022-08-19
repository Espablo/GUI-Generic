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

#ifndef SuplaWebPageLimitSwitch_h
#define SuplaWebPageLimitSwitch_h

#include "SuplaDeviceGUI.h"

#ifdef SUPLA_LIMIT_SWITCH

#define PATH_SWITCH     "switch"
#define PATH_SWITCH_SET "switchset"

#define INPUT_MAX_LIMIT_SWITCH   "mls"
#define INPUT_LIMIT_SWITCH_GPIO  "lsg"
#define INPUT_LIMIT_SWITCH_PULLUP "ilsp"

void createWebPageLimitSwitch();
void handleLimitSwitch(int save = 0);
void handleLimitSwitchSave();

void handleLimitSwitchSet(int save = 0);
void handleLimitSwitchSaveSet();

#endif

#endif  // ifndef SuplaWebPageLimitSwitch_h
