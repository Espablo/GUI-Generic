#include "SuplaWebServer.h"
#include "SuplaDeviceGUI.h"
#include "SuplaWebPageRelay.h"

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

  for(int i = 1; i <= MAX_GPIO; i++){
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
  WebServer->httpServer.send(200, "text/html", supla_webpage_relay(0));
}

void SuplaWebPageRelay::handleRelaySave() {
  Serial.println(F("HTTP_POST - metoda handleRelaySave"));

  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  String key, input, key_max, input_max;
  int nr, set_input,get_input, current_value, last_value;
  
  last_value = ConfigManager->get(KEY_MAX_RELAY )->getValueInt();
  
  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_RELAY).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_RELAY, WebServer->httpServer.arg(INPUT_MAX_RELAY).c_str());
  }
  current_value = ConfigManager->get(KEY_MAX_RELAY)->getValueInt();
 
  if(current_value >= last_value){
    for(nr = 1; nr <= last_value; nr++){
      key = KEY_RELAY_GPIO;
      key += nr;
      input = INPUT_RELAY_GPIO;
      input += nr;
      set_input = WebServer->httpServer.arg(input).toInt();
      get_input = ConfigManager->get(key.c_str())->getValueInt();
      if(get_input != set_input){
        if(WebServer->getBusyGpio(set_input) == false){
          ConfigManager->set(key.c_str(), WebServer->httpServer.arg(input).c_str());
          WebServer->setBusyGpio(get_input, false);
          WebServer->setBusyGpio(set_input, true);
        }
        else {
          Serial.println(F("ERROR!!!"));
          WebServer->httpServer.send(200, "text/html", supla_webpage_relay(6));
          return;
        }
      } 
    }
  }
  else if(current_value < last_value){
    for(nr = current_value + 1; nr <= last_value; nr++){
      key = KEY_RELAY_GPIO;
      key += nr;
      get_input = ConfigManager->get(key.c_str())->getValueInt();
      ConfigManager->set(key.c_str(), "17");
      WebServer->setBusyGpio(get_input, false);
    }
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      Serial.println(F("E_CONFIG_OK: Config save"));
      WebServer->sendContent(supla_webpage_relay(1));
      break;
    case E_CONFIG_FILE_OPEN:
      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      WebServer->httpServer.send(200, "text/html", supla_webpage_relay(2));
      break;  
  }
}

String SuplaWebPageRelay::supla_webpage_relay(int save) {
  String key;
  int selected, suported, nr;
  
  String pagerelay = "";
  pagerelay += WebServer->SuplaMetas();
  pagerelay += WebServer->SuplaStyle();
  pagerelay += WebServer->SuplaSaveResult(save);
  pagerelay += F("</div>");
  pagerelay += WebServer->SuplaJavaScript(PATH_RELAY);
  pagerelay += F("<div class='s'>");
  pagerelay += WebServer->SuplaLogo();
  pagerelay += WebServer->SuplaSummary();
  pagerelay += F("<form method='post' action='");
  pagerelay += PATH_SAVE_RELAY;
  pagerelay += F("'><div class='w'><h3>Ustawienie GPIO dla przekaźników</h3>");
  pagerelay += F("<i><label>ILOŚĆ</label><input name='");
  pagerelay += INPUT_MAX_RELAY;
  pagerelay += F("' type='number' placeholder='0' step='1' min='0' max='");
  pagerelay += MAX_GPIO - ConfigManager->get(KEY_MAX_BUTTON)->getValueInt() - ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt() - ConfigManager->get(KEY_MAX_DHT11)->getValueInt() - ConfigManager->get(KEY_MAX_DHT22)->getValueInt();
  pagerelay += F("' value='");
  pagerelay += String(ConfigManager->get(KEY_MAX_RELAY)->getValue());
  pagerelay += F("'></i>");
  for(nr = 1; nr <= ConfigManager->get(KEY_MAX_RELAY)->getValueInt(); nr++){
    pagerelay += F("<i><label>");
    pagerelay += F("<a href='");
    pagerelay += PATH_START;
    pagerelay += PATH_RELAY_SET;
    pagerelay += nr;
    pagerelay += F("'>");
    pagerelay += nr;
    pagerelay += F(". PRZEKAŹNIK</a></label><select name='");
    pagerelay += INPUT_RELAY_GPIO;
    pagerelay += nr;
    pagerelay += F("'>");
    key = KEY_RELAY_GPIO;
    key += nr;
    selected = ConfigManager->get(key.c_str())->getValueInt();
    for (suported = 0; suported < sizeof(WebServer->Supported_Gpio) / sizeof(char*); suported++) {
      if(WebServer->getBusyGpio(suported) == false || selected == suported){
          pagerelay += F("<option value='");
          pagerelay += suported;
          if (selected == suported) {
            pagerelay += F("' selected>");
          }
          else pagerelay += F("'>");
          pagerelay += (WebServer->Supported_Gpio[suported][0]);
      }
    }
    pagerelay += F("</select></i>");    
  }
  pagerelay += F("</div><button type='submit'>Zapisz</button></form>");
  pagerelay += F("<br><br>");
  pagerelay += F("<a href='/'><button>Powrót</button></a></div>");
  return pagerelay;
}
void SuplaWebPageRelay::handleRelaySet() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  WebServer->httpServer.send(200, "text/html", supla_webpage_relay_set(0));
}

