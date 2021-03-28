#include "SuplaWebPageRelay.h"

SuplaWebPageRelay *WebPageRelay = new SuplaWebPageRelay();

SuplaWebPageRelay::SuplaWebPageRelay() {
}

#ifdef GUI_RELAY
void SuplaWebPageRelay::createWebPageRelay() {
  WebServer->httpServer->on(getURL(PATH_RELAY), std::bind(&SuplaWebPageRelay::handleRelay, this));

  if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt() != FUNCTION_OFF) {
#ifdef SUPLA_MCP23017
    WebServer->httpServer->on(getURL(PATH_RELAY_SET_MCP23017), std::bind(&SuplaWebPageRelay::handleRelaySetMCP23017, this));
#endif
  }
  else {
#if defined(SUPLA_RELAY)
    WebServer->httpServer->on(getURL(PATH_RELAY_SET), std::bind(&SuplaWebPageRelay::handleRelaySet, this));
#endif
  }
}

void SuplaWebPageRelay::handleRelay() {
  if (!WebServer->isLoggedIn()) {
    return;
  }
  if (WebServer->httpServer->method() == HTTP_GET)
    supla_webpage_relay(0);
  else
    handleRelaySave();
}

void SuplaWebPageRelay::handleRelaySave() {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  uint8_t nr;

  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_RELAY)->getValueInt(); nr++) {
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt() != FUNCTION_OFF) {
      if (!WebServer->saveGpioMCP23017(INPUT_RELAY_GPIO, FUNCTION_RELAY, nr, INPUT_MAX_RELAY)) {
        supla_webpage_relay(6);
        return;
      }
    }
    else {
      if (!WebServer->saveGPIO(INPUT_RELAY_GPIO, FUNCTION_RELAY, nr, INPUT_MAX_RELAY)) {
        supla_webpage_relay(6);
        return;
      }
    }
  }

  if (strcmp(WebServer->httpServer->arg(INPUT_MAX_RELAY).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_RELAY, WebServer->httpServer->arg(INPUT_MAX_RELAY).c_str());
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      supla_webpage_relay(1);
      break;
    case E_CONFIG_FILE_OPEN:
      supla_webpage_relay(2);
      break;
  }
}

void SuplaWebPageRelay::supla_webpage_relay(int save) {
  uint8_t nr, countFreeGpio;

  WebServer->sendHeaderStart();

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_RELAY);

  addForm(webContentBuffer, F("post"), PATH_RELAY);
  addFormHeader(webContentBuffer, S_GPIO_SETTINGS_FOR_RELAYS);

  if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt() != FUNCTION_OFF) {
    countFreeGpio = 32;
  }
  else {
    countFreeGpio = ConfigESP->countFreeGpio(FUNCTION_RELAY);
  }

  addNumberBox(webContentBuffer, INPUT_MAX_RELAY, S_QUANTITY, KEY_MAX_RELAY, countFreeGpio);

  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_RELAY)->getValueInt(); nr++) {
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt() != FUNCTION_OFF) {
      addListMCP23017GPIOBox(webContentBuffer, INPUT_RELAY_GPIO, S_RELAY, FUNCTION_RELAY, nr, PATH_RELAY_SET_MCP23017);
    }
    else {
      addListGPIOLinkBox(webContentBuffer, INPUT_RELAY_GPIO, S_RELAY, getParameterRequest(PATH_RELAY_SET, ARG_PARM_NUMBER), FUNCTION_RELAY, nr);
    }
  }
  addFormHeaderEnd(webContentBuffer);

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);

  WebServer->sendHeaderEnd();
}
#endif

#if defined(SUPLA_RELAY)
void SuplaWebPageRelay::handleRelaySet() {
  if (!WebServer->isLoggedIn()) {
    return;
  }
  if (WebServer->httpServer->method() == HTTP_GET)
    supla_webpage_relay_set(0);
  else
    handleRelaySaveSet();
}

void SuplaWebPageRelay::handleRelaySaveSet() {
  if (!WebServer->isLoggedIn()) {
    return;
  }

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
    supla_webpage_relay_set(6, nr_relay.toInt());
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
    input = INPUT_PUSHOVER;
    ConfigManager->setElement(KEY_PUSHOVER, (nr_relay.toInt() - 1), WebServer->httpServer->arg(input).toInt());
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
      supla_webpage_relay_set(1, nr_relay.toInt());
      break;
    case E_CONFIG_FILE_OPEN:
      supla_webpage_relay_set(2, nr_relay.toInt());
      break;
  }
}

