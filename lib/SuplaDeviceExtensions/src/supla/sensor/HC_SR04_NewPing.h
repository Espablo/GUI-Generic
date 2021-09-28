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

#ifndef _hc_sr04_newping_h
#define _hc_sr04_newping_h

#include <NewPing.h>
#include <supla/sensor/HC_SR04.h>

namespace Supla {
namespace Sensor {
class HC_SR04_NewPing : public HC_SR04 {
 public:
  HC_SR04_NewPing(int8_t trigPin,
                  int8_t echoPin,
                  int16_t minIn = 0,
                  int16_t maxIn = 500,
                  int16_t minOut = 0,
                  int16_t maxOut = 500)
      : HC_SR04(trigPin, echoPin, minIn, maxIn, minOut, maxOut) {
    sonar = new NewPing(trigPin, echoPin, maxIn);
  }

  virtual double getValue() {
    unsigned long uS = sonar->ping_median();

    if (uS < 50) {
      failCount++;
    } else {
      failCount = 0;

      unsigned long distance = sonar->convert_cm(uS);

      value = map(distance, _minIn, _maxIn, _minOut, _maxOut);
      if (_minOut < _maxOut) {
        value = constrain(value, _minOut, _maxOut);
      } else {
        value = constrain(value, _maxOut, _minOut);
      }
    }

    return failCount <= 3 ? static_cast<double>(value) / 100.0
                          : DISTANCE_NOT_AVAILABLE;
  }

 protected:
  NewPing *sonar = nullptr;
  long value;
};

};  // namespace Sensor
};  // namespace Supla

#endif
