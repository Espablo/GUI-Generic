#include "SuplaWebPageControl.h"
#include "SuplaDeviceGUI.h"
#include "SuplaWebServer.h"
#include "SuplaCommonPROGMEM.h"
#include "GUIGenericCommon.h"
#include "Markup.h"

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
  for (uint8_t i = 1; i <= MAX_GPIO; i++) {
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
  WebServer->sendContent(supla_webpage_control(0));
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
  last_value = ConfigManager->get(KEY_MAX_BUTTON)->getValueInt();
  current_value = WebServer->httpServer.arg(INPUT_MAX_BUTTON).toInt();

  if (last_value > 0) {
    for (nr = 1; nr <= last_value; nr++) {
      input = INPUT_BUTTON_GPIO;
      input += nr;
      key = GPIO;
      key += WebServer->httpServer.arg(input).toInt();
      if (ConfigESP->getGpio(nr, FUNCTION_BUTTON) != WebServer->httpServer.arg(input).toInt() ||
          WebServer->httpServer.arg(input).toInt() == OFF_GPIO || ConfigManager->get(key.c_str())->getElement(NR).toInt() > current_value) {
        ConfigESP->clearGpio(ConfigESP->getGpio(nr, FUNCTION_BUTTON));
      }
      if (WebServer->httpServer.arg(input).toInt() != OFF_GPIO) {
        key = GPIO;
        key += WebServer->httpServer.arg(input).toInt();
        if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_OFF) {
          ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), nr, FUNCTION_BUTTON, 2);
        }
        else if (ConfigESP->getGpio(nr, FUNCTION_BUTTON) == WebServer->httpServer.arg(input).toInt() &&
                 ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_BUTTON) {
          ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), nr, FUNCTION_BUTTON, ConfigESP->getLevel(nr, FUNCTION_BUTTON));
        }
        else {
          WebServer->sendContent(supla_webpage_control(6));
          return;
        }
      }
    }
  }
  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_BUTTON).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_BUTTON, WebServer->httpServer.arg(INPUT_MAX_BUTTON).c_str());
  }
#endif

#ifdef SUPLA_LIMIT_SWITCH
  last_value = ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt();
  current_value = WebServer->httpServer.arg(INPUT_MAX_LIMIT_SWITCH).toInt();

  if (last_value > 0) {
    for (nr = 1; nr <= last_value; nr++) {
      input = INPUT_LIMIT_SWITCH_GPIO;
      input += nr;
      key = GPIO;
      key += WebServer->httpServer.arg(input).toInt();
      if (ConfigESP->getGpio(nr, FUNCTION_LIMIT_SWITCH) != WebServer->httpServer.arg(input).toInt() ||
          WebServer->httpServer.arg(input).toInt() == OFF_GPIO || ConfigManager->get(key.c_str())->getElement(NR).toInt() > current_value) {
        ConfigESP->clearGpio(ConfigESP->getGpio(nr, FUNCTION_LIMIT_SWITCH));
      }
      if (WebServer->httpServer.arg(input).toInt() != OFF_GPIO) {
        key = GPIO;
        key += WebServer->httpServer.arg(input).toInt();
        if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_OFF) {
          ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), nr, FUNCTION_LIMIT_SWITCH, 0);
        }
        else if (ConfigESP->getGpio(nr, FUNCTION_LIMIT_SWITCH) == WebServer->httpServer.arg(input).toInt() &&
                 ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_LIMIT_SWITCH) {
          ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), nr, FUNCTION_LIMIT_SWITCH, ConfigESP->getLevel(nr, FUNCTION_LIMIT_SWITCH));
        }
        else {
          WebServer->sendContent(supla_webpage_control(6));
          return;
        }
      }
    }
  }
  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_LIMIT_SWITCH).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_LIMIT_SWITCH, WebServer->httpServer.arg(INPUT_MAX_LIMIT_SWITCH).c_str());
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      //      Serial.println(F("E_CONFIG_OK: Config save"));
      WebServer->sendContent(supla_webpage_control(1));
      break;
    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      WebServer->sendContent(supla_webpage_control(2));
      break;
  }
}

String SuplaWebPageControl::supla_webpage_control(int save) {
  uint8_t nr, suported, selected;
  String pagebutton, key;

  pagebutton += WebServer->SuplaSaveResult(save);
  pagebutton += WebServer->SuplaJavaScript(PATH_CONTROL);
  pagebutton += F("<form method='post' action='");
  pagebutton += PATH_SAVE_CONTROL;

#if (defined(SUPLA_BUTTON) && defined(SUPLA_RELAY)) || (defined(SUPLA_RSUPLA_BUTTONELAY) || defined(SUPLA_ROLLERSHUTTER))
  pagebutton += F("'><div class='w'><h3>");
  pagebutton += S_GPIO_SETTINGS_FOR_BUTTONS;
  pagebutton += F("</h3>");
  pagebutton += F("<i><label>");
  pagebutton += S_QUANTITY;
  pagebutton += F("</label><input name='");
  pagebutton += INPUT_MAX_BUTTON;
  pagebutton += F("' type='number' placeholder='0' step='1' min='0' max='");
  pagebutton += ConfigESP->countFreeGpio(FUNCTION_BUTTON);
  pagebutton += F("' value='");
  pagebutton += String(ConfigManager->get(KEY_MAX_BUTTON)->getValue());
  pagebutton += F("'></i>");
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_BUTTON)->getValueInt(); nr++) {
    pagebutton += F("<i><label>");
    selected = ConfigESP->getGpio(nr, FUNCTION_BUTTON);
    if (selected != OFF_GPIO) {
      pagebutton += F("<a href='");
      pagebutton += PATH_START;
      pagebutton += PATH_BUTTON_SET;
      pagebutton += nr;
      pagebutton += F("'>");
    }
    pagebutton += nr;
    pagebutton += F(". ");
    pagebutton += S_BUTTON;
    if (selected != OFF_GPIO) {
      pagebutton += WebServer->SuplaIconEdit();
      pagebutton += F("</a>");
    }
    pagebutton += F("</label>");
    pagebutton += addListGPIOSelect(INPUT_BUTTON_GPIO, FUNCTION_BUTTON, nr);
    pagebutton += F("</i>");
  }
  pagebutton += F("</div>");
