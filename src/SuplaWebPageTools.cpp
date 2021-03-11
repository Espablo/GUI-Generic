#include "SuplaWebPageTools.h"
#include "SuplaDeviceGUI.h"

void createWebTools() {
  WebServer->httpServer->on(getURL(PATH_TOOLS), HTTP_GET, [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    String sCommand = WebServer->httpServer->arg(REQ_PARM_URL);

    if (strcasecmp_P(sCommand.c_str(), PATH_DOWNLOAD) == 0) {
      handleDownload();
    }
    else if (strcasecmp_P(sCommand.c_str(), PATH_UPLOAD) == 0) {
      handleUpload();
    }
    else if (strcasecmp_P(sCommand.c_str(), PATH_RESET) == 0) {
      WebServer->httpServer->sendHeader("Location", "/");
      WebServer->httpServer->send(303);
      WebServer->supla_webpage_start(0);
      ConfigESP->reset(true);
    }
    else if (strcasecmp_P(sCommand.c_str(), PATH_FACTORY_RESET) == 0) {
      WebServer->httpServer->sendHeader("Location", "/");
      WebServer->httpServer->send(303);
      WebServer->supla_webpage_start(0);
      ConfigESP->factoryReset(true);
    }
    else {
      handleTools();
    }
  });
}

void handleTools() {
  addFormHeader(webContentBuffer, S_TOOLS);
  //#ifdef SUPLA_BUTTON
  addButton(webContentBuffer, S_SAVE_CONFIGURATION, getParameterRequest(PATH_TOOLS, PATH_DOWNLOAD));
  //#endif
  //#ifdef SUPLA_BUTTON
  addButton(webContentBuffer, S_LOAD_CONFIGURATION, getParameterRequest(PATH_TOOLS, PATH_UPLOAD));
  //#endif
  //#ifdef SUPLA_BUTTON
  addButton(webContentBuffer, S_RESET_CONFIGURATION, getParameterRequest(PATH_TOOLS, PATH_RESET));
  //#endif
#ifdef SUPLA_OTA
  addButton(webContentBuffer, S_UPDATE, PATH_UPDATE_HENDLE);
#endif
  addButton(webContentBuffer, S_RESTORE_FACTORY_SETTING, getParameterRequest(PATH_TOOLS, PATH_FACTORY_RESET));
  addFormHeaderEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, "");

  WebServer->sendContent();
}
