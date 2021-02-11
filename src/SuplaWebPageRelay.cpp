#include "SuplaWebPageRelay.h"
#include "SuplaDeviceGUI.h"
#include "SuplaWebServer.h"
#include "SuplaCommonPROGMEM.h"
#include "GUIGenericCommon.h"
#include "Markup.h"
#include "SuplaWebPageSensor.h"

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
SuplaWebPageRelay *WebPageRelay = new SuplaWebPageRelay();

SuplaWebPageRelay::SuplaWebPageRelay() {
}

void SuplaWebPageRelay::createWebPageRelay() {
  String path;
  path.reserve(11);

  path += PATH_START;
  path += PATH_RELAY;
  WebServer->httpServer.on(path, HTTP_GET, std::bind(&SuplaWebPageRelay::handleRelay, this));
  path = PATH_START;
  path += PATH_SAVE_RELAY;
  WebServer->httpServer.on(path, HTTP_POST, std::bind(&SuplaWebPageRelay::handleRelaySave, this));

  if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt() != FUNCTION_OFF) {
    path = PATH_START;
    path += PATH_RELAY_SET;
    WebServer->httpServer.on(path, HTTP_GET, std::bind(&SuplaWebPageRelay::handleRelaySetMCP23017, this));

    path = PATH_START;
    path += PATH_SAVE_RELAY_SET;
    WebServer->httpServer.on(path, HTTP_POST, std::bind(&SuplaWebPageRelay::handleRelaySaveSetMCP23017, this));
  }
  else {
    for (uint8_t i = 1; i <= ConfigManager->get(KEY_MAX_RELAY)->getValueInt(); i++) {
      path = PATH_START;
      path += PATH_RELAY_SET;
      path += i;
      WebServer->httpServer.on(path, HTTP_GET, std::bind(&SuplaWebPageRelay::handleRelaySet, this));

      path = PATH_START;
      path += PATH_SAVE_RELAY_SET;
      path += i;
      WebServer->httpServer.on(path, HTTP_POST, std::bind(&SuplaWebPageRelay::handleRelaySaveSet, this));
    }
  }
}

void SuplaWebPageRelay::handleRelay() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  supla_webpage_relay(0);
}

void SuplaWebPageRelay::handleRelaySave() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  uint8_t nr, last_value;

  last_value = ConfigManager->get(KEY_MAX_RELAY)->getValueInt();
  for (nr = 1; nr <= last_value; nr++) {
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

  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_RELAY).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_RELAY, WebServer->httpServer.arg(INPUT_MAX_RELAY).c_str());
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      if (last_value >= ConfigManager->get(KEY_MAX_RELAY)->getValueInt()) {
        supla_webpage_relay(1);
      }
      else {
        supla_webpage_relay(2);
        ConfigESP->rebootESP();
      }
      break;
    case E_CONFIG_FILE_OPEN:
      supla_webpage_relay(2);
      break;
  }
}

void SuplaWebPageRelay::supla_webpage_relay(int save) {
  uint8_t nr, countFreeGpio;

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_RELAY);

  addForm(webContentBuffer, F("post"), PATH_SAVE_RELAY);
  addFormHeader(webContentBuffer, S_GPIO_SETTINGS_FOR_RELAYS);

  if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt() != FUNCTION_OFF) {
    countFreeGpio = 16;
  }
  else {
    countFreeGpio = ConfigESP->countFreeGpio(FUNCTION_RELAY);
  }

  addNumberBox(webContentBuffer, INPUT_MAX_RELAY, S_QUANTITY, KEY_MAX_RELAY, countFreeGpio);

  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_RELAY)->getValueInt(); nr++) {
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt() != FUNCTION_OFF) {
      addListMCP23017GPIOLinkBox(webContentBuffer, INPUT_RELAY_GPIO, S_RELAY, FUNCTION_RELAY, PATH_RELAY_SET, nr);
    }
    else {
      addListGPIOLinkBox(webContentBuffer, INPUT_RELAY_GPIO, S_RELAY, FUNCTION_RELAY, PATH_RELAY_SET, nr);
    }
  }
  addFormHeaderEnd(webContentBuffer);

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);

  WebServer->sendContent();
}

