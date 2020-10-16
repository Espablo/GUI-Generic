#include "SuplaWebServer.h"
#include "SuplaDeviceGUI.h"
#include "SuplaWebPageConfig.h"

SuplaWebPageConfig *WebPageConfig = new SuplaWebPageConfig();

SuplaWebPageConfig::SuplaWebPageConfig() {

}

void SuplaWebPageConfig::createWebPageConfig() {
  String path;
  path = PATH_START;
  path += PATH_CONFIG;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageConfig::handleConfig, this));
  path = PATH_START;
  path += PATH_SAVE_CONFIG;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageConfig::handleConfigSave, this));
}

void SuplaWebPageConfig::handleConfig() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  WebServer->httpServer.send(200, "text/html", supla_webpage_config(0));

}

void SuplaWebPageConfig::handleConfigSave() {
//  Serial.println(F("HTTP_POST - metoda handleConfigSave"));

  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  String key, input;


  input = INPUT_CFG_LED_GPIO;
  key = GPIO;
  key += WebServer->httpServer.arg(input).toInt();
  if (WebServer->httpServer.arg(input).toInt() != OFF_GPIO) {
    key = GPIO;
    key += WebServer->httpServer.arg(input).toInt();  
    if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_OFF ||
        (ConfigManager->get(key.c_str())->getElement(PIN).toInt() == WebServer->httpServer.arg(input).toInt() && 
          ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_CFG_LED)) {

      ConfigManager->setElement(key.c_str(), NR, 1);
      ConfigManager->setElement(key.c_str(), FUNCTION, FUNCTION_CFG_LED);
      ConfigManager->setElement(key.c_str(), LEVEL, 1);
    }
    else {
      WebServer->httpServer.send(200, "text/html", supla_webpage_config(6));
      return;
    }
  }
  if(ConfigESP->getGpio(1, FUNCTION_CFG_LED) != WebServer->httpServer.arg(input).toInt() || 
                      WebServer->httpServer.arg(input).toInt() == OFF_GPIO){
    key = GPIO;
    key += ConfigESP->getGpio(1, FUNCTION_CFG_LED);
    ConfigManager->setElement(key.c_str(), NR, 0);
    ConfigManager->setElement(key.c_str(), FUNCTION, FUNCTION_OFF );
    ConfigManager->setElement(key.c_str(), LEVEL, 0 );
    ConfigManager->setElement(key.c_str(), MEMORY, 0 );
  }

  key = GPIO;
  key += ConfigESP->getGpio(1, FUNCTION_CFG_LED);
  input = INPUT_CFG_LED_LEVEL;  
  ConfigManager->setElement(key.c_str(), LEVEL, WebServer->httpServer.arg(input).toInt());

  ConfigESP->sort(FUNCTION_CFG_LED);

  input = INPUT_CFG_BTN_GPIO;
  key = GPIO;
  key += WebServer->httpServer.arg(input).toInt();
  if (WebServer->httpServer.arg(input).toInt() != OFF_GPIO) {
    key = GPIO;
    key += WebServer->httpServer.arg(input).toInt();  
    if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_OFF ||
        (ConfigManager->get(key.c_str())->getElement(PIN).toInt() == WebServer->httpServer.arg(input).toInt() && 
          ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_CFG_BUTTON)) {

      ConfigManager->setElement(key.c_str(), NR, 1);
      ConfigManager->setElement(key.c_str(), FUNCTION, FUNCTION_CFG_BUTTON);
    }
    else {
      WebServer->httpServer.send(200, "text/html", supla_webpage_config(6));
      return;
    }
  }
  if(ConfigESP->getGpio(1, FUNCTION_CFG_BUTTON) != WebServer->httpServer.arg(input).toInt() || 
                      WebServer->httpServer.arg(input).toInt() == OFF_GPIO){
    key = GPIO;
    key += ConfigESP->getGpio(1, FUNCTION_CFG_BUTTON);
    ConfigManager->setElement(key.c_str(), NR, 0);
    ConfigManager->setElement(key.c_str(), FUNCTION, FUNCTION_OFF );
    ConfigManager->setElement(key.c_str(), LEVEL, 0 );
    ConfigManager->setElement(key.c_str(), MEMORY, 0 );
  }
  ConfigESP->sort(FUNCTION_CFG_BUTTON);

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
//      Serial.println(F("E_CONFIG_OK: Config save"));
      WebServer->sendContent(supla_webpage_config(1));
      break;
    case E_CONFIG_FILE_OPEN:
//      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      WebServer->httpServer.send(200, "text/html", supla_webpage_config(2));
      break;  
  }
}

String SuplaWebPageConfig::supla_webpage_config(int save) {
  uint8_t selected, suported;
  String page = "";
  page += WebServer->SuplaMetas();
  page += WebServer->SuplaStyle();
  page += WebServer->SuplaSaveResult(save);
  page += F("</div>");
  page += WebServer->SuplaJavaScript(PATH_CONFIG);
  page += F("<div class='s'>");
//  page += WebServer->SuplaLogo();
//  page += WebServer->SuplaSummary();
  page += F("<form method='post' action='");
  page += PATH_SAVE_CONFIG;
  page += F("'><div class='w'><h3>Ustawienie GPIO dla CONFIG</h3>");
  page += F("<i><label>");
  page += F("LED</label><select name='");
  page += INPUT_CFG_LED_GPIO;
  page += F("'>");
  selected = ConfigESP->getGpio(1, FUNCTION_CFG_LED);
  for (suported = 0; suported < 18; suported++) {
      if(ConfigESP->checkBusy(suported, FUNCTION_CFG_LED) == false || selected == suported){
        page += F("<option value='");
        page += suported;
        if (selected == suported) {
          page += F("' selected>");
        }
        else page += F("'>");
        page += (WebServer->Supported_Gpio[suported][0]);
    }
  }
  page += F("</select></i>");

  if(selected != 17){
    page += F("<i><label>");
    page += F("Sterowanie stanem</label><select name='");
    page += INPUT_CFG_LED_LEVEL;
    page += F("'>");
    selected = ConfigESP->getLevel(1, FUNCTION_CFG_LED); 
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
  }  
  page += F("<i><label>");
  page += F("BUTTON</label><select name='");
  page += INPUT_CFG_BTN_GPIO;
  page += F("'>");
  selected = ConfigESP->getGpio(1, FUNCTION_CFG_BUTTON);
  for (suported = 0; suported < 18; suported++) {
        if(ConfigESP->checkBusy(suported, FUNCTION_CFG_BUTTON) == false || selected == suported){
      page += F("<option value='");
      page += suported;
      if (selected == suported) {
        page += F("' selected>");
      }
      else page += F("'>");
      page += (WebServer->Supported_Gpio[suported][0]);
    }
  }
  page += F("</select></i>");
  page += F("</div><button type='submit'>Zapisz</button></form>");
  page += F("<br><br>");
  page += F("<a href='/'><button>Powrót</button></a></div>");
  return page;
}
