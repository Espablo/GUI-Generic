#include "SuplaWebPageControl.h"
#include "SuplaDeviceGUI.h"
#include "SuplaWebServer.h"
#include "GUIGenericCommon.h"
#include "Markup.h"

SuplaWebPageControl *WebPageControl = new SuplaWebPageControl();

void SuplaWebPageControl::createWebPageControl() {
  String path;
  path += PATH_START;
  path += PATH_CONTROL;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageControl::handleControl, this));
  path = PATH_START;
  path += PATH_SAVE_CONTROL;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageControl::handleControlSave, this));

#ifdef SUPLA_BUTTON
  if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_MCP23017).toInt() != FUNCTION_OFF) {
    path = PATH_START;
    path += PATH_BUTTON_SET;
    WebServer->httpServer.on(path, HTTP_GET, std::bind(&SuplaWebPageControl::handleButtonSetMCP23017, this));

    path = PATH_START;
    path += PATH_SAVE_BUTTON_SET;
    WebServer->httpServer.on(path, HTTP_POST, std::bind(&SuplaWebPageControl::handleButtonSaveSetMCP23017, this));
  }
  else {
    for (uint8_t i = 1; i <= ConfigManager->get(KEY_MAX_RELAY)->getValueInt(); i++) {
      path = PATH_START;
      path += PATH_BUTTON_SET;
      path += i;
      WebServer->httpServer.on(path, std::bind(&SuplaWebPageControl::handleButtonSet, this));

      path = PATH_START;
      path += PATH_SAVE_BUTTON_SET;
      path += i;
      WebServer->httpServer.on(path, std::bind(&SuplaWebPageControl::handleButtonSaveSet, this));
    }
  }
#endif

#ifdef SUPLA_LIMIT_SWITCH
  path = PATH_START;
  path += PATH_SWITCH;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageControl::handleLimitSwitch, this));
  path = PATH_START;
  path += PATH_SAVE_SWITCH;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageControl::handleLimitSwitchSave, this));
#endif
}

void SuplaWebPageControl::handleControl() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  supla_webpage_control(0);
}

void SuplaWebPageControl::handleControlSave() {
  //  Serial.println(F("HTTP_POST - metoda handleControlSave"));

  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  uint8_t nr, last_value;
#ifdef SUPLA_BUTTON
  last_value = ConfigManager->get(KEY_MAX_BUTTON)->getValueInt();
  for (nr = 1; nr <= last_value; nr++) {
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_MCP23017).toInt() != FUNCTION_OFF) {
      if (!WebServer->saveGpioMCP23017(INPUT_BUTTON_GPIO, FUNCTION_BUTTON, nr, INPUT_MAX_BUTTON)) {
        supla_webpage_control(6);
        return;
      }
    }
    else {
      if (!WebServer->saveGPIO(INPUT_BUTTON_GPIO, FUNCTION_BUTTON, nr, INPUT_MAX_BUTTON)) {
        supla_webpage_control(6);
        return;
      }
    }
  }

  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_BUTTON).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_BUTTON, WebServer->httpServer.arg(INPUT_MAX_BUTTON).c_str());
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      supla_webpage_control(1);
      break;
    case E_CONFIG_FILE_OPEN:
      supla_webpage_control(2);
      break;
  }
}

void SuplaWebPageControl::supla_webpage_control(int save) {
  uint8_t nr, countFreeGpio;

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_CONTROL);
  addForm(webContentBuffer, F("post"), PATH_SAVE_CONTROL);

