#ifndef __HTTP_UPDATE_SERVER_ESP32_H
#define __HTTP_UPDATE_SERVER_ESP32_H

#ifdef ARDUINO_ARCH_ESP32
#include <WebServer.h>
#include <supla/ESP32WebServer/ESP32WebServer.h>
#include <Update.h>

#define PATH_UPDATE_HENDLE "update"
#define PATH_UPDATE        "/updateOTA"

class ESP32HTTPUpdateServer {
 public:
  ESP32HTTPUpdateServer();

  void setup(ESP32WebServer* server) {
    setup(server, emptyString, emptyString);
  }

  void setup(ESP32WebServer* server, const String& path) {
    setup(server, path, emptyString, emptyString);
  }

  void setup(ESP32WebServer* server, const String& username, const String& password) {
    setup(server, PATH_UPDATE, username, password);
  }

  void setup(ESP32WebServer* server, const String& path, const String& username, const String& password);

  void updateCredentials(const String& username, const String& password) {
    _username = username;
    _password = password;
  }

 private:
  ESP32WebServer* _server;
  String _username;
  String _password;
  bool _authenticated;

  void handleFirmwareUp();
  void suplaWebPageUpddate();
};

#endif
#endif