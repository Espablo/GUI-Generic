// https://github.com/espressif/arduino-esp32/issues/5080
#ifndef ESP32WEBSERVER_H
#define ESP32WEBSERVER_H

#include <WebServer.h>
class ESP32WebServer : public WebServer {
 public:
  ESP32WebServer(int port = 80) : WebServer(port) {
  }

  void send_P(int code, PGM_P content_type, PGM_P content) {
    size_t contentLength = 0;

    if (content != NULL) {
      contentLength = strlen_P(content);
    }

    String header;
    char type[64];
    memccpy_P((void*)type, (PGM_VOID_P)content_type, 0, sizeof(type));
    _prepareHeader(header, code, (const char*)type, contentLength);
    _currentClientWrite(header.c_str(), header.length());
    if (contentLength) {  // if rajouté par FS ...........................+++++
      sendContent_P(content);
    }
  }

  bool chunkedResponseModeStart_P(int code, PGM_P content_type) {
    if (_currentVersion == 0)
      // no chunk mode in HTTP/1.0
      return false;
    setContentLength(CONTENT_LENGTH_UNKNOWN);
    send_P(code, content_type, "");
    return true;
  }
  bool chunkedResponseModeStart(int code, const char* content_type) {
    return chunkedResponseModeStart_P(code, content_type);
  }
  bool chunkedResponseModeStart(int code, const String& content_type) {
    return chunkedResponseModeStart_P(code, content_type.c_str());
  }
  void chunkedResponseFinalize() {
    sendContent(emptyString);
  }
};
#endif  // ESP32WEBSERVER_H