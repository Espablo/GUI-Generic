#ifndef _hcsr04_h
#define _hcsr04_h

#include <Arduino.h>
#include <supla/sensor/HC_SR04.h>

namespace Supla {
namespace Sensor {
class HCSR04 : public HC_SR04 {
  virtual double getValue() {
    // noInterrupts();
    // digitalWrite(_trigPin, HIGH);
    // delayMicroseconds(10);
    // digitalWrite(_trigPin, LOW);

    digitalWrite(_trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);
    noInterrupts();

    unsigned long duration = pulseIn(_echoPin, HIGH, 60000);
    interrupts();
    if (duration > 50) {
      index++;
      if (index > 4) index = 0;
      readouts[index] = duration;
      failCount = 0;
    } else {
      failCount++;
    }

    unsigned long min = 0, max = 0, sum = 0;
    int count = 0;
    for (int i = 0; i < 5; i++) {
      if (readouts[i] > 0) {
        count++;
        if (min > readouts[i] || min == 0) min = readouts[i];
        if (max < readouts[i]) max = readouts[i];
        sum += readouts[i];
      }
    }

    if (count == 5) {
      if (min > 0) {
        sum -= min;
        count--;
      }
      if (max > 0) {
        sum -= max;
        count--;
      }
    }
    if (count > 0) {
      duration = sum / count;
    }

    long distance = (duration / 2.0) / 29.1;
    long value = map(distance, _minIn, _maxIn, _minOut, _maxOut);
    if (_minOut < _maxOut) {
      value = constrain(value, _minOut, _maxOut);
    } else {
      value = constrain(value, _maxOut, _minOut);
    }
    return failCount <= 3 ? static_cast<double>(value) / 100.0
                          : DISTANCE_NOT_AVAILABLE;
  }
};

};  // namespace Sensor
};  // namespace Supla

#endif
