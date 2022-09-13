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

#include "AnalogReadingMap.h"

namespace Supla {
namespace Sensor {

AnalogRedingMap::AnalogRedingMap(uint8_t pin)
    : pin(pin), min(0), max(0), minDesired(0), maxDesired(0) {
}

void AnalogRedingMap::onInit() {
  pinMode(pin, INPUT);
  channel.setNewValue(0, getValue());
}

uint16_t AnalogRedingMap::readValuesFromDevice() {
  uint16_t average = 0;

  for (int i = 0; i < 10; i++) {
    average += analogRead(pin);
    delay(1);
  }
  average = average / 10;

  return average;
}

double AnalogRedingMap::getValue() {
  double value;

  if (min == max || minDesired == maxDesired) return TEMPERATURE_NOT_AVAILABLE;

  value = mapDouble(readValuesFromDevice(), min, max, minDesired, maxDesired);
  value = constrain(value, minDesired, maxDesired);

  return value;
}

void AnalogRedingMap::iterateAlways() {
  if (millis() - lastReadTime > 1000) {
    lastReadTime = millis();

    channel.setNewValue(0, getValue());
  }
}

void AnalogRedingMap::onSaveState() {
  Supla::Storage::WriteState((unsigned char *)&min, sizeof(min));
  Supla::Storage::WriteState((unsigned char *)&max, sizeof(max));
  Supla::Storage::WriteState((unsigned char *)&minDesired, sizeof(minDesired));
  Supla::Storage::WriteState((unsigned char *)&maxDesired, sizeof(maxDesired));
}

void AnalogRedingMap::onLoadState() {
  Supla::Storage::ReadState((unsigned char *)&min, sizeof(min));
  Supla::Storage::ReadState((unsigned char *)&max, sizeof(max));
  Supla::Storage::ReadState((unsigned char *)&minDesired, sizeof(minDesired));
  Supla::Storage::ReadState((unsigned char *)&maxDesired, sizeof(maxDesired));
}

double AnalogRedingMap::mapDouble(
    double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void AnalogRedingMap::calibrateMinValue() {
  setMinValue(readValuesFromDevice());
  Serial.print(F("Calibrate - write MIN value: "));
  Serial.println(min);
  Supla::Storage::ScheduleSave(1000);
}

void AnalogRedingMap::calibrateMaxValue() {
  setMaxValue(readValuesFromDevice());
  Serial.print(F("Calibrate - write MAX value: "));
  Serial.println(max);
  Supla::Storage::ScheduleSave(1000);
}

void AnalogRedingMap::setMinValue(float value) {
  min = value;
  Supla::Storage::ScheduleSave(1000);
}

float AnalogRedingMap::getMinValue() {
  return min;
}

void AnalogRedingMap::setMaxValue(float value) {
  max = value;
  Supla::Storage::ScheduleSave(1000);
}

float AnalogRedingMap::getMaxValue() {
  return max;
}

void AnalogRedingMap::setMinDesiredValue(float value) {
  minDesired = value;
  Supla::Storage::ScheduleSave(1000);
}

float AnalogRedingMap::getMinDesiredValue() {
  return minDesired;
}

void AnalogRedingMap::setMaxDesiredValue(float value) {
  maxDesired = value;
  Supla::Storage::ScheduleSave(1000);
}

float AnalogRedingMap::getMaxDesiredValue() {
  return maxDesired;
}

};  // namespace Sensor
};  // namespace Supla
