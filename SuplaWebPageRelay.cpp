#include "SuplaWebPageRelay.h"
#include "SuplaDeviceGUI.h"
#include "SuplaWebServer.h"
#include "SuplaCommonPROGMEM.h"

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
  String key, input;
  uint8_t nr, current_value, last_value;

  last_value = ConfigManager->get(KEY_MAX_RELAY)->getValueInt();
  current_value = WebServer->httpServer.arg(INPUT_MAX_RELAY).toInt();

  if (last_value > 0) {
    for (nr = 1; nr <= last_value; nr++) {
      input = INPUT_RELAY_GPIO;
      input += nr;
      key = GPIO;
      key += WebServer->httpServer.arg(input).toInt();
      if (WebServer->httpServer.arg(input).toInt() != OFF_GPIO) {
        key = GPIO;
        key += WebServer->httpServer.arg(input).toInt();
        if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_OFF) {
          ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), nr, FUNCTION_RELAY, 1);
        }
        else if (ConfigESP->getGpio(nr, FUNCTION_RELAY) == WebServer->httpServer.arg(input).toInt() &&
                 ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_RELAY) {
          ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), nr, FUNCTION_RELAY, ConfigESP->getLevel(nr, FUNCTION_RELAY));
        }
        else {
          WebServer->sendContent(supla_webpage_relay(6));
          return;
        }
      }
      if (ConfigESP->getGpio(nr, FUNCTION_RELAY) != WebServer->httpServer.arg(input).toInt() ||
          WebServer->httpServer.arg(input).toInt() == OFF_GPIO || ConfigManager->get(key.c_str())->getElement(NR).toInt() > current_value) {
        ConfigESP->clearGpio(ConfigESP->getGpio(nr, FUNCTION_RELAY));
      }
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
  String key;
  uint8_t selected, suported, nr;

  String pagerelay = "";
  pagerelay += WebServer->SuplaSaveResult(save);
  pagerelay += WebServer->SuplaJavaScript(PATH_RELAY);
  pagerelay += F("<form method='post' action='");
  pagerelay += PATH_SAVE_RELAY;
  pagerelay += F("'><div class='w'><h3>Ustawienie GPIO dla przekaźników</h3>");
  pagerelay += F("<i><label>ILOŚĆ</label><input name='");
  pagerelay += INPUT_MAX_RELAY;
  pagerelay += F("' type='number' placeholder='0' step='1' min='0' max='");
  pagerelay += ConfigESP->countFreeGpio(FUNCTION_RELAY);
  pagerelay += F("' value='");
  pagerelay += String(ConfigManager->get(KEY_MAX_RELAY)->getValue());
  pagerelay += F("'></i>");
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_RELAY)->getValueInt(); nr++) {
    pagerelay += F("<i><label>");
    selected = ConfigESP->getGpio(nr, FUNCTION_RELAY);
    if (selected != OFF_GPIO) {
      pagerelay += F("<a href='");
      pagerelay += PATH_START;
      pagerelay += PATH_RELAY_SET;
      pagerelay += nr;
      pagerelay += F("'>");
    }
    pagerelay += nr;
    pagerelay += F(". PRZEKAŹNIK ");
    if (selected != OFF_GPIO) {
      pagerelay += WebServer->SuplaIconEdit();
      pagerelay += F("</a>");
    }
    pagerelay += F("</label>");
    pagerelay += WebServer->selectGPIO(INPUT_RELAY_GPIO, FUNCTION_RELAY, nr);
    pagerelay += F("</i>");
  }
  pagerelay += F("</div><button type='submit'>Zapisz</button></form>");
  pagerelay += F("<br>");
  pagerelay += F("<a href='");
  pagerelay += PATH_START;
  pagerelay += PATH_DEVICE_SETTINGS;
  pagerelay += F("'><button>Powrót</button></a></div>");
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

  String readUrl, nr_relay, key, input;
  uint8_t place;

  String path = PATH_START;
  path += PATH_SAVE_RELAY_SET;
  readUrl = WebServer->httpServer.uri();

  place = readUrl.indexOf(path);
  nr_relay = readUrl.substring(place + path.length(), place + path.length() + 3);
  key = GPIO;
  key += ConfigESP->getGpio(nr_relay.toInt(), FUNCTION_RELAY);

  input = INPUT_RELAY_MEMORY;
  input += nr_relay;
  ConfigManager->setElement(key.c_str(), MEMORY, WebServer->httpServer.arg(input).toInt());

  input = INPUT_RELAY_LEVEL;
  input += nr_relay;
  ConfigManager->setElement(key.c_str(), LEVEL, WebServer->httpServer.arg(input).toInt());

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
    page += F("'><div class='w'><h3>Ustawienia przekaźnika nr. ");
    page += nr_relay;
    page += F("</h3>");
    page += F("<i><label>");
    page += F("Sterowanie</label><select name='");
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
    page += F("Pamięć po resecie</label><select name='");
    page += INPUT_RELAY_MEMORY;
    page += nr_relay;
    page += F("'>");
    selected = ConfigESP->getMemoryRelay(nr_relay.toInt());
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
    page += F("</div><button type='submit'>Zapisz</button></form>");
  }
  else {
    page += F("<div class='w'><h3>Brak przekaźnika nr. ");
    page += nr_relay;
    page += F("</h3>");
  }
  page += F("<br>");
  page += F("<a href='");
  page += PATH_START;
  page += PATH_RELAY;
  page += F("'><button>Powrót</button></a></div>");

  return page;
}
#endif
