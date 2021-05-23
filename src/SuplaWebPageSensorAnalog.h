
#ifndef SuplaWebPageSensorAnalog_h
#define SuplaWebPageSensorAnalog_h

#include "SuplaDeviceGUI.h"

#if defined(SUPLA_NTC_10K) || defined(SUPLA_MPX_5XXX)
#define GUI_SENSOR_ANALOG
#endif

#ifdef GUI_SENSOR_ANALOG

#define PATH_ANALOG "analog"

void createWebPageSensorAnalog();
void handleSensorAnalog(int save = 0);
void handleSensorAnalogSave();

#ifdef SUPLA_NTC_10K
#define INPUT_NTC_10K "in10k"
#endif

#ifdef SUPLA_MPX_5XXX
#define PATH_MPX_5XX_EMPTY "mpxe"
#define PATH_MPX_5XX_FULL  "mpxf"

#define INPUT_MPX_5XXX     "impx"
#define INPUT_THANK_HEIGHT "ith"
#define INPUT_THANK_EMPTY  "ite"
#define INPUT_THANK_FULL   "itf"
#endif

#endif

#endif  // ifndef SuplaWebPageSensorAnalog_h
