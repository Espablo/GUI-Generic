#include "SuplaWebPageControl.h"

#ifdef GUI_CONTROL
void createWebPageControl() {
  WebServer->httpServer->on(getURL(PATH_CONTROL), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handleControl();
    else
      handleControlSave();
  });

  WebServer->httpServer->on(getURL(PATH_BUTTON_SET), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }
#ifdef SUPLA_MCP23017
    if (ConfigESP->checkActiveMCP23017(FUNCTION_BUTTON)) {
      if (WebServer->httpServer->method() == HTTP_GET)
        handleButtonSetMCP23017();
      else
        handleButtonSaveSetMCP23017();
    }
    else {
#ifdef SUPLA_BUTTON
      if (WebServer->httpServer->method() == HTTP_GET)
        handleButtonSet();
      else
        handleButtonSaveSet();
#endif
    }
#else
#ifdef SUPLA_BUTTON
      if (WebServer->httpServer->method() == HTTP_GET)
        handleButtonSet();
      else
        handleButtonSaveSet();
#endif
#endif
  });
}
#endif

#ifdef GUI_CONTROL
void handleControlSave() {
  uint8_t nr;

  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_BUTTON)->getValueInt(); nr++) {
#ifdef SUPLA_MCP23017
    if (ConfigESP->checkActiveMCP23017(FUNCTION_BUTTON)) {
      if (!WebServer->saveGpioMCP23017(INPUT_BUTTON_GPIO, FUNCTION_BUTTON, nr, INPUT_MAX_BUTTON)) {
        handleControl(6);
        return;
      }
    }
    else {
      if (!WebServer->saveGPIO(INPUT_BUTTON_GPIO, FUNCTION_BUTTON, nr, INPUT_MAX_BUTTON)) {
        handleControl(6);
        return;
      }
    }
#else
    if (!WebServer->saveGPIO(INPUT_BUTTON_GPIO, FUNCTION_BUTTON, nr, INPUT_MAX_BUTTON)) {
      handleControl(6);
      return;
    }
#endif
  }

  if (strcmp(WebServer->httpServer->arg(INPUT_MAX_BUTTON).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_BUTTON, WebServer->httpServer->arg(INPUT_MAX_BUTTON).c_str());
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleControl(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleControl(2);
      break;
  }
}

void handleControl(int save) {
  uint8_t nr;

  WebServer->sendHeaderStart();
  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_CONTROL);

  addForm(webContentBuffer, F("post"), PATH_CONTROL);
  addFormHeader(webContentBuffer, S_GPIO_SETTINGS_FOR_BUTTONS);
  addNumberBox(webContentBuffer, INPUT_MAX_BUTTON, S_QUANTITY, KEY_MAX_BUTTON, ConfigESP->countFreeGpio(FUNCTION_BUTTON));

  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_BUTTON)->getValueInt(); nr++) {
#ifdef SUPLA_MCP23017
    if (ConfigESP->checkActiveMCP23017(FUNCTION_BUTTON)) {
      addListMCP23017GPIOBox(webContentBuffer, INPUT_BUTTON_GPIO, S_BUTTON, FUNCTION_BUTTON, nr, PATH_BUTTON_SET);
    }
    else {
      addListGPIOLinkBox(webContentBuffer, INPUT_BUTTON_GPIO, S_BUTTON, getParameterRequest(PATH_BUTTON_SET, ARG_PARM_NUMBER), FUNCTION_BUTTON, nr);
    }
#else
    addListGPIOLinkBox(webContentBuffer, INPUT_BUTTON_GPIO, S_BUTTON, getParameterRequest(PATH_BUTTON_SET, ARG_PARM_NUMBER), FUNCTION_BUTTON, nr);
#endif
  }

  addFormHeaderEnd(webContentBuffer);

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);

  WebServer->sendHeaderEnd();
}
#endif

