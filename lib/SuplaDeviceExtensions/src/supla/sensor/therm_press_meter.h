#ifndef _therm_press_meter_h
#define _therm_press_meter_h

#include <supla/sensor/thermometer.h>

#define PRESSURE_NOT_AVAILABLE -1

namespace Supla {
namespace Sensor {
class ThermPressMeter : public Thermometer {
 public:
  ThermPressMeter();
  virtual double getPressure();
  void iterateAlways();
  bool iterateConnected(void *srpc);
  Element &disableChannelState();
  Channel *getSecondaryChannel();

  // Override local action methods in order to delegate execution to Channel and
  // Secondary Channel
  void addAction(int action, ActionHandler &client, int event);
  void addAction(int action, ActionHandler *client, int event);

 protected:
  Channel pressureChannel;
};

};  // namespace Sensor
};  // namespace Supla

#endif
