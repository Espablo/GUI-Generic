#ifndef __SUPLA_HTTP_UPDATE_SERVER_H
#define __SUPLA_HTTP_UPDATE_SERVER_H

#include <Arduino.h>

#define PATH_UPDATE_HENDLE       "update"
#define PATH_UPDATE_HENDLE_2STEP "u2s"
#define PATH_UPDATE_URL          "u2u"
#define PATH_UPDATE_BUILDER      "u2b"
#define INPUT_UPDATE_URL         "iuu"
#define PATH_UPDATE              "updateOTA"

#define HOST_BUILDER "http://gui-generic-builder.supla.io/"

class HTTPUpdateServer {
 public:
  HTTPUpdateServer(bool serial_debug = false);

  void setup();

 protected:
  void successUpdateManualRefresh();

 private:
  bool _serial_output;
  String _updaterError;

  String getUpdateBuilderUrl();

  void handleFirmwareUp();
  void suplaWebPageUpddate(int save = 0, const String& location = PATH_UPDATE_HENDLE);

#ifdef ARDUINO_ARCH_ESP8266
  void setUpdaterError();
#endif
  void updateManual();
};
#endif