#if defined(SUPLA_BUTTON)
void handleButtonSaveSet() {
  String input, nr_button;
  uint8_t key, gpio;

  nr_button = WebServer->httpServer->arg(ARG_PARM_NUMBER);

  gpio = ConfigESP->getGpio(nr_button.toInt(), FUNCTION_BUTTON);
  key = KEY_GPIO + gpio;

  input = INPUT_BUTTON_INVERSED;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(key, INVERSED_BUTTON, 1);
  }
  else {
    ConfigManager->setElement(key, INVERSED_BUTTON, 0);
  }

  input = INPUT_BUTTON_LEVEL;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(key, PULL_UP_BUTTON, 1);
  }
  else {
    ConfigManager->setElement(key, PULL_UP_BUTTON, 0);
  }

  input = INPUT_BUTTON_EVENT;
  ConfigManager->setElement(key, EVENT_BUTTON, WebServer->httpServer->arg(input).toInt());

  input = INPUT_BUTTON_ACTION;
  ConfigManager->setElement(key, ACTION_BUTTON, WebServer->httpServer->arg(input).toInt());

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleButtonSet(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleButtonSet(2);
      break;
  }
}

void handleButtonSet(int save) {
  uint8_t gpio, selected;
  String nr_button;

  nr_button = WebServer->httpServer->arg(ARG_PARM_NUMBER);

  WebServer->sendHeaderStart();

  if (!nr_button.isEmpty()) {
    webContentBuffer += SuplaSaveResult(save);
    webContentBuffer += SuplaJavaScript(getParameterRequest(PATH_BUTTON_SET, ARG_PARM_NUMBER, nr_button));

    gpio = ConfigESP->getGpio(nr_button.toInt(), FUNCTION_BUTTON);

    addForm(webContentBuffer, F("post"), getParameterRequest(PATH_BUTTON_SET, ARG_PARM_NUMBER, nr_button));
    addFormHeader(webContentBuffer, S_BUTTON_NR_SETTINGS + nr_button);

    if (ConfigManager->get(KEY_MAX_ROLLERSHUTTER)->getValueInt() * 2 >= nr_button.toInt()) {
#ifdef SUPLA_ROLLERSHUTTER
      selected = ConfigESP->getPullUp(gpio);
      addCheckBox(webContentBuffer, INPUT_BUTTON_LEVEL, S_INTERNAL_PULL_UP, selected);
      selected = ConfigESP->getInversed(gpio);
      addCheckBox(webContentBuffer, INPUT_BUTTON_INVERSED, S_REVERSE_LOGIC, selected);

      if (nr_button.toInt() % 2 == 1) {
        selected = ConfigESP->getEvent(gpio);
        addListBox(webContentBuffer, INPUT_BUTTON_EVENT, S_REACTION_TO, TRIGGER_P, 3, selected);
        selected = ConfigESP->getAction(gpio);
        addListBox(webContentBuffer, INPUT_BUTTON_ACTION, S_ACTION, ACTION_ROLLER_SHUTTER_P, 3, selected);
      }
#endif
    }
    else {
      selected = ConfigESP->getPullUp(gpio);
      addCheckBox(webContentBuffer, INPUT_BUTTON_LEVEL, S_INTERNAL_PULL_UP, selected);
      selected = ConfigESP->getInversed(gpio);
      addCheckBox(webContentBuffer, INPUT_BUTTON_INVERSED, S_REVERSE_LOGIC, selected);
      selected = ConfigESP->getEvent(gpio);
      addListBox(webContentBuffer, INPUT_BUTTON_EVENT, S_REACTION_TO, TRIGGER_P, 3, selected);
      selected = ConfigESP->getAction(gpio);
      addListBox(webContentBuffer, INPUT_BUTTON_ACTION, S_ACTION, ACTION_P, 3, selected);
    }

    addFormHeaderEnd(webContentBuffer);
    addButtonSubmit(webContentBuffer, S_SAVE);
    addFormEnd(webContentBuffer);
  }
  addButton(webContentBuffer, S_RETURN, PATH_CONTROL);

  WebServer->sendHeaderEnd();
}
#endif

