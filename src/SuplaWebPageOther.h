#ifndef SuplaWebPageOther_h
#define SuplaWebPageOther_h

#include "SuplaDeviceGUI.h"

#if defined(SUPLA_HC_SR04) || defined(SUPLA_NTC_10K)
#define GUI_SENSOR_OTHER
#endif

#if defined(SUPLA_IMPULSE_COUNTER) || defined(SUPLA_HLW8012) || defined(SUPLA_PZEM_V_3) || defined(SUPLA_RGBW) || defined(SUPLA_PUSHOVER) || \
    defined(GUI_SENSOR_OTHER) || defined(SUPLA_CSE7766)
#define GUI_OTHER
#endif

#ifdef GUI_OTHER
#define PATH_OTHER "other"

void createWebPageOther();
void handleOther(int save = 0);
void handleOtherSave();
#endif

#if defined(SUPLA_IMPULSE_COUNTER)
#define INPUT_IMPULSE_COUNTER_GPIO             "ic"
#define INPUT_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT "icdt"
#define INPUT_IMPULSE_COUNTER_PULL_UP          "icpu"
#define INPUT_IMPULSE_COUNTER_RAISING_EDGE     "icre"
#define INPUT_IMPULSE_COUNTER_CHANGE_VALUE     "iccv"
#define INPUT_MAX_IMPULSE_COUNTER              "imic"

void handleImpulseCounterSet(int save = 0);
void handleImpulseCounterSaveSet();
#endif

#if defined(SUPLA_HLW8012)
#define INPUT_CF  "cf"
#define INPUT_CF1 "cf1"
#define INPUT_SEL "sel"

#define INPUT_COUNTER_CHANGE_VALUE_HLW8012 "iccvh"
#endif

#ifdef SUPLA_CSE7766
#define INPUT_CSE7766_RX "isrx"

#define INPUT_COUNTER_CHANGE_VALUE_CSE7766 "iccvc"
#endif

#if defined(SUPLA_HLW8012) || defined(SUPLA_CSE7766)
#define PATH_CALIBRATE      "calibrate"
#define INPUT_CALIB_POWER   "power"
#define INPUT_CALIB_VOLTAGE "voltage"

#define PATH_CSE7766 "cse7766"
#define PATH_HLW8012 "hlw8012"

void handleCounterCalibrate(int save = 0);
void handleCounterCalibrateSave();
#endif

#ifdef SUPLA_PZEM_V_3
#define INPUT_PZEM_RX "iprx"
#define INPUT_PZEM_TX "iptx"
#endif

#ifdef SUPLA_RGBW
#define INPUT_RGBW_MAX              "rgbwm"
#define INPUT_RGBW_RED              "rgbwr"
#define INPUT_RGBW_GREEN            "rgbwg"
#define INPUT_RGBW_BLUE             "rgbwb"
#define INPUT_RGBW_BRIGHTNESS       "rgbwbr"
#define INPUT_RGBW_COLOR_BRIGHTNESS "rgbwcb"
#endif

#ifdef SUPLA_PUSHOVER
#define INPUT_PUSHOVER_TOKEN "pot"
#define INPUT_PUSHOVER_USER  "pou"
#endif

#ifdef SUPLA_HC_SR04
#define INPUT_TRIG_GPIO               "trig"
#define INPUT_ECHO_GPIO               "echo"
#define INPUT_HC_SR04_MAX_SENSOR_READ "hsm"
#endif

#ifdef SUPLA_IMPULSE_COUNTER
#define PATH_IMPULSE_COUNTER_SET "setcounter"
#endif

#ifdef SUPLA_NTC_10K
#define INPUT_NTC_10K "in10k"
#endif

#endif  // SuplaWebPageOther_h