void SuplaWebPageRelay::supla_webpage_relay_set(int save, int nr) {
  uint8_t gpio, selected;
  String nr_relay, massage;

  massage.reserve(MAX_DIRECT_LINKS_SIZE);

  if (nr != 0) {
    nr_relay = nr;
  }
  else {
    nr_relay = WebServer->httpServer->arg(ARG_PARM_NUMBER);
  }

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

      selected = ConfigManager->get(KEY_PUSHOVER)->getElement(nr_relay.toInt() - 1).toInt();
      addListBox(webContentBuffer, INPUT_PUSHOVER, S_STATE, STATE_P, 2, selected);

      massage = ConfigManager->get(KEY_PUSHOVER_MASSAGE)->getElement(nr_relay.toInt() - 1).c_str();
      addTextBox(webContentBuffer, INPUT_PUSHOVER_MESSAGE, S_MESSAGE, massage, 0, 16, false);
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
void SuplaWebPageRelay::handleRelaySetMCP23017() {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  if (WebServer->httpServer->method() == HTTP_GET)
    supla_webpage_relay_set_MCP23017(0);
  else
    handleRelaySaveSetMCP23017();
}

void SuplaWebPageRelay::supla_webpage_relay_set_MCP23017(int save) {
  uint8_t gpio, selected;
  String massage, name, input;
  input.reserve(9);

  gpio = ConfigESP->getGpio(1, FUNCTION_RELAY);

  WebServer->sendHeaderStart();
  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_RELAY_SET_MCP23017);

  addForm(webContentBuffer, F("post"), PATH_RELAY_SET_MCP23017);
  addFormHeader(webContentBuffer, S_SETTINGS_FOR_RELAYS);

  selected = ConfigESP->getLevel(gpio);
  input = INPUT_RELAY_LEVEL;
  addListBox(webContentBuffer, input, S_STATE_CONTROL, LEVEL_P, 2, selected);

  selected = ConfigESP->getMemory(gpio);
  input = INPUT_RELAY_MEMORY;
  addListBox(webContentBuffer, input, S_REACTION_AFTER_RESET, MEMORY_P, 3, selected);
  addFormHeaderEnd(webContentBuffer);

#if defined(SUPLA_PUSHOVER)
  addFormHeader(webContentBuffer, S_PUSHOVER);
  for (uint8_t i = 0; i <= MAX_PUSHOVER_MESSAGE; i++) {
    selected = ConfigManager->get(KEY_PUSHOVER)->getElement(i).toInt();
    name = S_STATE;
    name = +S_SPACE;
    name += i + 1;
    input = INPUT_PUSHOVER;
    input += i;
    addListBox(webContentBuffer, input, name, STATE_P, 2, selected);

    massage = ConfigManager->get(KEY_PUSHOVER_MASSAGE)->getElement(i).c_str();
    name = S_MESSAGE;
    name += S_SPACE;
    name += i + 1;
    input = INPUT_PUSHOVER_MESSAGE;
    input += i;
    addTextBox(webContentBuffer, input, name, massage, 0, 16, false);
  }
  addFormHeaderEnd(webContentBuffer);
#endif

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_RELAY);

  WebServer->sendHeaderEnd();
}

void SuplaWebPageRelay::handleRelaySaveSetMCP23017() {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  String input;
  uint8_t key, gpio, memory, level, address;

  input.reserve(9);

  input = INPUT_RELAY_MEMORY;
  memory = WebServer->httpServer->arg(input).toInt();

  input = INPUT_RELAY_LEVEL;
  level = WebServer->httpServer->arg(input).toInt();

  for (gpio = 0; gpio <= OFF_GPIO; gpio++) {
    key = KEY_GPIO + gpio;
    ConfigManager->setElement(key, MEMORY, memory);
    ConfigManager->setElement(key, LEVEL_RELAY, level);
  }

#if defined(SUPLA_PUSHOVER)
  for (uint8_t i = 0; i <= MAX_PUSHOVER_MESSAGE; i++) {
    input = INPUT_PUSHOVER;
    input += i;
    ConfigManager->setElement(KEY_PUSHOVER, i, WebServer->httpServer->arg(input).toInt());
    input = INPUT_PUSHOVER_MESSAGE;
    input += i;
    ConfigManager->setElement(KEY_PUSHOVER_MASSAGE, i, WebServer->httpServer->arg(input).c_str());
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      supla_webpage_relay_set_MCP23017(1);
      break;
    case E_CONFIG_FILE_OPEN:
      supla_webpage_relay_set_MCP23017(2);
      break;
  }
}
#endif
