/*
  Copyright (C) krycha88

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "SuplaWebPageLimitSwitch.h"

#ifdef SUPLA_LIMIT_SWITCH
void createWebPageLimitSwitch() {
  WebServer->httpServer->on(getURL(PATH_SWITCH), [&]() {
    if (!WebServer->isLoggedIn())
      return;

    if (WebServer->httpServer->method() == HTTP_GET)
      handleLimitSwitch();
    else
      handleLimitSwitchSave();
  });

  WebServer->httpServer->on(getURL(PATH_SWITCH_SET), [&]() {
    if (!WebServer->isLoggedIn())
      return;

    if (WebServer->httpServer->method() == HTTP_GET)
      handleLimitSwitchSet();
    else
      handleLimitSwitchSaveSet();
  });
}

void handleLimitSwitch(int save) {
  uint8_t nr;

  WebServer->sendHeaderStart();

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_SWITCH);

  addForm(webContentBuffer, F("post"), PATH_SWITCH);
  addFormHeader(webContentBuffer, S_GPIO_SETTINGS_FOR_LIMIT_SWITCH);
  addNumberBox(webContentBuffer, INPUT_MAX_LIMIT_SWITCH, S_QUANTITY, KEY_MAX_LIMIT_SWITCH, ConfigESP->countFreeGpio(FUNCTION_LIMIT_SWITCH));

  for (nr = 0; nr < ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt(); nr++) {
#ifdef GUI_SENSOR_I2C_EXPENDER
    addListExpanderBox(webContentBuffer, INPUT_LIMIT_SWITCH_GPIO, S_LIMIT_SWITCH, FUNCTION_LIMIT_SWITCH, nr, PATH_SWITCH_SET);
#else
    addListGPIOLinkBox(webContentBuffer, INPUT_LIMIT_SWITCH_GPIO, S_LIMIT_SWITCH, getParameterRequest(PATH_SWITCH_SET, ARG_PARM_NUMBER),
                       FUNCTION_LIMIT_SWITCH, nr);
#endif
  }
  addFormHeaderEnd(webContentBuffer);

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);

  WebServer->sendHeaderEnd();
}

void handleLimitSwitchSave() {
  uint8_t nr, last_value;

  last_value = ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt();
  for (nr = 0; nr <= last_value; nr++) {
#ifdef GUI_SENSOR_I2C_EXPENDER
    if (Expander->checkActiveExpander(FUNCTION_LIMIT_SWITCH)) {
      if (!WebServer->saveGpioMCP23017(INPUT_LIMIT_SWITCH_GPIO, FUNCTION_LIMIT_SWITCH, nr, INPUT_MAX_LIMIT_SWITCH)) {
        handleLimitSwitch(6);
        return;
      }
    }
    else {
      if (!WebServer->saveGPIO(INPUT_LIMIT_SWITCH_GPIO, FUNCTION_LIMIT_SWITCH, nr, INPUT_MAX_LIMIT_SWITCH)) {
        handleLimitSwitch(6);
        return;
      }
    }
#else
    if (!WebServer->saveGPIO(INPUT_LIMIT_SWITCH_GPIO, FUNCTION_LIMIT_SWITCH, nr, INPUT_MAX_LIMIT_SWITCH)) {
      handleLimitSwitch(6);
      return;
    }
#endif
  }

  if (strcmp(WebServer->httpServer->arg(INPUT_MAX_LIMIT_SWITCH).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_LIMIT_SWITCH, WebServer->httpServer->arg(INPUT_MAX_LIMIT_SWITCH).c_str());
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleLimitSwitch(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleLimitSwitch(2);
      break;
  }
}

void handleLimitSwitchSet(int save) {
  uint8_t gpio, selected;
  String nr, url, input;

  nr = WebServer->httpServer->arg(ARG_PARM_NUMBER);

  WebServer->sendHeaderStart();

  if (!nr.isEmpty()) {
    gpio = ConfigESP->getGpio(nr.toInt(), FUNCTION_LIMIT_SWITCH);
    url = getParameterRequest(PATH_SWITCH_SET, ARG_PARM_NUMBER, nr);

    webContentBuffer += SuplaSaveResult(save);
    webContentBuffer += SuplaJavaScript(url);

    addForm(webContentBuffer, F("post"), url);
    addFormHeader(webContentBuffer, String(S_LIMIT_SWITCH) + S_SPACE + (nr.toInt() + 1));

    selected = ConfigESP->getPullUp(gpio);
    input = INPUT_LIMIT_SWITCH_PULLUP;
    addCheckBox(webContentBuffer, input, S_INTERNAL_PULL_UP, selected);
    addFormHeaderEnd(webContentBuffer);

#ifdef SUPLA_PUSHOVER
    Html::addPushover(nr.toInt());
#endif
  }

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);

  addButton(webContentBuffer, S_RETURN, PATH_SWITCH);

  WebServer->sendHeaderEnd();
}

void handleLimitSwitchSaveSet() {
  String input, nr;
  uint8_t gpio;

  nr = WebServer->httpServer->arg(ARG_PARM_NUMBER);
  gpio = ConfigESP->getGpio(nr.toInt(), FUNCTION_LIMIT_SWITCH);

  input = INPUT_LIMIT_SWITCH_PULLUP;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0)
    ConfigESP->setPullUp(gpio, 1);
  else
    ConfigESP->setPullUp(gpio, 0);

#ifdef SUPLA_PUSHOVER
  if (nr.toInt() <= MAX_PUSHOVER_MESSAGE) {
    input = INPUT_PUSHOVER_SOUND;
    ConfigManager->setElement(KEY_PUSHOVER_SOUND, (nr.toInt()), WebServer->httpServer->arg(input).c_str());
    input = INPUT_PUSHOVER_MESSAGE;
    ConfigManager->setElement(KEY_PUSHOVER_MASSAGE, (nr.toInt()), WebServer->httpServer->arg(input).c_str());
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleLimitSwitchSet(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleLimitSwitchSet(2);
      break;
  }
}
#endif