#ifndef SuplaWebPageControl_h
#define SuplaWebPageControl_h

#include "SuplaWebServer.h"
#include "SuplaDeviceGUI.h"

#define  PATH_CONTROL               "control"
#define  PATH_SAVE_CONTROL          "savecontrol"
#define  INPUT_TRIGGER              "trs"
#define  INPUT_BUTTON_SET           "bts"
#define  INPUT_BUTTON_GPIO          "btg"
#define  INPUT_LIMIT_SWITCH_GPIO    "lsg"
#define  INPUT_MAX_BUTTON           "mbt"
#define  INPUT_MAX_LIMIT_SWITCH     "mls"


class SuplaWebPageControl {
  public:
    SuplaWebPageControl();
    void createWebPageControl();
    void handleControl();
    void handleControlSave();
    
  private:
    String supla_webpage_control(int save);

    const char* Supported_Button[2] = {
      "MONOSTABILNY",
      "BISTABILNY"
    };

    const char* Supported_MonostableTrigger[2] = {
      "ZWOLNIENIE",
      "WCIŚNIĘCIE"
    };

};

extern SuplaWebPageControl *WebPageControl;
#endif // SuplaWebPageControl_h
