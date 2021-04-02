#ifndef _bmp280_h
#define _bmp280_h

// Dependency: Adafruid BMP280 library - use library manager to install it
#include <Adafruit_BMP280.h>

#include "supla/sensor/therm_press_meter.h"

namespace Supla {
namespace Sensor {
class BMP280 : public ThermPressMeter {
 public:
  BMP280(int8_t address = 0x77, float altitude = NAN) : address(address), sensorStatus(false), altitude(altitude) {
  }

  double getValue() {
    float value = TEMPERATURE_NOT_AVAILABLE;
    bool retryDone = false;
    do {
      if (!sensorStatus || isnan(value)) {
        sensorStatus = bmp.begin(address);
        retryDone = true;
      }
      value = TEMPERATURE_NOT_AVAILABLE;
      if (sensorStatus) {
        value = bmp.readTemperature();
      }
    } while (isnan(value) && !retryDone);
    return value;
  }

  double getPressure() {
    float value = PRESSURE_NOT_AVAILABLE;
    bool retryDone = false;
    do {
      if (!sensorStatus || isnan(value)) {
        sensorStatus = bmp.begin(address);
        retryDone = true;
      }
      value = PRESSURE_NOT_AVAILABLE;
      if (sensorStatus) {
        value = bmp.readPressure() / 100.0;
      }
    } while (isnan(value) && !retryDone);
    if (!isnan(altitude)) {
      value = bmp.seaLevelForAltitude(altitude, value);
    }
    return value;
  }

  void onInit() {
    sensorStatus = bmp.begin(address);

    pressureChannel.setNewValue(getPressure());
    channel.setNewValue(getValue());
  }

  void setAltitude(float newAltitude) {
    altitude = newAltitude;
  }

 protected:
  int8_t address;
  bool sensorStatus;
  float altitude;
  Adafruit_BMP280 bmp;  // I2C
};

};  // namespace Sensor
};  // namespace Supla

#endif
