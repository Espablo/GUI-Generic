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

#ifndef SuplaWebPageSensorSpi_h
#define SuplaWebPageSensorSpi_h

#include "SuplaDeviceGUI.h"

#if defined(SUPLA_MAX6675) || defined(SUPLA_MAX31855)
#define GUI_SENSOR_SPI
#endif

#ifdef GUI_SENSOR_SPI

#define PATH_SPI "spi"

#define INPUT_MAX6675  "max6675"
#define INPUT_MAX31855 "imax31855"
#define INPUT_CLK_GPIO "clk"
#define INPUT_CS_GPIO  "cs"
#define INPUT_D0_GPIO  "d0"

void createWebPageSensorSpi();
void handleSensorSpi(int save = 0);
void handleSensorSpiSave();
#endif

#endif  // ifndef SuplaWebPageSensorSpi_h
