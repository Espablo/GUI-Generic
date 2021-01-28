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

#include "Si7021_sonoff.h"

namespace Supla {
namespace Sensor {
Si7021Sonoff::Si7021Sonoff(int pin)
    : temperature(TEMPERATURE_NOT_AVAILABLE),
      humidity(HUMIDITY_NOT_AVAILABLE),
      pin(pin),
      retryCount(0) {
}

double Si7021Sonoff::getTemp() {
  return temperature;
}

double Si7021Sonoff::getHumi() {
  return humidity;
}

void Si7021Sonoff::iterateAlways() {
  if (millis() - lastReadTime > 10000) {
    lastReadTime = millis();
    read();
    channel.setNewValue(getTemp(), getHumi());
  }
}

void Si7021Sonoff::onInit() {
  pinMode(pin, INPUT);

  delay(100);
  read();
  channel.setNewValue(getTemp(), getHumi());
}

double Si7021Sonoff::readTemp(uint8_t* data) {
  double temp = (((data[2] & 0x7F) << 8) | data[3]) * 0.1;
  if (data[2] & 0x80) {
    temp *= -1;
  }
  return temp;
}

double Si7021Sonoff::readHumi(uint8_t* data) {
  double humi = ((data[0] << 8) | data[1]) * 0.1;
  return humi;
}

bool Si7021Sonoff::read() {
  uint8_t data[5] = {0};

  yield();

  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delayMicroseconds(500);
  digitalWrite(pin, HIGH);
  delayMicroseconds(20);
  pinMode(pin, INPUT);

  uint32_t i = 0;
  if (waitState(0) && waitState(1) && waitState(0)) {
    for (i = 0; i < 40; i++) {
      if (!waitState(1)) {
        break;
      }
      delayMicroseconds(35);
      if (digitalRead(pin) == HIGH) {
        data[i / 8] |= (1 << (7 - i % 8));
      }
      if (!waitState(0)) {
        break;
      }
    }
  }

  uint8_t checksum = (data[0] + data[1] + data[2] + data[3]) & 0xFF;
  if (i < 40 || data[4] != checksum) {
    retryCount++;
    if (retryCount > 3) {
      retryCount = 0;
      temperature = TEMPERATURE_NOT_AVAILABLE;
      humidity = HUMIDITY_NOT_AVAILABLE;
    }
  } else {
    retryCount = 0;
    temperature = readTemp(data);
    humidity = readHumi(data);
  }
}

bool Si7021Sonoff::waitState(bool state) {
  unsigned long timeout = micros();
  while (micros() - timeout < 100) {
    if (digitalRead(pin) == state) return true;
    delayMicroseconds(1);
  }
  return false;
}

};  // namespace Sensor
};  // namespace Supla