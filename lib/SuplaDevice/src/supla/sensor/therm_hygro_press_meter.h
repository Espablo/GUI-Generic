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

#ifndef _therm_hygro_press_meter_h
#define _therm_hygro_press_meter_h

#include "therm_hygro_meter.h"

#define PRESSURE_NOT_AVAILABLE -1

namespace Supla {
namespace Sensor {
class ThermHygroPressMeter : public ThermHygroMeter {
 public:
  ThermHygroPressMeter();
  virtual double getPressure();
  void iterateAlways();
  bool iterateConnected(void *srpc);
  Element &disableChannelState();
  Channel *getSecondaryChannel();

  // Override local action methods in order to delegate execution to Channel and
  // Secondary Channel
  void addAction(int action, ActionHandler &client, int event);
  void addAction(int action, ActionHandler *client, int event);

 protected:
  Channel pressureChannel;
};

};  // namespace Sensor
};  // namespace Supla

#endif
