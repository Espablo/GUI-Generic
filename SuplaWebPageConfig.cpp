#include "SuplaWebPageConfig.h"
#include "SuplaDeviceGUI.h"
#include "SuplaWebServer.h"
#include "SuplaCommonPROGMEM.h"

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
  WebServer->sendContent(supla_webpage_config(0));
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
    if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_OFF) {
      ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), 1, FUNCTION_CFG_LED, 1);
    }
    else if (ConfigESP->getGpio(FUNCTION_CFG_LED) == WebServer->httpServer.arg(input).toInt() &&
             ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_CFG_LED) {
      key = GPIO;
      key += ConfigESP->getGpio(FUNCTION_CFG_LED);
      input = INPUT_CFG_LED_LEVEL;
      ConfigManager->setElement(key.c_str(), LEVEL, WebServer->httpServer.arg(input).toInt());
    }
    else {
      WebServer->sendContent(supla_webpage_config(6));
      return;
    }
  }
  else if (ConfigESP->getGpio(FUNCTION_CFG_LED) != WebServer->httpServer.arg(input).toInt() || WebServer->httpServer.arg(input).toInt() == OFF_GPIO) {
    ConfigESP->clearGpio(ConfigESP->getGpio(FUNCTION_CFG_LED));
  }

  input = INPUT_CFG_BTN_GPIO;
  key = GPIO;
  key += WebServer->httpServer.arg(input).toInt();
  if (WebServer->httpServer.arg(input).toInt() != OFF_GPIO) {
    key = GPIO;
    key += WebServer->httpServer.arg(input).toInt();
    if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_OFF ||
        (ConfigESP->getGpio(FUNCTION_CFG_BUTTON) == WebServer->httpServer.arg(input).toInt() &&
         ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_CFG_BUTTON)) {
      ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), FUNCTION_CFG_BUTTON);
    }
    else if (ConfigESP->checkBusyGpio(WebServer->httpServer.arg(input).toInt(), FUNCTION_BUTTON) == false) {
      ConfigManager->setElement(key.c_str(), CFG, 1);
    }
    else {
      WebServer->sendContent(supla_webpage_config(6));
      return;
    }
  }
  if (ConfigESP->getGpio(FUNCTION_CFG_BUTTON) != WebServer->httpServer.arg(input).toInt() || WebServer->httpServer.arg(input).toInt() == OFF_GPIO) {
    ConfigESP->clearGpio(ConfigESP->getGpio(FUNCTION_CFG_BUTTON));
  }

#ifdef SUPLA_BUTTON
  for (int i = 1; i <= ConfigManager->get(KEY_MAX_BUTTON)->getValueInt(); i++) {
    key = GPIO;
    key += ConfigESP->getGpio(i, FUNCTION_BUTTON);
    if (ConfigESP->getGpio(i, FUNCTION_BUTTON) != WebServer->httpServer.arg(input).toInt() || WebServer->httpServer.arg(input).toInt() == OFF_GPIO) {
      if (ConfigManager->get(key.c_str())->getElement(CFG).toInt() == 1) {
        ConfigManager->setElement(key.c_str(), CFG, 0);
      }
    }
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      //      Serial.println(F("E_CONFIG_OK: Config save"));
      WebServer->sendContent(supla_webpage_config(1));
      break;
    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      WebServer->sendContent(supla_webpage_config(2));
      break;
  }
}

String SuplaWebPageConfig::supla_webpage_config(int save) {
  uint8_t selected, suported;
  String page = "";
  page += WebServer->SuplaSaveResult(save);
  page += WebServer->SuplaJavaScript(PATH_CONFIG);
  page += F("<form method='post' action='");
  page += PATH_SAVE_CONFIG;
  page += F("'><div class='w'><h3>Ustawienie GPIO dla CONFIG</h3>");
  page += F("<i><label>");
  page += F("LED</label>");
  page += WebServer->selectGPIO(INPUT_CFG_LED_GPIO, FUNCTION_CFG_LED);
  page += F("</i>");

  if (selected != 17) {
    page += F("<i><label>");
    page += F("Sterowanie stanem</label><select name='");
    page += INPUT_CFG_LED_LEVEL;
    page += F("'>");
    selected = ConfigESP->getLevel(FUNCTION_CFG_LED);
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
  }
  page += F("<i><label>");
  page += F("BUTTON</label>");
  page += WebServer->selectGPIO(INPUT_CFG_BTN_GPIO, FUNCTION_CFG_BUTTON);
  page += F("</i>");
  page += F("</div><button type='submit'>Zapisz</button></form>");
  page += F("<br>");
  page += F("<a href='");
  page += PATH_START;
  page += PATH_DEVICE_SETTINGS;
  page += F("'><button>Powrót</button></a></div>");
  return page;
}
