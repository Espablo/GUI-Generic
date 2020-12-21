#include "SuplaWebPageRelay.h"
#include "SuplaDeviceGUI.h"
#include "SuplaWebServer.h"
#include "SuplaCommonPROGMEM.h"
#include "GUIGenericCommon.h"
#include "Markup.h"

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
SuplaWebPageRelay *WebPageRelay = new SuplaWebPageRelay();

SuplaWebPageRelay::SuplaWebPageRelay() {
}

void SuplaWebPageRelay::createWebPageRelay() {
  String path;
  path += PATH_START;
  path += PATH_RELAY;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageRelay::handleRelay, this));
  path = PATH_START;
  path += PATH_SAVE_RELAY;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageRelay::handleRelaySave, this));

  for (uint8_t i = 1; i <= MAX_GPIO; i++) {
    path = PATH_START;
    path += PATH_RELAY_SET;
    path += i;
    WebServer->httpServer.on(path, std::bind(&SuplaWebPageRelay::handleRelaySet, this));

    path = PATH_START;
    path += PATH_SAVE_RELAY_SET;
    path += i;
    WebServer->httpServer.on(path, std::bind(&SuplaWebPageRelay::handleRelaySaveSet, this));
  }
}

void SuplaWebPageRelay::handleRelay() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  WebServer->sendContent(supla_webpage_relay(0));
}

void SuplaWebPageRelay::handleRelaySave() {
  //  Serial.println(F("HTTP_POST - metoda handleRelaySave"));

  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  uint8_t nr, last_value;

  last_value = ConfigManager->get(KEY_MAX_RELAY)->getValueInt();
  for (nr = 1; nr <= last_value; nr++) {
    if (!WebServer->saveGPIO(INPUT_RELAY_GPIO, FUNCTION_RELAY, nr, INPUT_MAX_RELAY)) {
      WebServer->sendContent(supla_webpage_relay(6));
      return;
    }
  }

  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_RELAY).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_RELAY, WebServer->httpServer.arg(INPUT_MAX_RELAY).c_str());
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      //      Serial.println(F("E_CONFIG_OK: Config save"));
      WebServer->sendContent(supla_webpage_relay(1));
      break;
    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      WebServer->sendContent(supla_webpage_relay(2));
      break;
  }
}

String SuplaWebPageRelay::supla_webpage_relay(int save) {
  uint8_t selected, suported, nr;

  String pagerelay = "";
  pagerelay += WebServer->SuplaSaveResult(save);
  pagerelay += WebServer->SuplaJavaScript(PATH_RELAY);
  pagerelay += F("<form method='post' action='");
  pagerelay += PATH_SAVE_RELAY;
  pagerelay += F("'>");
  addFormHeader(pagerelay, String(S_GPIO_SETTINGS_FOR_RELAYS));
  addNumberBox(pagerelay, INPUT_MAX_RELAY, S_QUANTITY, KEY_MAX_RELAY, ConfigESP->countFreeGpio(FUNCTION_RELAY));
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_RELAY)->getValueInt(); nr++) {
    addListGPIOLinkBox(pagerelay, INPUT_RELAY_GPIO, S_RELAY, FUNCTION_RELAY, PATH_RELAY_SET, nr);
  }
  addFormHeaderEnd(pagerelay);
  pagerelay += F("<button type='submit'>");
  pagerelay += S_SAVE;
  pagerelay += F("</button></form>");
  pagerelay += F("<br>");
  pagerelay += F("<a href='");
  pagerelay += PATH_START;
  pagerelay += PATH_DEVICE_SETTINGS;
  pagerelay += F("'><button>");
  pagerelay += S_RETURN;
  pagerelay += F("</button></a></div>");
  return pagerelay;
}

void SuplaWebPageRelay::handleRelaySet() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  WebServer->sendContent(supla_webpage_relay_set(0));
}

