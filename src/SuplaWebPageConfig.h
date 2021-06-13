#ifndef SuplaWebPageConfig_h
#define SuplaWebPageConfig_h

#include "SuplaDeviceGUI.h"

#ifdef SUPLA_CONFIG
#define PATH_CONFIG "config"

#define INPUT_CFG_LED_GPIO      "cfgl"
#define INPUT_CFG_BTN_GPIO      "cfgb"
#define INPUT_CFG_LED_LEVEL     "icll"
#define INPUT_CFG_MODE          "cfgm"
#define INPUT_CFG_SSL           "ics"
#define INPUT_CFG_AVAILABLE_GUI "icag"

#ifdef SUPLA_DEEP_SLEEP
#define INPUT_DEEP_SLEEP_TIME    "idst"
#endif

void createWebPageConfig();
void handleConfig(int save = 0);
void handleConfigSave();
#endif

#endif  // SuplaWebPageConfig_h
