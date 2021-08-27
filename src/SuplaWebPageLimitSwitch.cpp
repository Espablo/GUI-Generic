#include "SuplaWebPageLimitSwitch.h"

#ifdef SUPLA_LIMIT_SWITCH
void createWebPageLimitSwitch() {
  WebServer->httpServer->on(getURL(PATH_SWITCH), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handleLimitSwitch();
    else
      handleLimitSwitchSave();
  });
}

void handleLimitSwitch(int save) {
  uint8_t nr;

  WebServer->sendHeaderStart();

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_SWITCH);

  addForm(webContentBuffer, F("post"), PATH_SWITCH);
  addFormHeader(webContentBuffer, S_GPIO_SETTINGS_FOR_LIMIT_SWITCH);
  addNumberBox(webContentBuffer, INPUT_MAX_LIMIT_SWITCH, S_QUANTITY, KEY_MAX_LIMIT_SWITCH, ConfigESP->countFreeGpio(FUNCTION_LIMIT_SWITCH));

  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt(); nr++) {
#ifdef SUPLA_MCP23017
    if (ConfigESP->checkActiveMCP23017(FUNCTION_LIMIT_SWITCH)) {
      addListMCP23017GPIOBox(webContentBuffer, INPUT_LIMIT_SWITCH_GPIO, S_LIMIT_SWITCH, FUNCTION_LIMIT_SWITCH, nr);
    }
    else {
      addListGPIOBox(webContentBuffer, INPUT_LIMIT_SWITCH_GPIO, S_LIMIT_SWITCH, FUNCTION_LIMIT_SWITCH, nr);
    }
#else
    addListGPIOBox(webContentBuffer, INPUT_LIMIT_SWITCH_GPIO, S_LIMIT_SWITCH, FUNCTION_LIMIT_SWITCH, nr);
#endif
  }
  addFormHeaderEnd(webContentBuffer);

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);

  WebServer->sendHeaderEnd();
}

void handleLimitSwitchSave() {
  uint8_t nr, last_value;

  last_value = ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt();
  for (nr = 1; nr <= last_value; nr++) {
#ifdef SUPLA_MCP23017
    if (ConfigESP->checkActiveMCP23017(FUNCTION_LIMIT_SWITCH)) {
      if (!WebServer->saveGpioMCP23017(INPUT_LIMIT_SWITCH_GPIO, FUNCTION_LIMIT_SWITCH, nr, INPUT_MAX_LIMIT_SWITCH)) {
        handleLimitSwitch(6);
        return;
      }
    }
    else {
      if (!WebServer->saveGPIO(INPUT_LIMIT_SWITCH_GPIO, FUNCTION_LIMIT_SWITCH, nr, INPUT_MAX_LIMIT_SWITCH)) {
        handleLimitSwitch(6);
        return;
      }
    }
#else
    if (!WebServer->saveGPIO(INPUT_LIMIT_SWITCH_GPIO, FUNCTION_LIMIT_SWITCH, nr, INPUT_MAX_LIMIT_SWITCH)) {
      handleLimitSwitch(6);
      return;
    }
#endif
  }

  if (strcmp(WebServer->httpServer->arg(INPUT_MAX_LIMIT_SWITCH).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_LIMIT_SWITCH, WebServer->httpServer->arg(INPUT_MAX_LIMIT_SWITCH).c_str());
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleLimitSwitch(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleLimitSwitch(2);
      break;
  }
}
#endif