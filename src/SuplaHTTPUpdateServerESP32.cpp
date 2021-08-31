#ifdef ARDUINO_ARCH_ESP32
#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

#include <WiFiUdp.h>

#include <FS.h>
#include "StreamString.h"

#include "SuplaHTTPUpdateServerESP32.h"
#include "SuplaDeviceGUI.h"

static const char serverIndex[] PROGMEM =
    R"(<!DOCTYPE html>
     <html lang='{l}'>
     <head>
         <meta charset='utf-8'>
         <meta name='viewport' content='width=device-width,initial-scale=1'/>
     </head>
     <body>
     <div>{M}: {m}kB</div>
     <div>{U}: {u}kB</div>
     <div>{S}: {s}kB</div>
     <form method='POST' action='' enctype='multipart/form-data'>
         <input type='file' accept='.bin,.bin.gz' name='firmware'>
         </br>
         <input type='submit' value='{b}'>
     </form>
     </body>
     </html>)";

static const char successResponse[] PROGMEM = "<META http-equiv='refresh' content='10'>{m}";

ESP32HTTPUpdateServer::ESP32HTTPUpdateServer() {
  _server = NULL;
  _username = emptyString;
  _password = emptyString;
  _authenticated = false;
}

void ESP32HTTPUpdateServer::setup(ESP32WebServer* server, const String& path, const String& username, const String& password) {
  _server = server;
  _username = username;
  _password = password;

  _server->on(getURL(PATH_UPDATE_HENDLE), std::bind(&ESP32HTTPUpdateServer::handleFirmwareUp, this));
  // handler for the /update form page
  _server->on(path.c_str(), HTTP_GET, [&]() {
    if (_username != emptyString && _password != emptyString && !_server->authenticate(_username.c_str(), _password.c_str()))
      return _server->requestAuthentication();

    String index = FPSTR(serverIndex);
    index.replace("{l}", S_LANG);
    index.replace("{m}", String(ESP.getFlashChipSize() / 1024));
    index.replace("{s}", String(ESP.getFreeSketchSpace() / 1024));
    index.replace("{u}", String(ESP.getSketchSize() / 1024));
    index.replace("{M}", S_SKETCH_MEMORY_SIZE);
    index.replace("{S}", S_SKETCH_UPLOAD_MAX_SIZE);
    index.replace("{U}", S_SKETCH_LOADED_SIZE);
    index.replace("{b}", S_UPDATE_FIRMWARE);
    _server->send(200, PSTR("text/html"), index.c_str());
  });

  // handler for the /update form POST (once file upload finishes)
  _server->on(
      path.c_str(), HTTP_POST,
      [&]() {
        if (!_authenticated)
          return _server->requestAuthentication();
        if (Update.hasError()) {
          _server->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        }
        else {
          String succes = FPSTR(successResponse);
          succes.replace("{m}", S_UPDATE_SUCCESS_REBOOTING);
          _server->client().setNoDelay(true);
          _server->send(200, F("text/html"), succes.c_str());
          delay(100);
          _server->client().stop();
          ESP.restart();
        }
      },
      [&]() {
        HTTPUpload& upload = _server->upload();
        if (upload.status == UPLOAD_FILE_START) {
          _authenticated = (_username == emptyString || _password == emptyString || _server->authenticate(_username.c_str(), _password.c_str()));
          if (!_authenticated) {
            return;
          }
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
        delay(0);
      });
}

void ESP32HTTPUpdateServer::handleFirmwareUp() {
  if (!WebServer->isLoggedIn()) {
    return;
  }
  suplaWebPageUpddate();
}

void ESP32HTTPUpdateServer::suplaWebPageUpddate() {
  webContentBuffer += SuplaJavaScript();
  webContentBuffer += F("<div class='w'>");
  webContentBuffer += F("<h3>");
  webContentBuffer += S_SOFTWARE_UPDATE;
  webContentBuffer += F("</h3>");
  webContentBuffer += F("<br>");
  webContentBuffer += F("<center>");
  webContentBuffer += F("<iframe src='");
  webContentBuffer += PATH_UPDATE;
  webContentBuffer += F("' frameborder='0' height='200'></iframe>");
  webContentBuffer += F("</center>");
  webContentBuffer += F("</div>");
  webContentBuffer += F("<a href='");
  webContentBuffer += PATH_TOOLS;
  webContentBuffer += F("'><button>");
  webContentBuffer += S_RETURN;
  webContentBuffer += F("</button></a><br><br>");
  WebServer->sendContent();
}
#endif