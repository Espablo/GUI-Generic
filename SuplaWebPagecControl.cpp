#include "SuplaWebServer.h"
#include "SuplaDeviceGUI.h"
#include "SuplaWebPageControl.h"

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
  for(uint8_t i = 1; i <= MAX_GPIO; i++){
    path = PATH_START;
    path += PATH_BUTTON_SET;
    path += i;
    WebServer->httpServer.on(path, std::bind(&SuplaWebPageControl::handleButtonSet, this));
    
    path = PATH_START;
    path += PATH_SAVE_BUTTON_SET;
    path += i;
    WebServer->httpServer.on(path, std::bind(&SuplaWebPageControl::handleButtonSaveSet, this));
  }
  #endif
}

void SuplaWebPageControl::handleControl() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  WebServer->httpServer.send(200, "text/html", supla_webpage_control(0));

}

void SuplaWebPageControl::handleControlSave() {
//  Serial.println(F("HTTP_POST - metoda handleControlSave"));

  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  String key, input;
  uint8_t nr, current_value, last_value;
  #ifdef SUPLA_BUTTON
  last_value = ConfigManager->get(KEY_MAX_BUTTON )->getValueInt();
  current_value = WebServer->httpServer.arg(INPUT_MAX_BUTTON).toInt();

  if(last_value > 0){
    for(nr = 1; nr <= last_value; nr++){
      input = INPUT_BUTTON_GPIO;
      input += nr;
      key = GPIO;
      key += WebServer->httpServer.arg(input).toInt();
      if (WebServer->httpServer.arg(input).toInt() != OFF_GPIO) {
        key = GPIO;
        key += WebServer->httpServer.arg(input).toInt();  
        if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_OFF ||
            (ConfigManager->get(key.c_str())->getElement(PIN).toInt() == WebServer->httpServer.arg(input).toInt() && 
              ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_BUTTON)) {

          ConfigManager->setElement(key.c_str(), NR, nr);
          ConfigManager->setElement(key.c_str(), FUNCTION, FUNCTION_BUTTON);
          ConfigManager->setElement(key.c_str(), LEVEL, 2);
        }
        else {
          WebServer->httpServer.send(200, "text/html", supla_webpage_control(6));
          return;
        }
      }
      if(ConfigESP->getGpio(nr, FUNCTION_BUTTON) != WebServer->httpServer.arg(input).toInt() || 
                          WebServer->httpServer.arg(input).toInt() == OFF_GPIO || 
                          ConfigManager->get(key.c_str())->getElement(NR).toInt() > current_value){
        key = GPIO;
        key += ConfigESP->getGpio(nr, FUNCTION_BUTTON);
        ConfigManager->setElement(key.c_str(), NR, 0);
        ConfigManager->setElement(key.c_str(), FUNCTION, FUNCTION_OFF );
        ConfigManager->setElement(key.c_str(), LEVEL, 0 );
        ConfigManager->setElement(key.c_str(), MEMORY, 0 );
      }
    }
  }  
  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_BUTTON).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_BUTTON, WebServer->httpServer.arg(INPUT_MAX_BUTTON).c_str());  
  }
  ConfigESP->sort(FUNCTION_BUTTON);
#endif

#ifdef SUPLA_LIMIT_SWITCH
  last_value = ConfigManager->get(KEY_MAX_LIMIT_SWITCH )->getValueInt();
  current_value = WebServer->httpServer.arg(INPUT_MAX_LIMIT_SWITCH).toInt();

  if(last_value > 0){
    for(nr = 1; nr <= last_value; nr++){
      input = INPUT_LIMIT_SWITCH_GPIO;
      input += nr;
      key = GPIO;
      key += WebServer->httpServer.arg(input).toInt();
      if (WebServer->httpServer.arg(input).toInt() != OFF_GPIO) {
        key = GPIO;
        key += WebServer->httpServer.arg(input).toInt();  
        if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_OFF ||
            (ConfigManager->get(key.c_str())->getElement(PIN).toInt() == WebServer->httpServer.arg(input).toInt() && 
              ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_LIMIT_SWITCH)) {

          ConfigManager->setElement(key.c_str(), NR, nr);
          ConfigManager->setElement(key.c_str(), FUNCTION, FUNCTION_LIMIT_SWITCH);
        }
        else {
          WebServer->httpServer.send(200, "text/html", supla_webpage_control(6));
          return;
        }
      }
      if(ConfigESP->getGpio(nr, FUNCTION_LIMIT_SWITCH) != WebServer->httpServer.arg(input).toInt() || 
                          WebServer->httpServer.arg(input).toInt() == OFF_GPIO || 
                          ConfigManager->get(key.c_str())->getElement(NR).toInt() > current_value){
        key = GPIO;
        key += ConfigESP->getGpio(nr, FUNCTION_LIMIT_SWITCH);
        ConfigManager->setElement(key.c_str(), NR, 0);
        ConfigManager->setElement(key.c_str(), FUNCTION, FUNCTION_OFF );
        ConfigManager->setElement(key.c_str(), LEVEL, 0 );
        ConfigManager->setElement(key.c_str(), MEMORY, 0 );
      }
    }
  }  
  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_LIMIT_SWITCH).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_LIMIT_SWITCH, WebServer->httpServer.arg(INPUT_MAX_LIMIT_SWITCH).c_str());  
  }
  ConfigESP->sort(FUNCTION_LIMIT_SWITCH);
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
//      Serial.println(F("E_CONFIG_OK: Config save"));
      WebServer->sendContent(supla_webpage_control(1));
      break;
    case E_CONFIG_FILE_OPEN:
//      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      WebServer->httpServer.send(200, "text/html", supla_webpage_control(2));
      break;  
  }
}

String SuplaWebPageControl::supla_webpage_control(int save) {
  uint8_t nr, suported, selected;
  String pagebutton,key;
  
  pagebutton += WebServer->SuplaMetas();
  pagebutton += WebServer->SuplaStyle();
  pagebutton += WebServer->SuplaSaveResult(save);
  pagebutton += F("</div>");
  pagebutton += WebServer->SuplaJavaScript(PATH_CONTROL);
  pagebutton += F("<div class='s'>");
//  pagebutton += WebServer->SuplaLogo();
//  pagebutton += WebServer->SuplaSummary();
  pagebutton += F("<form method='post' action='");
  pagebutton += PATH_SAVE_CONTROL;

#if (defined(SUPLA_BUTTON) && defined(SUPLA_RELAY)) || (defined(SUPLA_RSUPLA_BUTTONELAY) || defined(SUPLA_ROLLERSHUTTER))
  pagebutton += F("'><div class='w'><h3>Ustawienie GPIO dla przycisków</h3>");
  pagebutton += F("<i><label>ILOŚĆ</label><input name='");
  pagebutton += INPUT_MAX_BUTTON;
  pagebutton += F("' type='number' placeholder='0' step='1' min='0' max='");
  pagebutton += MAX_GPIO - ConfigManager->get(KEY_MAX_RELAY)->getValueInt() - ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt() - ConfigManager->get(KEY_MAX_DHT11)->getValueInt() - ConfigManager->get(KEY_MAX_DHT22)->getValueInt();
  pagebutton += F("' value='");
  pagebutton += String(ConfigManager->get(KEY_MAX_BUTTON)->getValue());
  pagebutton += F("'></i>");
  for(nr = 1; nr <= ConfigManager->get(KEY_MAX_BUTTON)->getValueInt(); nr++){
    pagebutton += F("<i><label>");
    pagebutton += F("<a href='");
    pagebutton += PATH_START;
    pagebutton += PATH_BUTTON_SET;
    pagebutton += nr;
    pagebutton += F("'>");
    pagebutton += nr;
    pagebutton += F(". PRZYCISK</a></label><select name='");
    pagebutton += INPUT_BUTTON_GPIO;
    pagebutton += nr;
    pagebutton += F("'>");
    selected = ConfigESP->getGpio(nr, FUNCTION_BUTTON);
    for (suported = 0; suported < 18; suported++) {
        if(ConfigESP->checkBusy(suported, FUNCTION_BUTTON) == false || selected == suported){
        pagebutton += F("<option value='");
        pagebutton += suported;
        if (selected == suported) {
          pagebutton += F("' selected>");
        }
        else pagebutton += F("'>");
        pagebutton += (WebServer->Supported_Gpio[suported][0]);
      }
    }
    pagebutton += F("</select></i>");    
  }
  pagebutton += F("</div>");
#endif

#ifdef SUPLA_LIMIT_SWITCH
  pagebutton += F("<div class='w'><h3>Ustawienie GPIO dla cz. otwarcia</h3>");
  pagebutton += F("<i><label>ILOŚĆ</label><input name='");
  pagebutton += INPUT_MAX_LIMIT_SWITCH;
  pagebutton += F("' type='number' placeholder='0' step='1' min='0' max='");
  pagebutton += MAX_GPIO - ConfigManager->get(KEY_MAX_RELAY)->getValueInt()- ConfigManager->get(KEY_MAX_BUTTON)->getValueInt() - ConfigManager->get(KEY_MAX_DHT11)->getValueInt() - ConfigManager->get(KEY_MAX_DHT22)->getValueInt();
  pagebutton += F("' value='");
  pagebutton += String(ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValue());
  pagebutton += F("'></i>");
  for(nr = 1; nr <= ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt(); nr++){
    pagebutton += F("<i><label>");
    pagebutton += nr;
    pagebutton += F(". KRAŃCÓWKA</label><select name='");
    pagebutton += INPUT_LIMIT_SWITCH_GPIO;
    pagebutton += nr;
    pagebutton += F("'>");
    selected = ConfigESP->getGpio(nr, FUNCTION_LIMIT_SWITCH);
    for (suported = 0; suported < 18; suported++) {
      if(ConfigESP->checkBusy(suported, FUNCTION_LIMIT_SWITCH) == false || selected == suported){
        pagebutton += F("<option value='");
        pagebutton += suported;
        if (selected == suported) {
          pagebutton += F("' selected>");
        }
        else pagebutton += F("'>");
        pagebutton += (WebServer->Supported_Gpio[suported][0]);
      }
    }
    pagebutton += F("</select></i>");    
  }
  pagebutton += F("</div>");
#endif
  
  pagebutton += F("<button type='submit'>Zapisz</button></form>");
  pagebutton += F("<br><br>");
  pagebutton += F("<a href='/'><button>Powrót</button></a></div>");
  return pagebutton;
}

