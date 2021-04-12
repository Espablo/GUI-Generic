#ifndef SuplaWebPageControl_h
#define SuplaWebPageControl_h

#include "SuplaDeviceGUI.h"

#if defined(SUPLA_BUTTON) || defined(SUPLA_LIMIT_SWITCH) || defined(SUPLA_MCP23017)
#define GUI_CONTROL
#endif

#ifdef GUI_CONTROL
#define PATH_CONTROL "control"

#define PATH_BUTTON_SET          "setbutton"

#define INPUT_TRIGGER         "trs"
#define INPUT_BUTTON_SET      "bts"
#define INPUT_BUTTON_GPIO     "btg"
#define INPUT_BUTTON_LEVEL    "ibl"
#define INPUT_BUTTON_INVERSED "ibi"
#define INPUT_BUTTON_EVENT    "icl"
#define INPUT_BUTTON_ACTION   "bta"
#define INPUT_MAX_BUTTON      "mbt"

void createWebPageControl();
void handleControl(int save = 0);
void handleControlSave();
#endif

#if defined(SUPLA_BUTTON)
void handleButtonSet(int save = 0);
void handleButtonSaveSet();
#endif

#ifdef SUPLA_MCP23017
void handleButtonSetMCP23017(int save = 0);
void handleButtonSaveSetMCP23017();
#endif
#endif  // SuplaWebPageControl_h
