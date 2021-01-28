#include "SuplaWebPageTools.h"
#include "SuplaDeviceGUI.h"

void createWebTools() {
  WebServer->httpServer.on(getURL(PATH_TOOLS), handleTools);

  WebServer->httpServer.on(getURL(PATH_FACTORY_RESET), [&]() {
    if (ConfigESP->configModeESP == NORMAL_MODE) {
      if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
        return WebServer->httpServer.requestAuthentication();
    }
    WebServer->httpServer.sendHeader("Location", "/");
    WebServer->httpServer.send(303);
    WebServer->supla_webpage_start(0);
    ConfigESP->factoryReset(true);
  });
}

void handleTools() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  addFormHeader(webContentBuffer, F("Tools"));
  //#ifdef SUPLA_BUTTON
  addButton(webContentBuffer, F("Save config"), PATH_DOWNLOAD);
  //#endif
  //#ifdef SUPLA_BUTTON
  addButton(webContentBuffer, F("Load config"), PATH_UPLOAD);
  //#endif
#ifdef SUPLA_OTA
  addButton(webContentBuffer, S_UPDATE, PATH_UPDATE_HENDLE);
#endif
  addButton(webContentBuffer, F("Factory reset"), PATH_FACTORY_RESET);
  addFormHeaderEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, "");

  WebServer->sendContent();
}