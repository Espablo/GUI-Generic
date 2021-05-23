#include "MPX_5xxx.h"

namespace Supla {
namespace Sensor {

MPX_5XXX::MPX_5XXX(uint8_t pin)
    : _pin(pin),
      readouts{},
      index(0),
      _emptyValue(0),
      _fullValue(0),
      _thankHeight(0) {
}

void MPX_5XXX::onInit() {
  channel.setNewValue(getValue());
}

double MPX_5XXX::getValue() {
  if (_thankHeight == 0) return DISTANCE_NOT_AVAILABLE;

  long value =
      map(readValuesFromDevice(), _emptyValue, _fullValue, _thankHeight, 0);

  value = constrain(value, 0, _thankHeight);

  return static_cast<double>(value) / 100.00;
}

void MPX_5XXX::iterateAlways() {
  if (lastReadTime + 1000 < millis()) {
    lastReadTime = millis();
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
  int16_t value;

  index++;
  if (index > 4) index = 0;
  readouts[index] = analogRead(_pin);

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
    value = sum / count;
  }

  return value;
}

};  // namespace Sensor
};  // namespace Supla