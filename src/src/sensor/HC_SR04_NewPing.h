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
#include <supla/sensor/distance.h>

namespace Supla {
namespace Sensor {
class HC_SR04_NewPing : public Distance {
 public:
  HC_SR04_NewPing(int8_t trigPin, int8_t echoPin, int16_t minIn = 0, int16_t maxIn = 500, int16_t minOut = 0, int16_t maxOut = 500) {
    _trigPin = trigPin;
    _echoPin = echoPin;
    _minIn = minIn;
    _maxIn = maxIn;
    _minOut = minOut;
    _maxOut = maxOut;

    sonar = new NewPing(_trigPin, _echoPin, _maxIn);
    delay(100);  // give time to inizialise, preventing ping_median fails
    sonar->ping_median(5);
  }

  virtual double getValue() {
    float distance = (float)(sonar->ping_median(5)) / US_ROUNDTRIP_CM;

    float value = map(distance, _minIn, _maxIn, _minOut, _maxOut);
    if (_minOut < _maxOut) {
      value = constrain(value, _minOut, _maxOut);
    }
    else {
      value = constrain(value, _maxOut, _minOut);
    }

    return static_cast<double>(value) / 100.0;
  }

 protected:
  int8_t _trigPin;
  int8_t _echoPin;
  int16_t _minIn;
  int16_t _maxIn;
  int16_t _minOut;
  int16_t _maxOut;

  NewPing *sonar = nullptr;
};

};  // namespace Sensor
};  // namespace Supla

#endif
