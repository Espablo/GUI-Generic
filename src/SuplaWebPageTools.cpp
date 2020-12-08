#include "SuplaWebPageTools.h"
#include "SuplaDeviceGUI.h"

void createWebTools() {
  WebServer->httpServer.on(getURL(PATH_TOOLS), handleTools);
}

void handleTools() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  String content = "";
  addFormHeader(content, "Tools");
  //#ifdef SUPLA_BUTTON
  addButton(content, "Save config", PATH_DOWNLOAD);
  //#endif
  //#ifdef SUPLA_BUTTON
  addButton(content, "Load config", PATH_UPLOAD);
  //#endif
#ifdef SUPLA_OTA
  addButton(content, S_UPDATE, PATH_UPDATE_HENDLE);
#endif
  addFormHeaderEnd(content);
  addButton(content, S_RETURN, "/");

  WebServer->sendContent(content);
}