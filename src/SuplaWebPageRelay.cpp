#include "SuplaWebPageRelay.h"

#ifdef GUI_RELAY
void createWebPageRelay() {
  WebServer->httpServer->on(getURL(PATH_RELAY), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handleRelay();
    else
      handleRelaySave();
  });

  WebServer->httpServer->on(getURL(PATH_RELAY_SET), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }
#ifdef SUPLA_MCP23017
    if (ConfigESP->checkActiveMCP23017(FUNCTION_RELAY)) {
      if (WebServer->httpServer->method() == HTTP_GET)
        handleRelaySetMCP23017();
      else
        handleRelaySaveSetMCP23017();
    }
    else {
#ifdef SUPLA_RELAY
      if (WebServer->httpServer->method() == HTTP_GET)
        handleRelaySet();
      else
        handleRelaySaveSet();
#endif
    }
#else
#ifdef SUPLA_RELAY
    if (WebServer->httpServer->method() == HTTP_GET)
      handleRelaySet();
    else
      handleRelaySaveSet();
#endif
#endif
  });
}

void handleRelaySave() {
  uint8_t nr;

  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_RELAY)->getValueInt(); nr++) {
#ifdef SUPLA_MCP23017
    if (ConfigESP->checkActiveMCP23017(FUNCTION_RELAY)) {
      if (!WebServer->saveGpioMCP23017(INPUT_RELAY_GPIO, FUNCTION_RELAY, nr, INPUT_MAX_RELAY)) {
        handleRelay(6);
        return;
      }
    }
    else {
      if (!WebServer->saveGPIO(INPUT_RELAY_GPIO, FUNCTION_RELAY, nr, INPUT_MAX_RELAY)) {
        handleRelay(6);
        return;
      }
    }
#else
    if (!WebServer->saveGPIO(INPUT_RELAY_GPIO, FUNCTION_RELAY, nr, INPUT_MAX_RELAY)) {
      handleRelay(6);
      return;
    }
#endif
  }

  if (strcmp(WebServer->httpServer->arg(INPUT_MAX_RELAY).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_RELAY, WebServer->httpServer->arg(INPUT_MAX_RELAY).c_str());
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleRelay(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleRelay(2);
      break;
  }
}

void handleRelay(int save) {
  uint8_t nr;

  WebServer->sendHeaderStart();

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_RELAY);

  addForm(webContentBuffer, F("post"), PATH_RELAY);
  addFormHeader(webContentBuffer, S_GPIO_SETTINGS_FOR_RELAYS);
  addNumberBox(webContentBuffer, INPUT_MAX_RELAY, S_QUANTITY, KEY_MAX_RELAY, ConfigESP->countFreeGpio(FUNCTION_RELAY));

  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_RELAY)->getValueInt(); nr++) {
#ifdef SUPLA_MCP23017
    if (ConfigESP->checkActiveMCP23017(FUNCTION_RELAY)) {
      addListMCP23017GPIOBox(webContentBuffer, INPUT_RELAY_GPIO, S_RELAY, FUNCTION_RELAY, nr, PATH_RELAY_SET);
    }
    else {
      addListGPIOLinkBox(webContentBuffer, INPUT_RELAY_GPIO, S_RELAY, getParameterRequest(PATH_RELAY_SET, ARG_PARM_NUMBER), FUNCTION_RELAY, nr);
    }
#else
    addListGPIOLinkBox(webContentBuffer, INPUT_RELAY_GPIO, S_RELAY, getParameterRequest(PATH_RELAY_SET, ARG_PARM_NUMBER), FUNCTION_RELAY, nr);
#endif
  }
  addFormHeaderEnd(webContentBuffer);

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);

  WebServer->sendHeaderEnd();
}
#endif

