#ifndef SuplaWebPageConfig_h
#define SuplaWebPageConfig_h

#include "SuplaDeviceGUI.h"

#ifdef SUPLA_CONFIG
#define PATH_CONFIG "config"

#define INPUT_CFG_LED_GPIO  "cfgl"
#define INPUT_CFG_BTN_GPIO  "cfgb"
#define INPUT_CFG_LED_LEVEL "icll"
#define INPUT_CFG_MODE      "cfgm"

void createWebPageConfig();
void handleConfig(int save = 0);
void handleConfigSave();
#endif

#endif  // SuplaWebPageConfig_h
