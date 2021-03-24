#include "SuplaWebPageControl.h"
#include "SuplaDeviceGUI.h"
#include "SuplaWebServer.h"
#include "GUIGenericCommon.h"
#include "Markup.h"

SuplaWebPageControl *WebPageControl = new SuplaWebPageControl();

#if defined(SUPLA_BUTTON) || defined(SUPLA_LIMIT_SWITCH) || defined(SUPLA_MCP23017)
void SuplaWebPageControl::createWebPageControl() {
#if defined(SUPLA_BUTTON) || defined(SUPLA_MCP23017)
  WebServer->httpServer->on(getURL(PATH_CONTROL), std::bind(&SuplaWebPageControl::handleControl, this));
#endif

  if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt() != FUNCTION_OFF) {
#ifdef SUPLA_MCP23017
    WebServer->httpServer->on(getURL(PATH_BUTTON_SET_MCP23017), std::bind(&SuplaWebPageControl::handleButtonSetMCP23017, this));
#endif
  }
  else {
#if defined(SUPLA_BUTTON)
    WebServer->httpServer->on(getURL(PATH_BUTTON_SET), std::bind(&SuplaWebPageControl::handleButtonSet, this));
#endif
  }

#ifdef SUPLA_LIMIT_SWITCH
  WebServer->httpServer->on(getURL(PATH_SWITCH), std::bind(&SuplaWebPageControl::handleLimitSwitch, this));
#endif
}
#endif

#if defined(SUPLA_BUTTON) || defined(SUPLA_MCP23017)
void SuplaWebPageControl::handleControl() {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  if (WebServer->httpServer->method() == HTTP_GET)
    supla_webpage_control(0);
  else
    handleControlSave();
}

void SuplaWebPageControl::handleControlSave() {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  uint8_t nr;

  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_BUTTON)->getValueInt(); nr++) {
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt() != FUNCTION_OFF) {
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

  if (strcmp(WebServer->httpServer->arg(INPUT_MAX_BUTTON).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_BUTTON, WebServer->httpServer->arg(INPUT_MAX_BUTTON).c_str());
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

void SuplaWebPageControl::supla_webpage_control(int save) {
  uint8_t nr, countFreeGpio;

  WebServer->sendHeaderStart();

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_CONTROL);
  addForm(webContentBuffer, F("post"), PATH_CONTROL);

  addFormHeader(webContentBuffer, S_GPIO_SETTINGS_FOR_BUTTONS);

  if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt() != FUNCTION_OFF) {
    countFreeGpio = 32;
  }
  else {
    countFreeGpio = ConfigESP->countFreeGpio(FUNCTION_BUTTON);
  }

  addNumberBox(webContentBuffer, INPUT_MAX_BUTTON, S_QUANTITY, KEY_MAX_BUTTON, countFreeGpio);

  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_BUTTON)->getValueInt(); nr++) {
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt() != FUNCTION_OFF) {
      addListMCP23017GPIOBox(webContentBuffer, INPUT_BUTTON_GPIO, S_BUTTON, FUNCTION_BUTTON, nr, PATH_BUTTON_SET_MCP23017);
    }
    else {
      addListGPIOLinkBox(webContentBuffer, INPUT_BUTTON_GPIO, S_BUTTON, getParameterRequest(PATH_BUTTON_SET, ARG_PARM_NUMBER), FUNCTION_BUTTON, nr);
    }
  }
  addFormHeaderEnd(webContentBuffer);

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);

  WebServer->sendHeaderEnd();
}
#endif

#if defined(SUPLA_BUTTON)
void SuplaWebPageControl::handleButtonSet() {
  if (!WebServer->isLoggedIn()) {
    return;
  }
  if (WebServer->httpServer->method() == HTTP_GET)
    supla_webpage_button_set(0);
  else
    handleButtonSaveSet();
}

