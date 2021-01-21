#include "SuplaWebPageStatusLed.h"
#include "SuplaDeviceGUI.h"
#include "SuplaWebServer.h"
#include "GUIGenericCommon.h"
#include "Markup.h"

void createWebStatusLed() {
  WebServer->httpServer.on(getURL(PATH_LED), handleStatusLed);
  WebServer->httpServer.on(getURL(PATH_SAVE_LED), handleStatusLedSave);
}

void handleStatusLed() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  webStatusLed(0);
}

void handleStatusLedSave() {
  uint8_t nr;

  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_RELAY)->getValueInt(); nr++) {
    if (!WebServer->saveGPIO(INPUT_LED, FUNCTION_LED, nr)) {
      webStatusLed(6);
      return;
    }
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      webStatusLed(1);
      break;
    case E_CONFIG_FILE_OPEN:
      webStatusLed(2);
      break;
  }
}

void webStatusLed(int save) {
  uint8_t nr;

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_LED);
  addForm(webContentBuffer, F("post"), PATH_SAVE_LED);

  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + F(" LED"));

  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_RELAY)->getValueInt(); nr++) {
    addListGPIOBox(webContentBuffer, INPUT_LED, F("LED"), FUNCTION_LED, nr);
  }

  addFormHeaderEnd(webContentBuffer);

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);

  WebServer->sendContent();
}