#if defined(SUPLA_RELAY)
void handleRelaySaveSet() {
  String input, nr_relay;
  uint8_t key, gpio;

  nr_relay = WebServer->httpServer->arg(ARG_PARM_NUMBER);

  gpio = ConfigESP->getGpio(nr_relay.toInt(), FUNCTION_RELAY);
  key = KEY_GPIO + gpio;

  input = INPUT_RELAY_MEMORY;
  input += nr_relay;
  ConfigManager->setElement(key, MEMORY, WebServer->httpServer->arg(input).toInt());

  input = INPUT_RELAY_LEVEL;
  input += nr_relay;
  ConfigManager->setElement(key, LEVEL_RELAY, WebServer->httpServer->arg(input).toInt());

  input = INPUT_CONDITIONS_SENSOR_TYPE;
  ConfigManager->setElement(KEY_CONDITIONS_SENSOR_TYPE, (nr_relay.toInt() - 1), WebServer->httpServer->arg(input).toInt());
  input = INPUT_CONDITIONS_TYPE;
  ConfigManager->setElement(KEY_CONDITIONS_TYPE, (nr_relay.toInt() - 1), WebServer->httpServer->arg(input).toInt());
  input = INPUT_CONDITIONS_MIN;
  ConfigManager->setElement(KEY_CONDITIONS_MIN, (nr_relay.toInt() - 1), WebServer->httpServer->arg(input).c_str());
  input = INPUT_CONDITIONS_MAX;
  ConfigManager->setElement(KEY_CONDITIONS_MAX, (nr_relay.toInt() - 1), WebServer->httpServer->arg(input).c_str());

#if defined(SUPLA_LED)
  input = INPUT_LED;
  input += nr_relay;
  if (!WebServer->saveGPIO(input, FUNCTION_LED, nr_relay.toInt())) {
    handleRelaySet(6);
    return;
  }

  if (ConfigESP->getGpio(nr_relay.toInt(), FUNCTION_LED) != OFF_GPIO) {
    gpio = ConfigESP->getGpio(nr_relay.toInt(), FUNCTION_LED);
    key = KEY_GPIO + gpio;
    input = INPUT_LEVEL_LED;
    input += nr_relay;

    ConfigManager->setElement(key, INVERSED_BUTTON, WebServer->httpServer->arg(input).toInt());
  }

#endif

#if defined(SUPLA_PUSHOVER)
  if (nr_relay.toInt() - 1 <= MAX_PUSHOVER_MESSAGE) {
    input = INPUT_PUSHOVER_MESSAGE;
    ConfigManager->setElement(KEY_PUSHOVER_MASSAGE, (nr_relay.toInt() - 1), WebServer->httpServer->arg(input).c_str());
  }
#endif

#if defined(SUPLA_DIRECT_LINKS)
  if (nr_relay.toInt() - 1 <= MAX_DIRECT_LINK) {
    input = INPUT_DIRECT_LINK_ON;
    ConfigManager->setElement(KEY_DIRECT_LINKS_ON, (nr_relay.toInt() - 1), WebServer->httpServer->arg(input).c_str());
    input = INPUT_DIRECT_LINK_OFF;
    ConfigManager->setElement(KEY_DIRECT_LINKS_OFF, (nr_relay.toInt() - 1), WebServer->httpServer->arg(input).c_str());
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleRelaySet(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleRelaySet(2);
      break;
  }
}

void handleRelaySet(int save) {
  uint8_t gpio, selected;
  String nr_relay, massage;

  massage.reserve(MAX_MESSAGE_SIZE);
  nr_relay = WebServer->httpServer->arg(ARG_PARM_NUMBER);

  WebServer->sendHeaderStart();

  if (!nr_relay.isEmpty()) {
    webContentBuffer += SuplaSaveResult(save);
    webContentBuffer += SuplaJavaScript(getParameterRequest(PATH_RELAY_SET, ARG_PARM_NUMBER, nr_relay));

    gpio = ConfigESP->getGpio(nr_relay.toInt(), FUNCTION_RELAY);

    addForm(webContentBuffer, F("post"), getParameterRequest(PATH_RELAY_SET, ARG_PARM_NUMBER, nr_relay));
    addFormHeader(webContentBuffer, S_RELAY_NR_SETTINGS + nr_relay);

    selected = ConfigESP->getLevel(gpio);
    addListBox(webContentBuffer, INPUT_RELAY_LEVEL + nr_relay, S_STATE_CONTROL, LEVEL_P, 2, selected);

    selected = ConfigESP->getMemory(gpio);
    addListBox(webContentBuffer, INPUT_RELAY_MEMORY + nr_relay, S_REACTION_AFTER_RESET, MEMORY_P, 3, selected);
    addFormHeaderEnd(webContentBuffer);

#if defined(SUPLA_LED)
    addFormHeader(webContentBuffer, S_RELAY_ACTIVATION_STATUS);

    addListGPIOBox(webContentBuffer, INPUT_LED + nr_relay, S_LED, FUNCTION_LED, nr_relay.toInt());

    selected = ConfigESP->getInversed(ConfigESP->getGpio(nr_relay.toInt(), FUNCTION_LED));
    addListBox(webContentBuffer, INPUT_LEVEL_LED + nr_relay, S_STATE_CONTROL, LEVEL_P, 2, selected);

    addFormHeaderEnd(webContentBuffer);
#endif

#if defined(SUPLA_PUSHOVER)
    if (nr_relay.toInt() - 1 <= MAX_PUSHOVER_MESSAGE) {
      addFormHeader(webContentBuffer, S_PUSHOVER);

      massage = ConfigManager->get(KEY_PUSHOVER_MASSAGE)->getElement(nr_relay.toInt() - 1).c_str();
      addTextBox(webContentBuffer, INPUT_PUSHOVER_MESSAGE, S_MESSAGE, massage, 0, MAX_MESSAGE_SIZE, false);
      addFormHeaderEnd(webContentBuffer);
    }
#endif

#if defined(SUPLA_DIRECT_LINKS)
    if (nr_relay.toInt() - 1 <= MAX_DIRECT_LINK) {
      addFormHeader(webContentBuffer, S_DIRECT_LINKS);
      massage = ConfigManager->get(KEY_DIRECT_LINKS_ON)->getElement(nr_relay.toInt() - 1).c_str();
      addTextBox(webContentBuffer, INPUT_DIRECT_LINK_ON, S_ON, massage, F("xx/xxxxxxxxx/turn-on"), 0, MAX_DIRECT_LINKS_SIZE, false);

      massage = ConfigManager->get(KEY_DIRECT_LINKS_OFF)->getElement(nr_relay.toInt() - 1).c_str();
      addTextBox(webContentBuffer, INPUT_DIRECT_LINK_OFF, S_OFF, massage, F("xx/xxxxxxxxx/turn-off"), 0, MAX_DIRECT_LINKS_SIZE, false);

      addFormHeaderEnd(webContentBuffer);
    }
#endif

    if (COUNT_SENSOR_LIST > 1) {
      addFormHeader(webContentBuffer, S_CONDITIONING);
      selected = ConfigManager->get(KEY_CONDITIONS_SENSOR_TYPE)->getElement(nr_relay.toInt() - 1).toInt();
      addListBox(webContentBuffer, INPUT_CONDITIONS_SENSOR_TYPE, S_SENSOR, SENSOR_LIST_P, COUNT_SENSOR_LIST, selected);

      selected = ConfigManager->get(KEY_CONDITIONS_TYPE)->getElement(nr_relay.toInt() - 1).toInt();
      addListBox(webContentBuffer, INPUT_CONDITIONS_TYPE, S_CONDITION, CONDITIONS_TYPE_P, 4, selected);

      String value = ConfigManager->get(KEY_CONDITIONS_MIN)->getElement(nr_relay.toInt() - 1);
      addNumberBox(webContentBuffer, INPUT_CONDITIONS_MIN, S_ON, S_SWITCH_ON_VALUE, false, value);
      value = ConfigManager->get(KEY_CONDITIONS_MAX)->getElement(nr_relay.toInt() - 1);
      addNumberBox(webContentBuffer, INPUT_CONDITIONS_MAX, S_OFF, S_SWITCH_OFF_VALUE, false, value);
      addFormHeaderEnd(webContentBuffer);
    }

    addButtonSubmit(webContentBuffer, S_SAVE);
    addFormEnd(webContentBuffer);
  }
  addButton(webContentBuffer, S_RETURN, PATH_RELAY);

  WebServer->sendHeaderEnd();
}
#endif

#ifdef SUPLA_MCP23017
void handleRelaySetMCP23017(int save) {
  uint8_t gpio, selected;
  String massage, input, nr_relay;

  massage.reserve(MAX_MESSAGE_SIZE);
  input.reserve(9);
  nr_relay.reserve(2);

  nr_relay = WebServer->httpServer->arg(ARG_PARM_NUMBER);

  if (!nr_relay.isEmpty())
    gpio = ConfigESP->getGpioMCP23017(nr_relay.toInt(), FUNCTION_RELAY);
  else
    gpio = ConfigESP->getGpioMCP23017(1, FUNCTION_RELAY);

  WebServer->sendHeaderStart();
  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(getParameterRequest(PATH_RELAY_SET, ARG_PARM_NUMBER, nr_relay));

  addForm(webContentBuffer, F("post"), getParameterRequest(PATH_RELAY_SET, ARG_PARM_NUMBER, nr_relay));

  if (!nr_relay.isEmpty()) {
    addFormHeader(webContentBuffer, String(S_RELAY_NR_SETTINGS) + nr_relay.toInt());
  }
  else {
    addFormHeader(webContentBuffer, S_SETTINGS_FOR_RELAYS);
  }

  selected = ConfigESP->getLevel(gpio);
  input = INPUT_RELAY_LEVEL;
  addListBox(webContentBuffer, input, S_STATE_CONTROL, LEVEL_P, 2, selected);

  selected = ConfigESP->getMemory(gpio);
  input = INPUT_RELAY_MEMORY;
  addListBox(webContentBuffer, input, S_REACTION_AFTER_RESET, MEMORY_P, 3, selected);
  addFormHeaderEnd(webContentBuffer);

#ifdef SUPLA_PUSHOVER
  if (!nr_relay.isEmpty()) {
    if (nr_relay.toInt() - 1 <= MAX_PUSHOVER_MESSAGE) {
      addFormHeader(webContentBuffer, S_PUSHOVER);

      massage = ConfigManager->get(KEY_PUSHOVER_MASSAGE)->getElement(nr_relay.toInt() - 1).c_str();
      addTextBox(webContentBuffer, INPUT_PUSHOVER_MESSAGE, S_MESSAGE, massage, 0, MAX_MESSAGE_SIZE, false);
      addFormHeaderEnd(webContentBuffer);
    }
  }
#endif

  if (!nr_relay.isEmpty()) {
    if (COUNT_SENSOR_LIST > 1) {
      addFormHeader(webContentBuffer, S_CONDITIONING);
      selected = ConfigManager->get(KEY_CONDITIONS_SENSOR_TYPE)->getElement(nr_relay.toInt() - 1).toInt();
      addListBox(webContentBuffer, INPUT_CONDITIONS_SENSOR_TYPE, S_SENSOR, SENSOR_LIST_P, COUNT_SENSOR_LIST, selected);

      selected = ConfigManager->get(KEY_CONDITIONS_TYPE)->getElement(nr_relay.toInt() - 1).toInt();
      addListBox(webContentBuffer, INPUT_CONDITIONS_TYPE, S_CONDITION, CONDITIONS_TYPE_P, 4, selected);

      String value = ConfigManager->get(KEY_CONDITIONS_MIN)->getElement(nr_relay.toInt() - 1);
      addNumberBox(webContentBuffer, INPUT_CONDITIONS_MIN, S_ON, S_SWITCH_ON_VALUE, false, value);
      value = ConfigManager->get(KEY_CONDITIONS_MAX)->getElement(nr_relay.toInt() - 1);
      addNumberBox(webContentBuffer, INPUT_CONDITIONS_MAX, S_OFF, S_SWITCH_OFF_VALUE, false, value);
      addFormHeaderEnd(webContentBuffer);
    }
  }

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_RELAY);

  WebServer->sendHeaderEnd();
}