void SuplaWebPageControl::handleButtonSaveSet() {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  String input, nr_button;
  uint8_t key, gpio;

  nr_button = WebServer->httpServer->arg(ARG_PARM_NUMBER);

  gpio = ConfigESP->getGpio(nr_button.toInt(), FUNCTION_BUTTON);
  key = KEY_GPIO + gpio;

  input = INPUT_BUTTON_INVERSED;
  input += nr_button;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(key, INVERSED_BUTTON, 1);
  }
  else {
    ConfigManager->setElement(key, INVERSED_BUTTON, 0);
  }

  input = INPUT_BUTTON_LEVEL;
  input += nr_button;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(key, PULL_UP_BUTTON, 1);
  }
  else {
    ConfigManager->setElement(key, PULL_UP_BUTTON, 0);
  }

  input = INPUT_BUTTON_EVENT;
  input += nr_button;
  ConfigManager->setElement(key, EVENT_BUTTON, WebServer->httpServer->arg(input).toInt());

  input = INPUT_BUTTON_ACTION;
  input += nr_button;
  ConfigManager->setElement(key, ACTION_BUTTON, WebServer->httpServer->arg(input).toInt());

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      supla_webpage_button_set(1, nr_button.toInt());
      break;
    case E_CONFIG_FILE_OPEN:
      supla_webpage_button_set(2, nr_button.toInt());
      break;
  }
}

void SuplaWebPageControl::supla_webpage_button_set(int save, int nr) {
  uint8_t gpio, selected;
  String nr_button;

  if (nr != 0) {
    nr_button = nr;
  }
  else {
    nr_button = WebServer->httpServer->arg(ARG_PARM_NUMBER);
  }

  Serial.println(nr_button);
  WebServer->sendHeaderStart();

  if (!nr_button.isEmpty()) {
    webContentBuffer += SuplaSaveResult(save);
    webContentBuffer += SuplaJavaScript(getParameterRequest(PATH_BUTTON_SET, ARG_PARM_NUMBER, nr_button));

    gpio = ConfigESP->getGpio(nr_button.toInt(), FUNCTION_BUTTON);

    addForm(webContentBuffer, F("post"), getParameterRequest(PATH_BUTTON_SET, ARG_PARM_NUMBER, nr_button));
    addFormHeader(webContentBuffer, S_BUTTON_NR_SETTINGS + nr_button);

    selected = ConfigESP->getPullUp(gpio);
    addCheckBox(webContentBuffer, INPUT_BUTTON_LEVEL + nr_button, S_INTERNAL_PULL_UP, selected);

    selected = ConfigESP->getInversed(gpio);
    addCheckBox(webContentBuffer, INPUT_BUTTON_INVERSED + nr_button, S_REVERSE_LOGIC, selected);

    selected = ConfigESP->getEvent(gpio);
    addListBox(webContentBuffer, INPUT_BUTTON_EVENT + nr_button, S_REACTION_TO, TRIGGER_P, 3, selected);

    selected = ConfigESP->getAction(gpio);
    addListBox(webContentBuffer, INPUT_BUTTON_ACTION + nr_button, S_ACTION, ACTION_P, 3, selected);

    addFormHeaderEnd(webContentBuffer);
    addButtonSubmit(webContentBuffer, S_SAVE);
    addFormEnd(webContentBuffer);
  }
  addButton(webContentBuffer, S_RETURN, PATH_CONTROL);

  WebServer->sendHeaderEnd();
}
#endif

#ifdef SUPLA_LIMIT_SWITCH
void SuplaWebPageControl::handleLimitSwitch() {
  if (!WebServer->isLoggedIn()) {
    return;
  }
  if (WebServer->httpServer->method() == HTTP_GET)
    suplaWebpageLimitSwitch(0);
  else
    handleLimitSwitchSave();
}

