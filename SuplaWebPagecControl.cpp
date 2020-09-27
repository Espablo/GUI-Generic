#include "SuplaWebServer.h"
#include "SuplaDeviceGUI.h"
#include "SuplaWebPageControl.h"

SuplaWebPageControl *WebPageControl = new SuplaWebPageControl();

SuplaWebPageControl::SuplaWebPageControl() {

}

void SuplaWebPageControl::createWebPageControl() {
  String path;
  path += PATH_START;
  path += PATH_CONTROL;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageControl::handleControl, this));
  path = PATH_START;
  path += PATH_SAVE_CONTROL;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageControl::handleControlSave, this));
}

void SuplaWebPageControl::handleControl() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  WebServer->httpServer.send(200, "text/html", supla_webpage_control(0));

}

void SuplaWebPageControl::handleControlSave() {
  Serial.println(F("HTTP_POST - metoda handleControlSave"));

  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  String key, input, key_max, input_max;
  int nr, set_input,get_input, current_value, last_value;
  
  last_value = ConfigManager->get(KEY_MAX_BUTTON )->getValueInt();
  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_BUTTON).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_BUTTON, WebServer->httpServer.arg(INPUT_MAX_BUTTON).c_str());
  }
  current_value = ConfigManager->get(KEY_MAX_BUTTON)->getValueInt();
  if(current_value >= last_value){
    for(nr = 1; nr <= last_value; nr++){
      key = KEY_BUTTON_GPIO;
      input = INPUT_BUTTON_GPIO;
      key += nr;
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
          WebServer->httpServer.send(200, "text/html", supla_webpage_control(6));
          return;
        }
      } 
    }
  }
  else if(current_value < last_value){
    for(nr = current_value + 1; nr <= last_value; nr++){
      key = KEY_BUTTON_GPIO;
      key += nr;
      get_input = ConfigManager->get(key.c_str())->getValueInt();
      ConfigManager->set(key.c_str(), "17");
      WebServer->setBusyGpio(get_input, false);
    }
  }

  last_value = ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt();
  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_LIMIT_SWITCH).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_LIMIT_SWITCH, WebServer->httpServer.arg(INPUT_MAX_LIMIT_SWITCH).c_str());
  }
  current_value = ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt();
  if(current_value >= last_value){
    for(nr = 1; nr <= last_value; nr++){
      key = KEY_LIMIT_SWITCH_GPIO;
      input = INPUT_LIMIT_SWITCH_GPIO;
      key += nr;
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
          WebServer->httpServer.send(200, "text/html", supla_webpage_control(6));
          return;
        }
      } 
    }
  }
  else if(current_value < last_value){
    for(nr = current_value + 1; nr <= last_value; nr++){
      key = KEY_LIMIT_SWITCH_GPIO;
      key += nr;
      get_input = ConfigManager->get(key.c_str())->getValueInt();
      ConfigManager->set(key.c_str(), "17");
      WebServer->setBusyGpio(get_input, false);
    }
  }
  
  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      Serial.println(F("E_CONFIG_OK: Config save"));
      WebServer->sendContent(supla_webpage_control(1));
      break;
    case E_CONFIG_FILE_OPEN:
      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      WebServer->httpServer.send(200, "text/html", supla_webpage_control(2));
      break;  
  }
}

String SuplaWebPageControl::supla_webpage_control(int save) {
  int nr, suported, selected;
  String pagebutton,key;
  
  pagebutton += WebServer->SuplaMetas();
  pagebutton += WebServer->SuplaStyle();
  pagebutton += WebServer->SuplaSaveResult(save);
  pagebutton += F("</div>");
  pagebutton += WebServer->SuplaJavaScript(PATH_CONTROL);
  pagebutton += F("<div class='s'>");
  pagebutton += WebServer->SuplaLogo();
  pagebutton += WebServer->SuplaSummary();
  pagebutton += F("<form method='post' action='");
  pagebutton += PATH_SAVE_CONTROL;
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
    pagebutton += nr;
    pagebutton += F(". PRZYCISK</label><select name='");
    pagebutton += INPUT_BUTTON_GPIO;
    pagebutton += nr;
    pagebutton += F("'>");
    key = KEY_BUTTON_GPIO;
    key += nr;
    selected = ConfigManager->get(key.c_str())->getValueInt();
    for (suported = 0; suported < sizeof(WebServer->Supported_Gpio) / sizeof(char*); suported++) {
      if(WebServer->Supported_Gpio[suported][1] == "0" || selected == suported){
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
    key = KEY_LIMIT_SWITCH_GPIO;
    key += nr;
    selected = ConfigManager->get(key.c_str())->getValueInt();
    for (suported = 0; suported < sizeof(WebServer->Supported_Gpio) / sizeof(char*); suported++) {
      if(WebServer->Supported_Gpio[suported][1] == "0" || selected == suported){
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
  pagebutton += F("</div><button type='submit'>Zapisz</button></form>");
  pagebutton += F("<br><br>");
  pagebutton += F("<a href='/'><button>Powrót</button></a></div>");
  return pagebutton;
}
