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
#ifndef _PMSx003_h
#define _PMSx003_h

#include <Arduino.h>
#include <supla/sensor/thermometer.h>
#include <supla/element.h>

#include <PMserial.h>

namespace Supla {
namespace Sensor {

class PMSx003 : public ChannelElement {
 public:
  PMSx003(int8_t pin_rx, int8_t pin_tx = -1);
  void onInit();
  void iterateAlways();

 protected:
  unsigned long lastReadTime;
};

class PMS_PM01 : public Thermometer {
 public:
  PMS_PM01();
  double getValue();
};

class PMS_PM25 : public Thermometer {
 public:
  PMS_PM25();
  double getValue();
};

class PMS_PM10 : public Thermometer {
 public:
  PMS_PM10();
  double getValue();
};

}  // namespace Sensor
}  // namespace Supla

#endif