#ifdef SUPLA_MCP23017
void handleButtonSetMCP23017(int save) {
  uint8_t gpio, selected;
  String nr_button;

  nr_button.reserve(2);

  WebServer->sendHeaderStart();
  nr_button = WebServer->httpServer->arg(ARG_PARM_NUMBER);

  if (!nr_button.isEmpty())
    gpio = ConfigESP->getGpioMCP23017(nr_button.toInt(), FUNCTION_BUTTON);
  else
    gpio = ConfigESP->getGpioMCP23017(1, FUNCTION_BUTTON);

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(getParameterRequest(PATH_BUTTON_SET, ARG_PARM_NUMBER, nr_button));

  addForm(webContentBuffer, F("post"), getParameterRequest(PATH_BUTTON_SET, ARG_PARM_NUMBER, nr_button));

  if (!nr_button.isEmpty()) {
    addFormHeader(webContentBuffer, String(S_BUTTON_NR_SETTINGS) + nr_button.toInt());
  }
  else {
    addFormHeader(webContentBuffer, S_SETTINGS_FOR_BUTTONS);
  }

  if (ConfigManager->get(KEY_MAX_ROLLERSHUTTER)->getValueInt() * 2 >= nr_button.toInt() && !nr_button.isEmpty()) {
#ifdef SUPLA_ROLLERSHUTTER
    selected = ConfigESP->getPullUp(gpio);
    addCheckBox(webContentBuffer, INPUT_BUTTON_LEVEL, S_INTERNAL_PULL_UP, selected);
    selected = ConfigESP->getInversed(gpio);
    addCheckBox(webContentBuffer, INPUT_BUTTON_INVERSED, S_REVERSE_LOGIC, selected);

    if (nr_button.toInt() % 2 == 1) {
      selected = ConfigESP->getEvent(gpio);
      addListBox(webContentBuffer, INPUT_BUTTON_EVENT, S_REACTION_TO, TRIGGER_P, 3, selected);
      selected = ConfigESP->getAction(gpio);
      addListBox(webContentBuffer, INPUT_BUTTON_ACTION, S_ACTION, ACTION_ROLLER_SHUTTER_P, 3, selected);
    }
#endif
  }
  else {
    selected = ConfigESP->getPullUp(gpio);
    addCheckBox(webContentBuffer, INPUT_BUTTON_LEVEL, S_INTERNAL_PULL_UP, selected);
    selected = ConfigESP->getInversed(gpio);
    addCheckBox(webContentBuffer, INPUT_BUTTON_INVERSED, S_REVERSE_LOGIC, selected);
    selected = ConfigESP->getEvent(gpio);
    addListBox(webContentBuffer, INPUT_BUTTON_EVENT, S_REACTION_TO, TRIGGER_P, 3, selected);
    selected = ConfigESP->getAction(gpio);
    addListBox(webContentBuffer, INPUT_BUTTON_ACTION, S_ACTION, ACTION_P, 3, selected);
  }

  addFormHeaderEnd(webContentBuffer);
  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_CONTROL);

  WebServer->sendHeaderEnd();
}

void handleButtonSaveSetMCP23017() {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  String input, nr_button;
  uint8_t key, gpio, pullup, inversed, event, action;

  input.reserve(10);
  nr_button.reserve(2);

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

  nr_button = WebServer->httpServer->arg(ARG_PARM_NUMBER);

  if (!nr_button.isEmpty()) {
    gpio = ConfigESP->getGpioMCP23017(nr_button.toInt(), FUNCTION_BUTTON);
    key = KEY_GPIO + gpio;

    ConfigManager->setElement(key, PULL_UP_BUTTON, pullup);
    ConfigManager->setElement(key, INVERSED_BUTTON, inversed);
    ConfigManager->setElement(key, EVENT_BUTTON, event);
    ConfigManager->setElement(key, ACTION_BUTTON, action);
  }
  else {
    for (gpio = 0; gpio <= OFF_GPIO; gpio++) {
      key = KEY_GPIO + gpio;
      ConfigManager->setElement(key, PULL_UP_BUTTON, pullup);
      ConfigManager->setElement(key, INVERSED_BUTTON, inversed);
      ConfigManager->setElement(key, EVENT_BUTTON, event);
      ConfigManager->setElement(key, ACTION_BUTTON, action);
    }
  }
  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleButtonSetMCP23017(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleButtonSetMCP23017(2);
      break;
  }
}
#endif
