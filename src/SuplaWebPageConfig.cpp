#include "SuplaWebPageConfig.h"
#include "SuplaDeviceGUI.h"
#include "SuplaWebServer.h"
#include "SuplaCommonPROGMEM.h"
#include "GUIGenericCommon.h"
#include "Markup.h"

SuplaWebPageConfig *WebPageConfig = new SuplaWebPageConfig();

SuplaWebPageConfig::SuplaWebPageConfig() {
}

void SuplaWebPageConfig::createWebPageConfig() {
  String path;
  path = PATH_START;
  path += PATH_CONFIG;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageConfig::handleConfig, this));
  path = PATH_START;
  path += PATH_SAVE_CONFIG;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageConfig::handleConfigSave, this));
}

void SuplaWebPageConfig::handleConfig() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  WebServer->sendContent(supla_webpage_config(0));
}

void SuplaWebPageConfig::handleConfigSave() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  if (!WebServer->saveGPIO(INPUT_CFG_LED_GPIO, FUNCTION_CFG_LED)) {
    WebServer->sendContent(supla_webpage_config(6));
    return;
  }

  uint8_t key = KEY_GPIO + ConfigESP->getGpio(FUNCTION_CFG_LED);
  String input = INPUT_CFG_LED_LEVEL;
  ConfigManager->setElement(key, LEVEL, WebServer->httpServer.arg(input).toInt());

  if (!WebServer->saveGPIO(INPUT_CFG_BTN_GPIO, FUNCTION_CFG_BUTTON)) {
    WebServer->sendContent(supla_webpage_config(6));
    return;
  }

  if (strcmp(WebServer->httpServer.arg(INPUT_CFG_MODE).c_str(), "") != 0) {
    ConfigManager->set(KEY_CFG_MODE, WebServer->httpServer.arg(INPUT_CFG_MODE).c_str());
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      //      Serial.println(F("E_CONFIG_OK: Config save"));
      WebServer->sendContent(supla_webpage_config(1));
      break;
    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      WebServer->sendContent(supla_webpage_config(2));
      break;
  }
}

String SuplaWebPageConfig::supla_webpage_config(int save) {
  uint8_t selected, suported;
  String page = "";

  page += WebServer->SuplaSaveResult(save);
  page += WebServer->SuplaJavaScript(PATH_CONFIG);

  addForm(page, F("post"), PATH_SAVE_CONFIG);
  addFormHeader(page, S_GPIO_SETTINGS_FOR_CONFIG);
  addListGPIOBox(page, INPUT_CFG_LED_GPIO, F("LED"), FUNCTION_CFG_LED);

  selected = ConfigESP->getLevel(FUNCTION_CFG_LED);
  addListBox(page, INPUT_CFG_LED_LEVEL, S_STATE_CONTROL, LEVEL_P, 2, selected);
  addListGPIOBox(page, INPUT_CFG_BTN_GPIO, S_BUTTON, FUNCTION_CFG_BUTTON);

  selected = ConfigManager->get(KEY_CFG_MODE)->getValueInt();
  addListBox(page, INPUT_CFG_MODE, S_CFG_MODE, CFG_MODE_P, 2, selected);
  
  addFormHeaderEnd(page);
  addButtonSubmit(page, S_SAVE);
  addFormEnd(page);

  addButton(page, S_RETURN, PATH_DEVICE_SETTINGS);

  return page;
}