void SuplaWebPageRelay::handleRelaySaveSet() {
  //  Serial.println(F("HTTP_POST - metoda handleRelaySaveSet"));
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  String readUrl, nr_relay, input;
  uint8_t place;

  String path = PATH_START;
  path += PATH_SAVE_RELAY_SET;
  readUrl = WebServer->httpServer.uri();

  place = readUrl.indexOf(path);
  nr_relay = readUrl.substring(place + path.length(), place + path.length() + 3);
  uint8_t key = KEY_GPIO + ConfigESP->getGpio(nr_relay.toInt(), FUNCTION_RELAY);

  input = INPUT_RELAY_MEMORY;
  input += nr_relay;
  ConfigManager->setElement(key, MEMORY, WebServer->httpServer.arg(input).toInt());

  input = INPUT_RELAY_LEVEL;
  input += nr_relay;
  ConfigManager->setElement(key, LEVEL, WebServer->httpServer.arg(input).toInt());

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      //      Serial.println(F("E_CONFIG_OK: Dane zapisane"));
      WebServer->sendContent(supla_webpage_relay(1));
      break;

    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      WebServer->sendContent(supla_webpage_relay(2));
      break;
  }
}

String SuplaWebPageRelay::supla_webpage_relay_set(int save) {
  String readUrl, nr_relay, key;
  uint8_t place, selected, suported;

  String path = PATH_START;
  path += PATH_RELAY_SET;
  readUrl = WebServer->httpServer.uri();

  place = readUrl.indexOf(path);
  nr_relay = readUrl.substring(place + path.length(), place + path.length() + 3);

  String page = "";
  page += WebServer->SuplaSaveResult(save);
  page += WebServer->SuplaJavaScript(PATH_RELAY);
  uint8_t relays = ConfigManager->get(KEY_MAX_RELAY)->getValueInt();
  if (nr_relay.toInt() <= relays && ConfigESP->getGpio(nr_relay.toInt(), FUNCTION_RELAY) != OFF_GPIO) {
    page += F("<form method='post' action='");
    page += PATH_SAVE_RELAY_SET;
    page += nr_relay;
    page += F("'><div class='w'><h3>");
    page += S_RELAY_NR_SETTINGS;
    page += F(" ");
    page += nr_relay;
    page += F("</h3>");
    page += F("<i><label>");
    page += S_STATE_CONTROL;
    page += F("</label><select name='");
    page += INPUT_RELAY_LEVEL;
    page += nr_relay;
    page += F("'>");
    selected = ConfigESP->getLevel(nr_relay.toInt(), FUNCTION_RELAY);
    for (suported = 0; suported < 2; suported++) {
      page += F("<option value='");
      page += suported;
      if (selected == suported) {
        page += F("' selected>");
      }
      else
        page += F("'>");
      page += LevelString(suported);
    }
    page += F("</select></i>");
    page += F("<i><label>");
    page += S_REACTION_AFTER_RESET;
    page += F("</label><select name='");
    page += INPUT_RELAY_MEMORY;
    page += nr_relay;
    page += F("'>");
    selected = ConfigESP->getMemory(nr_relay.toInt(), FUNCTION_RELAY);
    for (suported = 0; suported < 3; suported++) {
      page += F("<option value='");
      page += suported;
      if (selected == suported) {
        page += F("' selected>");
      }
      else
        page += F("'>");
      page += MemoryString(suported);
    }
    page += F("</select></i>");
    page += F("</div><button type='submit'>");
    page += S_SAVE;
    page += F("</button></form>");
  }
  else {
    page += F("<div class='w'><h3>");
    page += S_NO_RELAY_NR;
    page += F(" ");
    page += nr_relay;
    page += F("</h3>");
  }
  page += F("<br>");
  page += F("<a href='");
  page += PATH_START;
  page += PATH_RELAY;
  page += F("'><button>");
  page += S_RETURN;
  page += F("</button></a></div>");

  return page;
}
#endif