#endif

#ifdef SUPLA_LIMIT_SWITCH
  pagebutton += F("<div class='w'><h3>");
  pagebutton += S_GPIO_SETTINGS_FOR_LIMIT_SWITCH;
  pagebutton += F("</h3>");
  pagebutton += F("<i><label>");
  pagebutton += S_QUANTITY;
  pagebutton += F("</label><input name='");
  pagebutton += INPUT_MAX_LIMIT_SWITCH;
  pagebutton += F("' type='number' placeholder='0' step='1' min='0' max='");
  pagebutton += ConfigESP->countFreeGpio(FUNCTION_LIMIT_SWITCH);
  pagebutton += F("' value='");
  pagebutton += String(ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValue());
  pagebutton += F("'></i>");
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt(); nr++) {
    pagebutton += F("<i><label>");
    pagebutton += nr;
    pagebutton += F(". ");
    pagebutton += S_LIMIT_SWITCH;
    pagebutton += F("</label>");
    pagebutton += addListGPIOSelect(INPUT_LIMIT_SWITCH_GPIO, FUNCTION_LIMIT_SWITCH, nr);
    pagebutton += F("</i>");
  }
  pagebutton += F("</div>");
#endif

  pagebutton += F("<button type='submit'>");
  pagebutton += S_SAVE;
  pagebutton += F("</button></form>");
  pagebutton += F("<br>");
  pagebutton += F("<a href='");
  pagebutton += PATH_START;
  pagebutton += PATH_DEVICE_SETTINGS;
  pagebutton += F("'><button>");
  pagebutton += S_RETURN;
  pagebutton += F("</button></a></div>");
  return pagebutton;
}

#if (defined(SUPLA_BUTTON) && defined(SUPLA_RELAY)) || (defined(SUPLA_RSUPLA_BUTTONELAY) || defined(SUPLA_ROLLERSHUTTER))
void SuplaWebPageControl::handleButtonSet() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  WebServer->sendContent(supla_webpage_button_set(0));
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
  nr_button = readUrl.substring(place + path.length(), place + path.length() + 3);
  key = GPIO;
  key += ConfigESP->getGpio(nr_button.toInt(), FUNCTION_BUTTON);

  input = INPUT_BUTTON_LEVEL;
  input += nr_button;
  ConfigManager->setElement(key.c_str(), LEVEL, WebServer->httpServer.arg(input).toInt());

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      //      Serial.println(F("E_CONFIG_OK: Dane zapisane"));
      WebServer->sendContent(supla_webpage_control(1));
      break;

    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      WebServer->sendContent(supla_webpage_control(2));
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
  nr_button = readUrl.substring(place + path.length(), place + path.length() + 3);

  String page = "";
  page += WebServer->SuplaSaveResult(save);
  page += WebServer->SuplaJavaScript(PATH_CONTROL);
  uint8_t buttons = ConfigManager->get(KEY_MAX_BUTTON)->getValueInt();
  if (nr_button.toInt() <= buttons && ConfigESP->getGpio(nr_button.toInt(), FUNCTION_BUTTON) != OFF_GPIO) {
    page += F("<form method='post' action='");
    page += PATH_SAVE_BUTTON_SET;
    page += nr_button;
    page += F("'><div class='w'><h3>");
    page += S_BUTTON_NR_SETTINGS;
    page += F(" ");
    page += nr_button;
    page += F("</h3>");
    page += F("<i><label>");
    page += S_REACTION_TO;
    page += F("</label><select name='");
    page += INPUT_BUTTON_LEVEL;
    page += nr_button;
    page += F("'>");
    selected = ConfigESP->getLevel(nr_button.toInt(), FUNCTION_BUTTON);
    for (suported = 0; suported < 3; suported++) {
      page += F("<option value='");
      page += suported;
      if (selected == suported) {
        page += F("' selected>");
      }
      else
        page += F("'>");
      page += TriggerString(suported);
    }
    page += F("</select></i>");
    page += F("</div><button type='submit'>");
    page += S_SAVE;
    page += F("</button></form>");
  }
  else {
    page += F("<div class='w'><h3>");
    page += S_NO_BUTTON_NR;
    page += F(" </h3>");
    page += nr_button;
    page += F("</h3>");
  }
  page += F("<br>");
  page += F("<a href='");
  page += PATH_START;
  page += PATH_CONTROL;
  page += F("'><button>");
  page += S_RETURN;
  page += F("</button></a></div>");

  return page;
}
#endif
