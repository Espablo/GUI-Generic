#include "SuplaWebPageHome.h"

void createWebPageHome() {
  WebServer->httpServer->on(PATH_START, [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (strcmp(WebServer->httpServer->arg(PATH_REBOT).c_str(), "1") == 0) {
      WebServer->httpServer->sendHeader("Location", PATH_START, true);
      handlePageHome(2);
      ConfigESP->rebootESP();
      return;
    }

    if (strcmp(WebServer->httpServer->arg(PATH_REBOT_CREATOR).c_str(), "1") == 0) {
      WebServer->httpServer->sendHeader("Location", PATH_START, true);
      handlePageHome(2);
      ConfigESP->rebootESP();
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handlePageHome();
    else
      handlePageHomeSave();
  });
}

void handlePageHome(int save) {
  WebServer->sendHeaderStart();

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript();

  addForm(webContentBuffer, F("post"));
  addFormHeader(webContentBuffer, S_SETTING_WIFI_SSID);
  addTextBox(webContentBuffer, INPUT_WIFI_SSID, S_WIFI_SSID, KEY_WIFI_SSID, 0, MAX_SSID, true);
  addTextBoxPassword(webContentBuffer, INPUT_WIFI_PASS, S_WIFI_PASS, KEY_WIFI_PASS, MIN_PASSWORD, MAX_PASSWORD, true);
  addTextBox(webContentBuffer, INPUT_HOSTNAME, S_HOST_NAME, KEY_HOST_NAME, 0, MAX_HOSTNAME, true);
  addFormHeaderEnd(webContentBuffer);

  addFormHeader(webContentBuffer, S_SETTING_SUPLA);
  addTextBox(webContentBuffer, INPUT_SERVER, S_SUPLA_SERVER, KEY_SUPLA_SERVER, DEFAULT_SERVER, 0, MAX_SUPLA_SERVER, true);
  addTextBox(webContentBuffer, INPUT_EMAIL, S_SUPLA_EMAIL, KEY_SUPLA_EMAIL, DEFAULT_EMAIL, 0, MAX_EMAIL, true);
  addFormHeaderEnd(webContentBuffer);

  addFormHeader(webContentBuffer, S_SETTING_ADMIN);
  addTextBox(webContentBuffer, INPUT_MODUL_LOGIN, S_LOGIN, KEY_LOGIN, 0, MAX_MLOGIN, true);
  addTextBoxPassword(webContentBuffer, INPUT_MODUL_PASS, S_LOGIN_PASS, KEY_LOGIN_PASS, MIN_PASSWORD, MAX_MPASSWORD, true);
  addFormHeaderEnd(webContentBuffer);

#ifdef SUPLA_ROLLERSHUTTER
  uint8_t maxrollershutter = ConfigManager->get(KEY_MAX_RELAY)->getValueInt();
  if (maxrollershutter >= 2) {
    addFormHeader(webContentBuffer, S_ROLLERSHUTTERS);
    addNumberBox(webContentBuffer, INPUT_ROLLERSHUTTER, S_QUANTITY, KEY_MAX_ROLLERSHUTTER, (maxrollershutter / 2));
    addFormHeaderEnd(webContentBuffer);
  }
#endif

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);

  addButton(webContentBuffer, S_DEVICE_SETTINGS, PATH_DEVICE_SETTINGS);
  addButton(webContentBuffer, S_TOOLS, PATH_TOOLS);

  WebServer->sendHeaderEnd();
}

void handlePageHomeSave() {
  if (strcmp(WebServer->httpServer->arg(INPUT_WIFI_SSID).c_str(), "") != 0)
    ConfigManager->set(KEY_WIFI_SSID, WebServer->httpServer->arg(INPUT_WIFI_SSID).c_str());
  if (strcmp(WebServer->httpServer->arg(INPUT_WIFI_PASS).c_str(), "") != 0)
    ConfigManager->set(KEY_WIFI_PASS, WebServer->httpServer->arg(INPUT_WIFI_PASS).c_str());
  if (strcmp(WebServer->httpServer->arg(INPUT_SERVER).c_str(), "") != 0)
    ConfigManager->set(KEY_SUPLA_SERVER, WebServer->httpServer->arg(INPUT_SERVER).c_str());
  if (strcmp(WebServer->httpServer->arg(INPUT_EMAIL).c_str(), "") != 0)
    ConfigManager->set(KEY_SUPLA_EMAIL, WebServer->httpServer->arg(INPUT_EMAIL).c_str());
  if (strcmp(WebServer->httpServer->arg(INPUT_HOSTNAME).c_str(), "") != 0)
    ConfigManager->set(KEY_HOST_NAME, WebServer->httpServer->arg(INPUT_HOSTNAME).c_str());
  if (strcmp(WebServer->httpServer->arg(INPUT_MODUL_LOGIN).c_str(), "") != 0)
    ConfigManager->set(KEY_LOGIN, WebServer->httpServer->arg(INPUT_MODUL_LOGIN).c_str());
  if (strcmp(WebServer->httpServer->arg(INPUT_MODUL_PASS).c_str(), "") != 0)
    ConfigManager->set(KEY_LOGIN_PASS, WebServer->httpServer->arg(INPUT_MODUL_PASS).c_str());

#ifdef SUPLA_ROLLERSHUTTER
  if (strcmp(WebServer->httpServer->arg(INPUT_ROLLERSHUTTER).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_ROLLERSHUTTER, WebServer->httpServer->arg(INPUT_ROLLERSHUTTER).toInt());
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      if (ConfigESP->configModeESP == NORMAL_MODE) {
        handlePageHome(1);
      }
      else {
        handlePageHome(7);
      }
      break;

    case E_CONFIG_FILE_OPEN:
      handlePageHome(2);
      break;
  }
}