void SuplaWebPageRelay::handleRelaySaveSet() {
  Serial.println(F("HTTP_POST - metoda handleRelaySaveSet"));
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  String readUrl, nr_relay, key, input;
  int place, selected, suported, set_input;
  
  String path = PATH_START;
  path += PATH_SAVE_RELAY_SET;
  readUrl = WebServer->httpServer.uri();
  
  place = readUrl.indexOf(path);
  nr_relay = readUrl.substring(place + path.length(), place + path.length()+3);

  key = KEY_RELAY_LEVEL;
  key += nr_relay;
  input = INPUT_RELAY_LEVEL;  
  input += nr_relay;
  ConfigManager->set(key.c_str(), WebServer->httpServer.arg(input).c_str());
  key = KEY_RELAY_MEMORY;
  key += nr_relay;
  input = INPUT_RELAY_MEMORY;  
  input += nr_relay;
  ConfigManager->set(key.c_str(), WebServer->httpServer.arg(input).c_str());
  key = KEY_RELAY_DURATION;
  key += nr_relay;
  input = INPUT_RELAY_DURATION;  
  input += nr_relay;
  ConfigManager->set(key.c_str(), WebServer->httpServer.arg(input).c_str());

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      Serial.println(F("E_CONFIG_OK: Dane zapisane"));
      WebServer->sendContent(supla_webpage_relay(1));
      break;

    case E_CONFIG_FILE_OPEN:
      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      WebServer->httpServer.send(200, "text/html", supla_webpage_relay(2));
      break;
  }
}

String SuplaWebPageRelay::supla_webpage_relay_set(int save) {

  String readUrl, nr_relay, key;
  int place, selected, suported;
  
  String path = PATH_START;
  path += PATH_RELAY_SET;
  readUrl = WebServer->httpServer.uri();
  
  place = readUrl.indexOf(path);
  nr_relay = readUrl.substring(place + path.length(), place + path.length()+3);
  
  String page = "";
  page += WebServer->SuplaMetas();
  page += WebServer->SuplaStyle();
  page += WebServer->SuplaSaveResult(save);
  page += F("</div>");
  page += WebServer->SuplaJavaScript(PATH_RELAY);
  page += F("<div class='s'>");
  page += WebServer->SuplaLogo();
  page += WebServer->SuplaSummary();
  int relays = ConfigManager->get(KEY_MAX_RELAY)->getValueInt();
  if(nr_relay.toInt() <= relays){
    page += F("<form method='post' action='");
    page += PATH_SAVE_RELAY_SET;
    page += nr_relay;
    page += F("'><div class='w'><h3>Ustawienia przekaźnika nr. ");
    page += nr_relay;
    page += F("</h3>");
    page += F("<i><label>");
    page += F("Sterowanie stanem</label><select name='");
    page += INPUT_RELAY_LEVEL;
    page += nr_relay;
    page += F("'>");
    key = KEY_RELAY_LEVEL;
    key += nr_relay;
    selected = ConfigManager->get(key.c_str())->getValueInt();
    for (suported = 0; suported < sizeof(WebServer->Supported_Level) / sizeof(char*); suported++) {
        page += F("<option value='");
        page += suported;
        if (selected == suported) {
          page += F("' selected>");
        }
        else page += F("'>");
        page += (WebServer->Supported_Level[suported]);
    }
    page += F("</select></i>");
    page += F("<i><label>");
    page += F("Pamięć po resecie</label><select name='");
    page += INPUT_RELAY_MEMORY;
    page += nr_relay;
    page += F("'>");
    key = KEY_RELAY_MEMORY;
    key += nr_relay;
    selected = ConfigManager->get(key.c_str())->getValueInt();
    for (suported = 0; suported < sizeof(Supported_RelayMemory) / sizeof(char*); suported++) {
        page += F("<option value='");
        page += suported;
        if (selected == suported) {
          page += F("' selected>");
        }
        else page += F("'>");
        page += (Supported_RelayMemory[suported]);
    }
    page += F("</select></i>");
    page += F("<i><label>");
    page += F("Czas ząłączenia</label><select name='");
    page += INPUT_RELAY_DURATION;
    page += nr_relay;
    page += F("'>");
    key = KEY_RELAY_DURATION;
    key += nr_relay;
    selected = ConfigManager->get(key.c_str())->getValueInt();
    for (suported = 0; suported < sizeof(WebServer->Supported_Level) / sizeof(char*); suported++) {
        page += F("<option value='");
        page += suported;
        if (selected == suported) {
          page += F("' selected>");
        }
        else page += F("'>");
        page += (WebServer->Supported_Level[suported]);
    }
    page += F("</select></i>");
    page += F("</div><button type='submit'>Zapisz</button></form>");
    page += F("<br><br>");
  }
  else {
    page += F("<div class='w'><h3>Brak przekaźnika nr. ");
    page += nr_relay;
    page += F("</h3>");
  }
  page += F("<a href='/'><button>Powrót</button></a></div>");
  
  return page;
}
