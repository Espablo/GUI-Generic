
#ifndef SuplaWebPageLimitSwitch_h
#define SuplaWebPageLimitSwitch_h

#include "SuplaDeviceGUI.h"

#ifdef SUPLA_LIMIT_SWITCH

#define PATH_SWITCH "switch"

#define INPUT_MAX_LIMIT_SWITCH  "mls"
#define INPUT_LIMIT_SWITCH_GPIO "lsg"

void createWebPageLimitSwitch();
void handleLimitSwitch(int save = 0);
void handleLimitSwitchSave();

#endif

#endif  // ifndef SuplaWebPageLimitSwitch_h