#if (defined(SUPLA_BUTTON) && defined(SUPLA_RELAY)) || (defined(SUPLA_BUTTON) && defined(SUPLA_ROLLERSHUTTER))
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR_BUTTONS));

  if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_MCP23017).toInt() != FUNCTION_OFF) {
    countFreeGpio = 16;
  }
  else {
    countFreeGpio = ConfigESP->countFreeGpio(FUNCTION_BUTTON);
  }

  addNumberBox(webContentBuffer, INPUT_MAX_BUTTON, S_QUANTITY, KEY_MAX_BUTTON, countFreeGpio);

  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_BUTTON)->getValueInt(); nr++) {
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_MCP23017).toInt() != FUNCTION_OFF) {
      addListMCP23017GPIOLinkBox(webContentBuffer, INPUT_BUTTON_GPIO, S_BUTTON, FUNCTION_BUTTON, PATH_BUTTON_SET, nr);
    }
    else {
      addListGPIOLinkBox(webContentBuffer, INPUT_BUTTON_GPIO, S_BUTTON, FUNCTION_BUTTON, PATH_BUTTON_SET, nr);
    }
  }
  addFormHeaderEnd(webContentBuffer);
#endif

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);

  WebServer->sendContent();
}

#if (defined(SUPLA_BUTTON) && defined(SUPLA_RELAY)) || (defined(SUPLA_BUTTON) && defined(SUPLA_ROLLERSHUTTER))
void SuplaWebPageControl::handleButtonSet() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  supla_webpage_button_set(0);
}

void SuplaWebPageControl::handleButtonSaveSet() {
  //  Serial.println(F("HTTP_POST - metoda handleRelaySaveSet"));
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  String readUrl, nr_button, input, path;
  uint8_t place, key, gpio;

  input.reserve(5);
  readUrl.reserve(16);
  nr_button.reserve(2);
  path.reserve(16);

  path = PATH_START;
  path += PATH_SAVE_BUTTON_SET;
  readUrl = WebServer->httpServer.uri();

  place = readUrl.indexOf(path);
  nr_button = readUrl.substring(place + path.length(), place + path.length() + 3);

  gpio = ConfigESP->getGpio(nr_button.toInt(), FUNCTION_BUTTON);
  key = KEY_GPIO + gpio;

  input = INPUT_BUTTON_LEVEL;
  input += nr_button;
  ConfigManager->setElement(key, LEVEL, WebServer->httpServer.arg(input).toInt());

  input = INPUT_BUTTON_ACTION;
  input += nr_button;
  ConfigManager->setElement(key, ACTION, WebServer->httpServer.arg(input).toInt());

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      supla_webpage_control(1);
      break;
    case E_CONFIG_FILE_OPEN:
      supla_webpage_control(2);
      break;
  }
}

void SuplaWebPageControl::supla_webpage_button_set(int save) {
  String path, readUrl, nr_button;
  uint8_t place, selected;

  path.reserve(10);
  readUrl.reserve(11);
  nr_button.reserve(2);

  path = PATH_START;
  path += PATH_BUTTON_SET;
  readUrl = WebServer->httpServer.uri();

  place = readUrl.indexOf(path);
  nr_button = readUrl.substring(place + path.length(), place + path.length() + 3);

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_CONTROL);

  addForm(webContentBuffer, F("post"), PATH_SAVE_BUTTON_SET + nr_button);
  addFormHeader(webContentBuffer, S_BUTTON_NR_SETTINGS + nr_button);

  selected = ConfigESP->getLevel(nr_button.toInt(), FUNCTION_BUTTON);
  addListBox(webContentBuffer, INPUT_BUTTON_LEVEL + nr_button, S_REACTION_TO, TRIGGER_P, 3, selected);

  selected = ConfigESP->getAction(nr_button.toInt(), FUNCTION_BUTTON);
  addListBox(webContentBuffer, INPUT_BUTTON_ACTION + nr_button, S_ACTION, ACTION_P, 3, selected);

  addFormHeaderEnd(webContentBuffer);
  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_CONTROL);

  WebServer->sendContent();
}
#endif

#ifdef SUPLA_LIMIT_SWITCH
void SuplaWebPageControl::handleLimitSwitch() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  suplaWebpageLimitSwitch(0);
}

