#ifndef SuplaSensorDS18B20_h
#define SuplaSensorDS18B20_h

#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#include <supla-common/log.h>
#include <supla/sensor/thermometer.h>


class OneWireBus {
 public:
  OneWireBus(uint8_t pinNumberConfig);
  int8_t getIndex(uint8_t *deviceAddress);

  OneWire oneWire;
  uint8_t pin;
  OneWireBus *nextBus;
  unsigned long lastReadTime;
  DallasTemperature sensors;
};

class DS18B20 : public Supla::Sensor::Thermometer {
 public:
  DS18B20(uint8_t pin, uint8_t *deviceAddress = nullptr);
  void iterateAlways();
  double getValue();
  void onInit();
  uint8_t getPin();
  void setDeviceAddress(uint8_t *deviceAddress = nullptr);

 protected:
  static OneWireBus *oneWireBus;
  OneWireBus *myBus;
  DeviceAddress address;
  int8_t retryCounter;
  double lastValidValue;
};

#endif  // SuplaSensorDS18B20_h
