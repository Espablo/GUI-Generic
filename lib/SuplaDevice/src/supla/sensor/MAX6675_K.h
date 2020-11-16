#ifndef _max6675_k_h
#define _max6675_k_h

#if defined(ESP8266)
#include <pgmspace.h>
#endif
#ifdef avr
#include <util/delay.h>
#endif

#include <stdlib.h>
#include "supla/channel.h"
#include "supla/sensor/thermometer.h"

namespace Supla {
namespace Sensor {
class MAX6675_K : public Thermometer {
 public:
  MAX6675_K(uint8_t pin_CLK, uint8_t pin_CS, uint8_t pin_DO) {
    _pin_CLK = pin_CLK;
    _pin_CS = pin_CS;
    _pin_DO = pin_DO;
  }

  double getValue() {
    uint16_t value;

    digitalWrite(_pin_CS, LOW);
    delay(1);

    value = spi_read();
    value <<= 8;
    value |= spi_read();

    digitalWrite(_pin_CS, HIGH);

    if (value & 0x4) {
      return -275;
    }
    value >>= 3;

    return value * 0.25;
  }

  void onInit() {
    pinMode(_pin_CS, OUTPUT);
    pinMode(_pin_CLK, OUTPUT);
    pinMode(_pin_DO, INPUT);

    digitalWrite(_pin_CS, HIGH);

    channel.setNewValue(getValue());
  }

  byte spi_read(void) {
    int i;
    byte d = 0;

    for (i = 7; i >= 0; i--) {
      digitalWrite(_pin_CLK, LOW);
      delay(1);
      if (digitalRead(_pin_DO)) {
        d |= (1 << i);
      }

      digitalWrite(_pin_CLK, HIGH);
      delay(1);
    }

    return d;
  }

 protected:
  int8_t _pin_CLK;
  int8_t _pin_CS;
  int8_t _pin_DO;
};
};  // namespace Sensor
};  // namespace Supla

#endif