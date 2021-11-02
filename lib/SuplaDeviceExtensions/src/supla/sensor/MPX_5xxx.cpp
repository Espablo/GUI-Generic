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

#include "MPX_5xxx.h"

namespace Supla {
namespace Sensor {

MPX_5XXX::MPX_5XXX(uint8_t pin)
    : _pin(pin),
      sensorValue(0),
      _emptyValue(0),
      _fullValue(0),
      _thankHeight(0) {
}

void MPX_5XXX::onInit() {
  channel.setNewValue(getValue());
}

double MPX_5XXX::getValue() {
  double value;

  if (_emptyValue == _fullValue) return DISTANCE_NOT_AVAILABLE;

  if (_thankHeight != 0) {
    value = map(sensorValue, _emptyValue, _fullValue, 0, _thankHeight);
    value = constrain(value, 0, _thankHeight);
    value = value * 0.01;
  } else {
    value = map(sensorValue, _emptyValue, _fullValue, 0, 100);
    value = constrain(value, 0, 100);
  }

  return value;
}

void MPX_5XXX::iterateAlways() {
  if (millis() - lastReadTime > 1000) {
    lastReadTime = millis();

    sensorValue = readValuesFromDevice();

    channel.setNewValue(getValue());
  }
}

void MPX_5XXX::onSaveState() {
  Supla::Storage::WriteState((unsigned char *)&_emptyValue,
                             sizeof(_emptyValue));

  Supla::Storage::WriteState((unsigned char *)&_fullValue, sizeof(_fullValue));
  Supla::Storage::WriteState((unsigned char *)&_thankHeight,
                             sizeof(_thankHeight));
}

void MPX_5XXX::onLoadState() {
  Supla::Storage::ReadState((unsigned char *)&_emptyValue, sizeof(_emptyValue));

  Supla::Storage::ReadState((unsigned char *)&_fullValue, sizeof(_fullValue));

  Supla::Storage::ReadState((unsigned char *)&_thankHeight,
                            sizeof(_thankHeight));
}

void MPX_5XXX::calibrateEmptyThank() {
  setEmptyValue(readValuesFromDevice());
  Serial.print(F("Write MPX Empty Thank: "));
  Serial.println(_emptyValue);
}

void MPX_5XXX::calibrateFullThank() {
  setFullValue(readValuesFromDevice());
  Serial.print(F("Write MPX Full Thank: "));
  Serial.println(_fullValue);
}

void MPX_5XXX::setEmptyValue(int16_t value) {
  _emptyValue = value;
  Supla::Storage::ScheduleSave(1000);
}

int16_t MPX_5XXX::getEmptyValue() {
  return _emptyValue;
}

void MPX_5XXX::setFullValue(int16_t value) {
  _fullValue = value;
  Supla::Storage::ScheduleSave(1000);
}

int16_t MPX_5XXX::getFullValue() {
  return _fullValue;
}

void MPX_5XXX::setThankHeight(int16_t value) {
  _thankHeight = value;
  Supla::Storage::ScheduleSave(1000);
}

int16_t MPX_5XXX::getThankHeight() {
  return _thankHeight;
}

int16_t MPX_5XXX::readValuesFromDevice() {
  static uint8_t samples = 10;
  static int readings[20];
  static uint8_t cur = 0;
  static uint8_t allValid = 0;
  int16_t average = 0;

  readings[cur++ % samples] = analogRead(_pin);

  if (allValid) {
    for (uint8_t i = 0; i < samples; i++) {
      average += readings[i];
    }
    average /= samples;
  } else {
    for (uint8_t i = 0; i < cur; i++) {
      average += readings[i];
    }
    average /= cur;
    if (cur == samples) {
      allValid = 1;
    }
  }

  return average;
}

};  // namespace Sensor
};  // namespace Supla