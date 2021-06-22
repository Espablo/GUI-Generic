
#ifndef _mpx_5xxx_h
#define _mpx_5xxx_h

#include <Arduino.h>
#include <supla/storage/storage.h>

#include "distance_hygro_meter.h"
#include "supla/channel.h"

namespace Supla {
namespace Sensor {
class MPX_5XXX : public Distance {
 public:
  MPX_5XXX(uint8_t pin);

  void onInit();
  virtual double getValue();
  void iterateAlways();
  void onSaveState();
  void onLoadState();

  void calibrateEmptyThank();
  void calibrateFullThank();

  void setEmptyValue(int16_t value);
  int16_t getEmptyValue();

  void setFullValue(int16_t value);
  int16_t getFullValue();

  void setThankHeight(int16_t value);
  int16_t getThankHeight();

  int16_t readValuesFromDevice();

 protected:
  uint8_t _pin;

  double sensorValue;
  
  int16_t _emptyValue;
  int16_t _fullValue;
  int16_t _thankHeight;  // maximum thank height reading in centimeters
};

};  // namespace Sensor
};  // namespace Supla

#endif
