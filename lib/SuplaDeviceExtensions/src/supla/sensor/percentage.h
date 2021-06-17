
#ifndef _percentage_h
#define _percentage_h

#include <Arduino.h>

#include "supla/channel_element.h"

#define HUMIDITY_NOT_AVAILABLE -1

namespace Supla {
namespace Sensor {
class Percentage : public ChannelElement {
 public:
  Percentage(Supla::ChannelElement *src,
             int16_t minValue,
             int16_t maxValue,
             bool useAlternativeMeasurement = false);
  virtual double getValue();
  void iterateAlways();

  void setMinValue(int16_t minValue);
  void setMaxValue(int16_t maxValue);

  int16_t getMinValue();
  int16_t getMaxValue();

 protected:
  unsigned long lastReadTime;
  bool useAlternativeMeasurement;
  int16_t _minValue;
  int16_t _maxValue;

  Supla::ChannelElement *source;
};

};  // namespace Sensor
};  // namespace Supla

#endif
