#ifndef __SUPLA_HTTP_UPDATE_SERVER_H
#define __SUPLA_HTTP_UPDATE_SERVER_H

#ifdef ARDUINO_ARCH_ESP8266
#include <Arduino.h>

#define PATH_UPDATE_HENDLE       "update"
#define PATH_UPDATE_HENDLE_2STEP "u2s"
#define PATH_UPDATE_URL          "u2u"
#define INPUT_UPDATE_URL         "iuu"
#define PATH_UPDATE              "updateOTA"

class ESP8266HTTPUpdateServer {
 public:
  ESP8266HTTPUpdateServer(bool serial_debug = false);

  void setup();

 protected:
  void setUpdaterError();
  void successUpdateManualRefresh();

 private:
  bool _serial_output;
  String _updaterError;

  void handleFirmwareUp();
  void suplaWebPageUpddate(int save = 0);

  void updateManual();
};
#endif

#endif
