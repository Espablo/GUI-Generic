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
  for (nr = 1; nr <= last_value; nr++) {
    if (!WebServer->saveGPIO(INPUT_BUTTON_GPIO, FUNCTION_BUTTON, nr, INPUT_MAX_BUTTON)) {
      WebServer->sendContent(supla_webpage_control(6));
      return;
    }
  }

  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_BUTTON).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_BUTTON, WebServer->httpServer.arg(INPUT_MAX_BUTTON).c_str());
  }
#endif

#ifdef SUPLA_LIMIT_SWITCH
  last_value = ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt();
  for (nr = 1; nr <= last_value; nr++) {
    if (!WebServer->saveGPIO(INPUT_LIMIT_SWITCH_GPIO, FUNCTION_LIMIT_SWITCH, nr, INPUT_MAX_LIMIT_SWITCH)) {
      WebServer->sendContent(supla_webpage_control(6));
      return;
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
  pagebutton += F("'>");

#if (defined(SUPLA_BUTTON) && defined(SUPLA_RELAY)) || (defined(SUPLA_BUTTON) && defined(SUPLA_ROLLERSHUTTER))
  addFormHeader(pagebutton, String(S_GPIO_SETTINGS_FOR_BUTTONS));
  addNumberBox(pagebutton, INPUT_MAX_BUTTON, S_QUANTITY, KEY_MAX_BUTTON, ConfigESP->countFreeGpio(FUNCTION_BUTTON));
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_BUTTON)->getValueInt(); nr++) {
    addListGPIOLinkBox(pagebutton, INPUT_BUTTON_GPIO, S_BUTTON, FUNCTION_BUTTON, PATH_BUTTON_SET, nr);
  }
  addFormHeaderEnd(pagebutton);
#endif

#ifdef SUPLA_LIMIT_SWITCH
  addFormHeader(pagebutton, String(S_GPIO_SETTINGS_FOR_LIMIT_SWITCH));
  addNumberBox(pagebutton, INPUT_MAX_LIMIT_SWITCH, S_QUANTITY, KEY_MAX_LIMIT_SWITCH, ConfigESP->countFreeGpio(FUNCTION_LIMIT_SWITCH));
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt(); nr++) {
    addListGPIOBox(pagebutton, INPUT_LIMIT_SWITCH_GPIO, S_LIMIT_SWITCH, FUNCTION_LIMIT_SWITCH, nr);
  }
  addFormHeaderEnd(pagebutton);
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

#if (defined(SUPLA_BUTTON) && defined(SUPLA_RELAY)) || (defined(SUPLA_BUTTON) && defined(SUPLA_ROLLERSHUTTER))
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

  String readUrl, nr_button, input;
  uint8_t place;

  String path = PATH_START;
  path += PATH_SAVE_BUTTON_SET;
  readUrl = WebServer->httpServer.uri();

  place = readUrl.indexOf(path);
  nr_button = readUrl.substring(place + path.length(), place + path.length() + 3);
  uint8_t key = KEY_GPIO + ConfigESP->getGpio(nr_button.toInt(), FUNCTION_BUTTON);

  input = INPUT_BUTTON_LEVEL;
  input += nr_button;
  ConfigManager->setElement(key, LEVEL, WebServer->httpServer.arg(input).toInt());

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