void SuplaWebPageRelay::handleRelaySet() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  supla_webpage_relay_set(0);
}

void SuplaWebPageRelay::handleRelaySaveSet() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  String readUrl, nr_relay, input, path;
  uint8_t place, key, gpio;

  input.reserve(6);
  readUrl.reserve(11);
  nr_relay.reserve(2);
  path.reserve(15);

  path = PATH_START;
  path += PATH_SAVE_RELAY_SET;
  readUrl = WebServer->httpServer.uri();

  place = readUrl.indexOf(path);
  nr_relay = readUrl.substring(place + path.length(), place + path.length() + 3);

  gpio = ConfigESP->getGpio(nr_relay.toInt(), FUNCTION_RELAY);
  key = KEY_GPIO + gpio;

  input = INPUT_RELAY_MEMORY;
  input += nr_relay;
  ConfigManager->setElement(key, MEMORY, WebServer->httpServer.arg(input).toInt());

  input = INPUT_RELAY_LEVEL;
  input += nr_relay;
  ConfigManager->setElement(key, LEVEL, WebServer->httpServer.arg(input).toInt());

#if defined(SUPLA_PUSHOVER)
  input = INPUT_PUSHOVER;
  ConfigManager->setElement(KEY_PUSHOVER, (nr_relay.toInt() - 1), WebServer->httpServer.arg(input).toInt());
#endif

  input = INPUT_CONDITIONS_SENSOR_TYPE;
  ConfigManager->setElement(KEY_CONDITIONS_SENSOR_TYPE, (nr_relay.toInt() - 1), WebServer->httpServer.arg(input).toInt());
  input = INPUT_CONDITIONS_TYPE;
  ConfigManager->setElement(KEY_CONDITIONS_TYPE, (nr_relay.toInt() - 1), WebServer->httpServer.arg(input).toInt());
  input = INPUT_CONDITIONS_MIN;
  ConfigManager->setElement(KEY_CONDITIONS_MIN, (nr_relay.toInt() - 1), WebServer->httpServer.arg(input).c_str());
  input = INPUT_CONDITIONS_MAX;
  ConfigManager->setElement(KEY_CONDITIONS_MAX, (nr_relay.toInt() - 1), WebServer->httpServer.arg(input).c_str());
  if (nr_relay.toInt() <= MAX_PUSHOVER_MESSAGE) {
    input = INPUT_PUSHOVER_MESSAGE;
    ConfigManager->setElement(KEY_PUSHOVER_MASSAGE, (nr_relay.toInt() - 1), WebServer->httpServer.arg(input).c_str());
  }

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
  String path, readUrl, nr_relay;
  uint8_t place, selected;

  path.reserve(10);
  readUrl.reserve(11);
  nr_relay.reserve(2);

  webContentBuffer += SuplaSaveResult(save);

  if (nr != 0) {
    webContentBuffer += SuplaJavaScript(PATH_RELAY_SET + String(nr));
    nr_relay = nr;
  }
  else {
    path = PATH_START;
    path += PATH_RELAY_SET;
    readUrl = WebServer->httpServer.uri();

    place = readUrl.indexOf(path);
    nr_relay = readUrl.substring(place + path.length(), place + path.length() + 3);
    webContentBuffer += SuplaJavaScript(String(PATH_RELAY_SET + nr_relay));
  }

  addForm(webContentBuffer, F("post"), PATH_SAVE_RELAY_SET + nr_relay);
  addFormHeader(webContentBuffer, S_RELAY_NR_SETTINGS + nr_relay);

  selected = ConfigESP->getLevel(nr_relay.toInt(), FUNCTION_RELAY);
  addListBox(webContentBuffer, INPUT_RELAY_LEVEL + nr_relay, S_STATE_CONTROL, LEVEL_P, 2, selected);

  selected = ConfigESP->getMemory(nr_relay.toInt(), FUNCTION_RELAY);
  addListBox(webContentBuffer, INPUT_RELAY_MEMORY + nr_relay, S_REACTION_AFTER_RESET, MEMORY_P, 3, selected);

