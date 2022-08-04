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

#ifndef _percentage_h
#define _percentage_h

#include <Arduino.h>

#include "supla/channel_element.h"

#define HUMIDITY_NOT_AVAILABLE -1

namespace Supla {
namespace Sensor {
class Percentage : public ChannelElement {
 public:
  Percentage(Supla::ChannelElement *src,
             double minValue,
             double maxValue,
             bool useAlternativeMeasurement = false);
  virtual double getValue();
  void iterateAlways();

  void setMinValue(double minValue);
  void setMaxValue(double maxValue);

  double getMinValue();
  double getMaxValue();

  double mapDouble(
      double x, double in_min, double in_max, double out_min, double out_max);

 protected:
  Supla::ChannelElement *source;
  double _minValue;
  double _maxValue;
  bool useAlternativeMeasurement;
  unsigned long lastReadTime;
};

};  // namespace Sensor
};  // namespace Supla

#endif
