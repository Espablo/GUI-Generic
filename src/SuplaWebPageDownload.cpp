#include "SuplaWebPageDownload.h"
#include "SuplaDeviceGUI.h"
#include "LittleFS.h"

void createWebDownload() {
  WebServer->httpServer.on(getURL(PATH_DOWNLOAD), handleDownload);
}

void handleDownload() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  File dataFile = LittleFS.open(F(CONFIG_FILE_PATH), "r");

  if (!dataFile) {
    return;
  }

  String str = F("attachment; filename=config_");
  str += ConfigManager->get(KEY_HOST_NAME)->getValue();
  str += Supla::Channel::reg_dev.SoftVer;
  str += '_';
  str += F(".dat");

  WebServer->httpServer.sendHeader(F("Content-Disposition"), str);
  WebServer->httpServer.streamFile(dataFile, F("application/octet-stream"));
  dataFile.close();
}