#if defined(SUPLA_PUSHOVER)
  selected = ConfigManager->get(KEY_PUSHOVER)->getElement(nr_relay.toInt() - 1).toInt();
  addListBox(webContentBuffer, INPUT_PUSHOVER, F("Pushover"), STATE_P, 2, selected);

  if (nr_relay.toInt() <= MAX_PUSHOVER_MESSAGE) {
    String pushoverMassage = ConfigManager->get(KEY_PUSHOVER_MASSAGE)->getElement(nr_relay.toInt() - 1).c_str();
    addTextBox(webContentBuffer, INPUT_PUSHOVER_MESSAGE, "Wiadomość", pushoverMassage, 0, 16, false);
  }
#endif
  addFormHeaderEnd(webContentBuffer);

  addFormHeader(webContentBuffer, F("Warunkowanie"));

  selected = ConfigManager->get(KEY_CONDITIONS_SENSOR_TYPE)->getElement(nr_relay.toInt() - 1).toInt();
  addListBox(webContentBuffer, INPUT_CONDITIONS_SENSOR_TYPE, F("Sensor"), SENSOR_LIST_P, COUNT_SENSOR_LIST, selected);

  selected = ConfigManager->get(KEY_CONDITIONS_TYPE)->getElement(nr_relay.toInt() - 1).toInt();
  addListBox(webContentBuffer, INPUT_CONDITIONS_TYPE, F("Warunek"), CONDITIONS_TYPE_P, 4, selected);

  String value = ConfigManager->get(KEY_CONDITIONS_MIN)->getElement(nr_relay.toInt() - 1);
  addNumberBox(webContentBuffer, INPUT_CONDITIONS_MIN, F("ON"), F("wartość włączenia"), false, value);
  value = ConfigManager->get(KEY_CONDITIONS_MAX)->getElement(nr_relay.toInt() - 1);
  addNumberBox(webContentBuffer, INPUT_CONDITIONS_MAX, F("OFF"), F("wartość wyłączenia"), false, value);
  addFormHeaderEnd(webContentBuffer);

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_RELAY);

  WebServer->sendContent();
}

void SuplaWebPageRelay::handleRelaySetMCP23017() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  supla_webpage_relay_set_MCP23017(0);
}

void SuplaWebPageRelay::supla_webpage_relay_set_MCP23017(int save) {
  uint8_t selected;

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_RELAY);

  addForm(webContentBuffer, F("post"), PATH_SAVE_RELAY_SET);
  addFormHeader(webContentBuffer, F("Ustawienia dla przekaźników"));

  selected = ConfigESP->getLevel(1, FUNCTION_RELAY);
  addListBox(webContentBuffer, INPUT_RELAY_LEVEL, S_STATE_CONTROL, LEVEL_P, 2, selected);

  selected = ConfigESP->getMemory(1, FUNCTION_RELAY);
  addListBox(webContentBuffer, INPUT_RELAY_MEMORY, S_REACTION_AFTER_RESET, MEMORY_P, 3, selected);

  addFormHeaderEnd(webContentBuffer);
  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_RELAY);

  WebServer->sendContent();
}

void SuplaWebPageRelay::handleRelaySaveSetMCP23017() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  String input;
  uint8_t key, gpio, memory, level;

  input.reserve(9);

  input = INPUT_RELAY_MEMORY;
  memory = WebServer->httpServer.arg(input).toInt();

  input = INPUT_RELAY_LEVEL;
  level = WebServer->httpServer.arg(input).toInt();

  for (gpio = 0; gpio <= OFF_GPIO; gpio++) {
    key = KEY_GPIO + gpio;
    ConfigManager->setElement(key, MEMORY, memory);
    ConfigManager->setElement(key, LEVEL_RELAY, level);
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
#endif
