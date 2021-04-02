#include "SuplaWebPageSensors.h"

void createWebPageSensors() {
#ifdef GUI_SENSOR_SPI
  createWebPageSensorSpi();
#endif

#ifdef GUI_SENSOR_I2C
  createWebPageSensorI2c();
#endif

#ifdef GUI_SENSOR_1WIRE
  createWebPageSensor1Wire();
#endif

#if defined(GUI_SENSOR_SPI) || defined(GUI_SENSOR_I2C) || defined(GUI_SENSOR_1WIRE)
  createWebCorrection();
#endif
}
