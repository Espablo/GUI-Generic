#include "SuplaWebPageConfig.h"

#ifdef SUPLA_CONFIG
void createWebPageConfig() {
  WebServer->httpServer->on(getURL(PATH_CONFIG), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handleConfig();
    else
      handleConfigSave();
  });
}

void handleConfigSave() {
  if (!WebServer->saveGPIO(INPUT_CFG_LED_GPIO, FUNCTION_CFG_LED)) {
    handleConfig(6);
    return;
  }

  uint8_t key = KEY_GPIO + ConfigESP->getGpio(FUNCTION_CFG_LED);
  String input = INPUT_CFG_LED_LEVEL;
  ConfigManager->setElement(key, LEVEL_RELAY, WebServer->httpServer->arg(input).toInt());

  if (!WebServer->saveGPIO(INPUT_CFG_BTN_GPIO, FUNCTION_CFG_BUTTON)) {
    handleConfig(6);
    return;
  }

  if (strcmp(WebServer->httpServer->arg(INPUT_CFG_MODE).c_str(), "") != 0)
    ConfigManager->set(KEY_CFG_MODE, WebServer->httpServer->arg(INPUT_CFG_MODE).c_str());
  if (strcmp(WebServer->httpServer->arg(INPUT_CFG_SSL).c_str(), "") != 0)
    ConfigManager->set(KEY_ENABLE_SSL, WebServer->httpServer->arg(INPUT_CFG_SSL).c_str());
  if (strcmp(WebServer->httpServer->arg(INPUT_CFG_AVAILABLE_GUI).c_str(), "") != 0)
    ConfigManager->set(KEY_ENABLE_GUI, WebServer->httpServer->arg(INPUT_CFG_AVAILABLE_GUI).c_str());

#ifdef SUPLA_DEEP_SLEEP
  if (strcmp(WebServer->httpServer->arg(INPUT_DEEP_SLEEP_TIME).c_str(), "") != 0)
    ConfigManager->set(KEY_DEEP_SLEEP_TIME, WebServer->httpServer->arg(INPUT_DEEP_SLEEP_TIME).c_str());

#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleConfig(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleConfig(2);
      break;
  }
}

void handleConfig(int save) {
  uint8_t selected;

  WebServer->sendHeaderStart();
  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_CONFIG);

  addForm(webContentBuffer, F("post"), PATH_CONFIG);
  addFormHeader(webContentBuffer, S_GPIO_SETTINGS_FOR_CONFIG);
  addListGPIOBox(webContentBuffer, INPUT_CFG_LED_GPIO, F("LED"), FUNCTION_CFG_LED);

  selected = ConfigESP->getLevel(ConfigESP->getGpio(FUNCTION_CFG_LED));
  addListBox(webContentBuffer, INPUT_CFG_LED_LEVEL, S_STATE_CONTROL, LEVEL_P, 2, selected);

  addListGPIOBox(webContentBuffer, INPUT_CFG_BTN_GPIO, S_BUTTON, FUNCTION_CFG_BUTTON);

  selected = ConfigManager->get(KEY_CFG_MODE)->getValueInt();
  addListBox(webContentBuffer, INPUT_CFG_MODE, S_CFG_MODE, CFG_MODE_P, 2, selected);

  addFormHeaderEnd(webContentBuffer);

  addFormHeader(webContentBuffer, F("Połączenie SSL"));
  selected = ConfigManager->get(KEY_ENABLE_SSL)->getValueInt();
  addListBox(webContentBuffer, INPUT_CFG_SSL, F("SSL"), STATE_P, 2, selected);
  addFormHeaderEnd(webContentBuffer);

  addFormHeader(webContentBuffer, F("GUI podczas normalnej pracy"));
  selected = ConfigManager->get(KEY_ENABLE_GUI)->getValueInt();
  addListBox(webContentBuffer, INPUT_CFG_AVAILABLE_GUI, F("GUI"), STATE_P, 2, selected);
  addFormHeaderEnd(webContentBuffer);

#ifdef SUPLA_DEEP_SLEEP
  addFormHeader(webContentBuffer, String(S_SETTING_FOR) + S_SPACE + "uśpienia");
  addNumberBox(webContentBuffer, INPUT_DEEP_SLEEP_TIME, F("Czas uśpienia[min]"), KEY_DEEP_SLEEP_TIME, 999);
  addFormHeaderEnd(webContentBuffer);
#endif

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);

  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);
  WebServer->sendHeaderEnd();
}
#endif
