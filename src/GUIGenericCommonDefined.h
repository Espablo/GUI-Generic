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

#ifndef GUI_GENERIC_COMMON_DEFINED_H
#define GUI_GENERIC_COMMON_DEFINED_H

#if defined(SUPLA_MCP23017) || defined(SUPLA_ADE7953)
#define GUI_SENSOR_I2C_EXPENDER
#endif

#ifndef DEBUG_MODE
#define supla_lib_config_h_  // silences unnecessary debug messages "should be disabled by default"
#endif

#ifndef TEMPLATE_BOARD_OLD
#ifndef TEMPLATE_BOARD_JSON
#define TEMPLATE_BOARD_JSON
#endif
#endif

#endif