void SuplaWebPageControl::handleLimitSwitchSave() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  uint8_t nr, last_value;

  last_value = ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt();
  for (nr = 1; nr <= last_value; nr++) {
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_MCP23017).toInt() != FUNCTION_OFF) {
      if (!WebServer->saveGpioMCP23017(INPUT_LIMIT_SWITCH_GPIO, FUNCTION_LIMIT_SWITCH, nr, INPUT_MAX_LIMIT_SWITCH)) {
        suplaWebpageLimitSwitch(6);
        return;
      }
    }
    else {
      if (!WebServer->saveGPIO(INPUT_LIMIT_SWITCH_GPIO, FUNCTION_LIMIT_SWITCH, nr, INPUT_MAX_LIMIT_SWITCH)) {
        suplaWebpageLimitSwitch(6);
        return;
      }
    }
  }

  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_LIMIT_SWITCH).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_LIMIT_SWITCH, WebServer->httpServer.arg(INPUT_MAX_LIMIT_SWITCH).c_str());
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      suplaWebpageLimitSwitch(1);
      break;
    case E_CONFIG_FILE_OPEN:
      suplaWebpageLimitSwitch(2);
      break;
  }
}

void SuplaWebPageControl::suplaWebpageLimitSwitch(int save) {
  uint8_t nr, countFreeGpio;

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_SWITCH);
  addForm(webContentBuffer, F("post"), PATH_SAVE_SWITCH);

  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR_LIMIT_SWITCH));

  if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_MCP23017).toInt() != FUNCTION_OFF) {
    countFreeGpio = 16;
  }
  else {
    countFreeGpio = ConfigESP->countFreeGpio(FUNCTION_LIMIT_SWITCH);
  }

  addNumberBox(webContentBuffer, INPUT_MAX_LIMIT_SWITCH, S_QUANTITY, KEY_MAX_LIMIT_SWITCH, countFreeGpio);

  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt(); nr++) {
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_MCP23017).toInt() != FUNCTION_OFF) {
      addListMCP23017GPIOBox(webContentBuffer, INPUT_LIMIT_SWITCH_GPIO, S_LIMIT_SWITCH, FUNCTION_LIMIT_SWITCH, nr);
    }
    else {
      addListGPIOBox(webContentBuffer, INPUT_LIMIT_SWITCH_GPIO, S_LIMIT_SWITCH, FUNCTION_LIMIT_SWITCH, nr);
    }
  }
  addFormHeaderEnd(webContentBuffer);

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);

  WebServer->sendContent();
}
#endif

void SuplaWebPageControl::handleButtonSetMCP23017() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  supla_webpage_button_set_MCP23017(0);
}

void SuplaWebPageControl::supla_webpage_button_set_MCP23017(int save) {
  uint8_t selected;

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_CONTROL);

  addForm(webContentBuffer, F("post"), PATH_SAVE_BUTTON_SET);
  addFormHeader(webContentBuffer, F("Ustawienia dla przycisków"));

  selected = ConfigESP->getLevel(1, FUNCTION_BUTTON);
  addListBox(webContentBuffer, INPUT_BUTTON_LEVEL, S_REACTION_TO, TRIGGER_P, 3, selected);

  selected = ConfigESP->getAction(1, FUNCTION_BUTTON);
  addListBox(webContentBuffer, INPUT_BUTTON_ACTION, S_ACTION, ACTION_P, 3, selected);

  addFormHeaderEnd(webContentBuffer);
  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_CONTROL);

  WebServer->sendContent();
}

void SuplaWebPageControl::handleButtonSaveSetMCP23017() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  String input;
  uint8_t key, gpio, level, action;

  input.reserve(10);

  input = INPUT_BUTTON_LEVEL;
  level = WebServer->httpServer.arg(input).toInt();

  input = INPUT_BUTTON_ACTION;
  action = WebServer->httpServer.arg(input).toInt();

  for (uint8_t i = 1; i <= OFF_GPIO; i++) {
    gpio = ConfigESP->getGpioMCP23017(i, FUNCTION_BUTTON);
    if (gpio != OFF_GPIO) {
      key = KEY_GPIO + gpio;
      ConfigManager->setElement(key, LEVEL, level);
      ConfigManager->setElement(key, ACTION, action);
    }
  }
  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      supla_webpage_control(1);
      break;
    case E_CONFIG_FILE_OPEN:
      supla_webpage_control(2);
      break;
  }
}