#if (defined(SUPLA_BUTTON) && defined(SUPLA_RELAY)) || (defined(SUPLA_RSUPLA_BUTTONELAY) || defined(SUPLA_ROLLERSHUTTER))
void SuplaWebPageControl::handleButtonSet() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  WebServer->httpServer.send(200, "text/html", supla_webpage_button_set(0));
}

void SuplaWebPageControl::handleButtonSaveSet() {
//  Serial.println(F("HTTP_POST - metoda handleRelaySaveSet"));
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  String readUrl, nr_button, key, input;
  uint8_t place;
  
  String path = PATH_START;
  path += PATH_SAVE_BUTTON_SET;
  readUrl = WebServer->httpServer.uri();
  
  place = readUrl.indexOf(path);
  nr_button = readUrl.substring(place + path.length(), place + path.length()+3);
  key = GPIO;
  key += ConfigESP->getGpio(nr_button.toInt(), FUNCTION_BUTTON);
  
  input = INPUT_BUTTON_LEVEL;  
  input += nr_button;
  ConfigManager->setElement(key.c_str(), LEVEL, WebServer->httpServer.arg(input).toInt());

  ConfigESP->sort(FUNCTION_BUTTON);

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
//      Serial.println(F("E_CONFIG_OK: Dane zapisane"));
      WebServer->sendContent(supla_webpage_control(1));
      break;

    case E_CONFIG_FILE_OPEN:
//      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      WebServer->httpServer.send(200, "text/html", supla_webpage_control(2));
      break;
  }
}

String SuplaWebPageControl::supla_webpage_button_set(int save) {

  String readUrl, nr_button, key;
  uint8_t place, selected, suported;
  
  String path = PATH_START;
  path += PATH_BUTTON_SET;
  readUrl = WebServer->httpServer.uri();
  
  place = readUrl.indexOf(path);
  nr_button = readUrl.substring(place + path.length(), place + path.length()+3);
  
  String page = "";
  page += WebServer->SuplaMetas();
  page += WebServer->SuplaStyle();
  page += WebServer->SuplaSaveResult(save);
  page += F("</div>");
  page += WebServer->SuplaJavaScript(PATH_CONTROL);
  page += F("<div class='s'>");
//  page += WebServer->SuplaLogo();
//  page += WebServer->SuplaSummary();
  uint8_t buttons = ConfigManager->get(KEY_MAX_BUTTON)->getValueInt();
  if(nr_button.toInt() <= buttons && ConfigESP->getGpio(nr_button.toInt(), FUNCTION_BUTTON) != OFF_GPIO){
    page += F("<form method='post' action='");
    page += PATH_SAVE_BUTTON_SET;
    page += nr_button;
    page += F("'><div class='w'><h3>Ustawienia przycisku nr. ");
    page += nr_button;
    page += F("</h3>");
    page += F("<i><label>");
    page += F("Reakcja na</label><select name='");
    page += INPUT_BUTTON_LEVEL;
    page += nr_button;
    page += F("'>");
    selected = ConfigESP->getLevel(nr_button.toInt(), FUNCTION_BUTTON);
    for (suported = 0; suported < sizeof(SupportedTrigger) / sizeof(char*); suported++) {
      page += F("<option value='");
      page += suported;
      if (selected == suported) {
        page += F("' selected>");
      }
      else page += F("'>");
      page += (SupportedTrigger[suported]);
    }
    page += F("</select></i>");
    page += F("</div><button type='submit'>Zapisz</button></form>");
    page += F("<br><br>");
  }
  else {
    page += F("<div class='w'><h3>Brak przycisku nr. ");
    page += nr_button;
    page += F("</h3>");
  }
  page += F("<a href='/'><button>Powrót</button></a></div>");
  
  return page;
}
#endif