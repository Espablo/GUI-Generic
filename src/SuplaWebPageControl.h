#ifndef SuplaWebPageControl_h
#define SuplaWebPageControl_h

#if defined(SUPLA_BUTTON) || defined(SUPLA_LIMIT_SWITCH) || defined(SUPLA_MCP23017)
#define GUI_CONTROL
#endif

#ifdef GUI_CONTROL
#define PATH_CONTROL "control"
#define PATH_SWITCH  "switch"

#define PATH_BUTTON_SET          "setbutton"
#define PATH_BUTTON_SET_MCP23017 "setbuttonmcp"

#define INPUT_TRIGGER           "trs"
#define INPUT_BUTTON_SET        "bts"
#define INPUT_BUTTON_GPIO       "btg"
#define INPUT_BUTTON_LEVEL      "ibl"
#define INPUT_BUTTON_INVERSED   "ibi"
#define INPUT_BUTTON_EVENT      "icl"
#define INPUT_BUTTON_ACTION     "bta"
#define INPUT_LIMIT_SWITCH_GPIO "lsg"
#define INPUT_MAX_BUTTON        "mbt"
#define INPUT_MAX_LIMIT_SWITCH  "mls"
#endif

class SuplaWebPageControl {
 public:
#ifdef GUI_CONTROL
  void createWebPageControl();
#endif

#if defined(SUPLA_BUTTON) || defined(SUPLA_MCP23017)
  void handleControl();
  void handleControlSave();
  void supla_webpage_control(int save);
#endif

#ifdef SUPLA_LIMIT_SWITCH
  void handleLimitSwitch();
  void handleLimitSwitchSave();
  void suplaWebpageLimitSwitch(int save);
#endif

#if defined(SUPLA_BUTTON)
  void handleButtonSet();
  void handleButtonSaveSet();
  void supla_webpage_button_set(int save, int nr = 0);
#endif

#ifdef SUPLA_MCP23017
  void handleButtonSetMCP23017();
  void handleButtonSaveSetMCP23017();
  void supla_webpage_button_set_MCP23017(int save);
#endif
};

extern SuplaWebPageControl* WebPageControl;
#endif  // SuplaWebPageControl_h