void SuplaWebPageControl::handleLimitSwitchSave() {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  uint8_t nr, last_value;

  last_value = ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt();
  for (nr = 1; nr <= last_value; nr++) {
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt() != FUNCTION_OFF) {
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

  if (strcmp(WebServer->httpServer->arg(INPUT_MAX_LIMIT_SWITCH).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_LIMIT_SWITCH, WebServer->httpServer->arg(INPUT_MAX_LIMIT_SWITCH).c_str());
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

  WebServer->sendHeaderStart();

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_SWITCH);
  addForm(webContentBuffer, F("post"), PATH_SWITCH);

  addFormHeader(webContentBuffer, S_GPIO_SETTINGS_FOR_LIMIT_SWITCH);

  if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt() != FUNCTION_OFF) {
    countFreeGpio = 32;
  }
  else {
    countFreeGpio = ConfigESP->countFreeGpio(FUNCTION_LIMIT_SWITCH);
  }

  addNumberBox(webContentBuffer, INPUT_MAX_LIMIT_SWITCH, S_QUANTITY, KEY_MAX_LIMIT_SWITCH, countFreeGpio);

  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt(); nr++) {
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt() != FUNCTION_OFF) {
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

  WebServer->sendHeaderEnd();
}
#endif

#ifdef SUPLA_MCP23017
void SuplaWebPageControl::handleButtonSetMCP23017() {
  if (!WebServer->isLoggedIn()) {
    return;
  }
  if (WebServer->httpServer->method() == HTTP_GET)
    supla_webpage_button_set_MCP23017(0);
  else
    handleButtonSaveSetMCP23017();
}

void SuplaWebPageControl::supla_webpage_button_set_MCP23017(int save) {
  uint8_t gpio, selected;

  WebServer->sendHeaderStart();

  gpio = ConfigESP->getGpio(1, FUNCTION_BUTTON);

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_BUTTON_SET_MCP23017);

  addForm(webContentBuffer, F("post"), PATH_BUTTON_SET_MCP23017);
  addFormHeader(webContentBuffer, S_SETTINGS_FOR_BUTTONS);

  selected = ConfigESP->getPullUp(gpio);
  addCheckBox(webContentBuffer, INPUT_BUTTON_LEVEL, S_INTERNAL_PULL_UP, selected);

  selected = ConfigESP->getInversed(gpio);
  addCheckBox(webContentBuffer, INPUT_BUTTON_INVERSED, S_REVERSE_LOGIC, selected);

  selected = ConfigESP->getEvent(gpio);
  addListBox(webContentBuffer, INPUT_BUTTON_EVENT, S_REACTION_TO, TRIGGER_P, 3, selected);

  selected = ConfigESP->getAction(gpio);
  addListBox(webContentBuffer, INPUT_BUTTON_ACTION, S_ACTION, ACTION_P, 3, selected);

  addFormHeaderEnd(webContentBuffer);
  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_CONTROL);

  WebServer->sendHeaderEnd();
}

void SuplaWebPageControl::handleButtonSaveSetMCP23017() {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  String input;
  uint8_t key, gpio, pullup, inversed, event, action, address;

  input.reserve(10);

  input = INPUT_BUTTON_EVENT;
  event = WebServer->httpServer->arg(input).toInt();

  input = INPUT_BUTTON_LEVEL;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    pullup = 1;
  }
  else {
    pullup = 0;
  }

  input = INPUT_BUTTON_INVERSED;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    inversed = 1;
  }
  else {
    inversed = 0;
  }

  input = INPUT_BUTTON_ACTION;
  action = WebServer->httpServer->arg(input).toInt();

  for (gpio = 0; gpio <= OFF_GPIO; gpio++) {
    key = KEY_GPIO + gpio;
    ConfigManager->setElement(key, PULL_UP_BUTTON, pullup);
    ConfigManager->setElement(key, INVERSED_BUTTON, inversed);
    ConfigManager->setElement(key, EVENT_BUTTON, event);
    ConfigManager->setElement(key, ACTION_BUTTON, action);
  }
  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      supla_webpage_button_set_MCP23017(1);
      break;
    case E_CONFIG_FILE_OPEN:
      supla_webpage_button_set_MCP23017(2);
      break;
  }
}
#endif
