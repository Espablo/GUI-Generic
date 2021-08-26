#ifndef __HTTP_UPDATE_SERVER_ESP32_H
#define __HTTP_UPDATE_SERVER_ESP32_H

#include <WebServer.h>
#include <Update.h>

#define PATH_UPDATE_HENDLE "update"
#define PATH_UPDATE        "/updateOTA"

/*
 * Login page
 */
#define ESP32_WEB_UPDATE_HTML                                                                                                      \
  "<html><body><form method='POST' action='' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' " \
  "value='Update'></form></body></html>"
#define ESP32_WEB_UPDATE_SUCCESS_RESPONSE "<META http-equiv=\"refresh\" content=\"10;URL=/\">Update Success! Rebooting...\n"

class ESP32HTTPUpdateServer {
 public:
  ESP32HTTPUpdateServer() {
    _server = nullptr;
    _username = "";
    _password = "";
  };

  void setup(ESP32WebServer* server) {
    setup(server, emptyString, emptyString);
  }

  void setup(ESP32WebServer* server, const String& path) {
    setup(server, path, emptyString, emptyString);
  }

  void setup(ESP32WebServer* server, const String& username, const String& password) {
    setup(server, PATH_UPDATE, username, password);
  }

  void setup(ESP32WebServer* server, const String& path, const String& username, const String& password) {
    _server = server;
    _username = username;
    _password = password;

    //_server->on(getURL(PATH_UPDATE_HENDLE), std::bind(&handleFirmwareUp, this));

    _server->on(path.c_str(), HTTP_GET, [&]() {
      if (_username != emptyString && _password != emptyString && !_server->authenticate(_username.c_str(), _password.c_str()))
        return _server->requestAuthentication();

      _server->sendHeader("Connection", "close");
      _server->send(200, "text/html", ESP32_WEB_UPDATE_HTML);
    });

    /*handling uploading firmware file */
    _server->on(
        path.c_str(), HTTP_POST,
        [&]() {
          _server->client().setNoDelay(true);
          _server->sendHeader("Connection", "close");
          _server->send(200, "text/plain", (Update.hasError()) ? "FAIL" : ESP32_WEB_UPDATE_SUCCESS_RESPONSE);
          delay(100);
          _server->client().stop();
          ESP.restart();
        },
        [&]() {
          HTTPUpload& upload = _server->upload();
          if (upload.status == UPLOAD_FILE_START) {
            Serial.printf("Update: %s\n", upload.filename.c_str());
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {  // start with max available size
              Update.printError(Serial);
            }
          }
          else if (upload.status == UPLOAD_FILE_WRITE) {
            /* flashing firmware to ESP*/
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
              Update.printError(Serial);
            }
          }
          else if (upload.status == UPLOAD_FILE_END) {
            if (Update.end(true)) {  // true to set the size to the current progress
              Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
            }
            else {
              Update.printError(Serial);
            }
          }
        });

    _server->begin();
  }

  void updateCredentials(const String& username, const String& password) {
    _username = username;
    _password = password;
  }

 private:
  ESP32WebServer* _server;
  String _username;
  String _password;
  bool _authenticated;
  String _updaterError;
};

#endif
