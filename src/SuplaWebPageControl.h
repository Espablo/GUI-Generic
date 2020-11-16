#ifndef SuplaWebPageControl_h
#define SuplaWebPageControl_h

#include "SuplaWebServer.h"
#include "SuplaDeviceGUI.h"

#define  PATH_CONTROL               "control"
#define  PATH_SAVE_CONTROL          "savecontrol"
#define  PATH_BUTTON_SET            "setbutton"
#define  PATH_SAVE_BUTTON_SET       "savesetbutton"
#define  INPUT_TRIGGER              "trs"
#define  INPUT_BUTTON_SET           "bts"
#define  INPUT_BUTTON_GPIO          "btg"
#define  INPUT_BUTTON_LEVEL         "icl"
#define  INPUT_LIMIT_SWITCH_GPIO    "lsg"
#define  INPUT_MAX_BUTTON           "mbt"
#define  INPUT_MAX_LIMIT_SWITCH     "mls"

/*enum _trigger_button {
  TRIGGER_PRESS,
  TRIGGER_RELEASE,
  TRIGGER_CHANGE
};*/

class SuplaWebPageControl {
 public:
  void createWebPageControl();
  void handleControl();
  void handleControlSave();

#if (defined(SUPLA_BUTTON) && defined(SUPLA_RELAY)) || (defined(SUPLA_RSUPLA_BUTTONELAY) || defined(SUPLA_ROLLERSHUTTER))
  void handleButtonSet();
  void handleButtonSaveSet();
#endif

 private:
  String supla_webpage_control(int save);

#ifdef SUPLA_BUTTON
  String supla_webpage_button_set(int save);
#endif

};

extern SuplaWebPageControl* WebPageControl;
#endif  // SuplaWebPageControl_h