void handleRelaySaveSetMCP23017() {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  String input, nr_relay;
  uint8_t key, gpio, memory, level;

  input.reserve(9);
  nr_relay.reserve(2);

  input = INPUT_RELAY_MEMORY;
  memory = WebServer->httpServer->arg(input).toInt();

  input = INPUT_RELAY_LEVEL;
  level = WebServer->httpServer->arg(input).toInt();

  nr_relay = WebServer->httpServer->arg(ARG_PARM_NUMBER);

  if (!nr_relay.isEmpty()) {
    gpio = ConfigESP->getGpioMCP23017(nr_relay.toInt(), FUNCTION_RELAY);
    key = KEY_GPIO + gpio;

    ConfigManager->setElement(key, MEMORY, memory);
    ConfigManager->setElement(key, LEVEL_RELAY, level);

#if defined(SUPLA_PUSHOVER)
    if (nr_relay.toInt() - 1 <= MAX_PUSHOVER_MESSAGE) {
      input = INPUT_PUSHOVER_MESSAGE;
      ConfigManager->setElement(KEY_PUSHOVER_MASSAGE, (nr_relay.toInt() - 1), WebServer->httpServer->arg(input).c_str());
    }
#endif

    input = INPUT_CONDITIONS_SENSOR_TYPE;
    ConfigManager->setElement(KEY_CONDITIONS_SENSOR_TYPE, (nr_relay.toInt() - 1), WebServer->httpServer->arg(input).toInt());
    input = INPUT_CONDITIONS_TYPE;
    ConfigManager->setElement(KEY_CONDITIONS_TYPE, (nr_relay.toInt() - 1), WebServer->httpServer->arg(input).toInt());
    input = INPUT_CONDITIONS_MIN;
    ConfigManager->setElement(KEY_CONDITIONS_MIN, (nr_relay.toInt() - 1), WebServer->httpServer->arg(input).c_str());
    input = INPUT_CONDITIONS_MAX;
    ConfigManager->setElement(KEY_CONDITIONS_MAX, (nr_relay.toInt() - 1), WebServer->httpServer->arg(input).c_str());
  }
  else {
    for (gpio = 0; gpio <= OFF_GPIO; gpio++) {
      key = KEY_GPIO + gpio;
      ConfigManager->setElement(key, MEMORY, memory);
      ConfigManager->setElement(key, LEVEL_RELAY, level);
    }

#if defined(SUPLA_PUSHOVER)
    for (uint8_t i = 0; i <= MAX_PUSHOVER_MESSAGE; i++) {
      input = INPUT_PUSHOVER_MESSAGE;
      input += i;
      ConfigManager->setElement(KEY_PUSHOVER_MASSAGE, i, WebServer->httpServer->arg(input).c_str());
    }
#endif
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleRelaySetMCP23017(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleRelaySetMCP23017(2);
      break;
  }
}
#endif
