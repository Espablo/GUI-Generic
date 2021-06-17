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

#include "percentage.h"

namespace Supla {
namespace Sensor {
Supla::Sensor::Percentage::Percentage(Supla::ChannelElement *src,
                                      int16_t minValue,
                                      int16_t maxValue,
                                      bool useAlternativeMeasurement)
    : source(src),
      _minValue(minValue),
      _maxValue(maxValue),
      useAlternativeMeasurement(useAlternativeMeasurement),
      lastReadTime(0) {
  channel.setType(SUPLA_CHANNELTYPE_HUMIDITYSENSOR);
  channel.setDefault(SUPLA_CHANNELFNC_HUMIDITY);
  channel.setNewValue(HUMIDITY_NOT_AVAILABLE);
}

double Percentage::getValue() {
  if (_minValue == _maxValue) return HUMIDITY_NOT_AVAILABLE;

  int channelType = source->getChannel()->getChannelType();

  double value = 0;
  switch (channelType) {
    case SUPLA_CHANNELTYPE_DISTANCESENSOR:
    case SUPLA_CHANNELTYPE_THERMOMETER:
    case SUPLA_CHANNELTYPE_WINDSENSOR:
    case SUPLA_CHANNELTYPE_PRESSURESENSOR:
    case SUPLA_CHANNELTYPE_RAINSENSOR:
    case SUPLA_CHANNELTYPE_WEIGHTSENSOR:
      value = source->getChannel()->getValueDouble();
      break;
    case SUPLA_CHANNELTYPE_IMPULSE_COUNTER:
      value = source->getChannel()->getValueInt64();
      break;
    case SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR:
      value = useAlternativeMeasurement
                  ? source->getChannel()->getValueDoubleSecond()
                  : source->getChannel()->getValueDoubleFirst();
      break;
    default:
      return HUMIDITY_NOT_AVAILABLE;

      double value = map(value, _minValue, _maxValue, 0, 100);
      value = constrain(value, 0, 100);

      return static_cast<double>(value);
  }
}

void Percentage::setMinValue(int16_t minValue) {
  _minValue = minValue;
}
void Percentage::setMaxValue(int16_t maxValue) {
  _maxValue = maxValue;
}

int16_t Percentage::getMinValue() {
  return _minValue;
}

int16_t Percentage::getMaxValue() {
  return _maxValue;
}

void Percentage::iterateAlways() {
  if (millis() - lastReadTime > 10000) {
    lastReadTime = millis();
    channel.setNewValue(0, getValue());
  }
}

}  // namespace Sensor
}  // namespace Supla
