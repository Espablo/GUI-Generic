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
  humidityChannel.setNewValue(getHumi());
}

double MPX_5XXX::getValue() {
  if (_thankHeight == 0) return DISTANCE_NOT_AVAILABLE;

  long value = map(sensorValue, _emptyValue, _fullValue, 0, _thankHeight);
  value = constrain(value, 0, _thankHeight);

  return static_cast<double>(value) / 100.00;
}

double MPX_5XXX::getHumi() {
  if (_thankHeight == 0) return DISTANCE_NOT_AVAILABLE;

  long value = map(sensorValue, _emptyValue, _fullValue, 0, 100);
  value = constrain(value, 0, 100);

  return static_cast<double>(value);
}

void MPX_5XXX::iterateAlways() {
  if (millis() - lastReadTime > 1000) {
    lastReadTime = millis();

    sensorValue = readValuesFromDevice();

    channel.setNewValue(getValue());
    humidityChannel.setNewValue(0, getHumi());
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