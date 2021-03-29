
#ifndef SuplaWebDeviceSettings_h
#define SuplaWebDeviceSettings_h

#include "SuplaDeviceGUI.h"

#define PATH_DEVICE_SETTINGS  "devicesettings"
#define PATH_DEFAULT_SETTINGS "defaultsettings"
#define PATH_LOGIN_SETTINGS   "loginsettings"
#define INPUT_BOARD           "board"

void createWebPageDeviceSettings();
void handleDeviceSettings(int save = 0);
void handleDeviceSettingsSave();

#endif  // ifndef SuplaWebDeviceSettings_h