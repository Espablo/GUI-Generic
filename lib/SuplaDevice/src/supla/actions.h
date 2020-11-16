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

#ifndef _actions_h
#define _actions_h

// Actions are used in triggerable elements. They are grouped by most common
// usage, but you should not rely on it. Please check exact supported actions
// in triggerable element documentation

namespace Supla {
enum Action {
  // Relays
  TURN_ON,
  TURN_OFF,
  TOGGLE,

  // Settable binary sensors
  SET,
  CLEAR,

  // Roller shutters
  OPEN,
  CLOSE,
  STOP,
  OPEN_OR_STOP,
  CLOSE_OR_STOP,
  COMFORT_UP_POSITION,
  COMFORT_DOWN_POSITION,
  STEP_BY_STEP,
  MOVE_UP,
  MOVE_DOWN,
  MOVE_UP_OR_STOP,
  MOVE_DOWN_OR_STOP,

  // Dimmable light, RGB(W) LEDs
  BRIGHTEN_ALL,
  DIM_ALL,
  BRIGHTEN_R,
  BRIGHTEN_G,
  BRIGHTEN_B,
  BRIGHTEN_W,
  BRIGHTEN_RGB,
  DIM_R,
  DIM_G,
  DIM_B,
  DIM_W,
  DIM_RGB,
  TURN_ON_RGB,
  TURN_OFF_RGB,
  TOGGLE_RGB,
  TURN_ON_W,
  TURN_OFF_W,
  TOGGLE_W,
  TURN_ON_RGB_DIMMED,
  TURN_ON_W_DIMMED,
  TURN_ON_ALL_DIMMED,
  ITERATE_DIM_RGB,
  ITERATE_DIM_W,
  ITERATE_DIM_ALL,

  // Impulse counter
  RESET

};

};

#endif
