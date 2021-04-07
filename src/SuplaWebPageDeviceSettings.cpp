#include "SuplaWebPageDeviceSettings.h"

void createWebPageDeviceSettings() {
  WebServer->httpServer->on(getURL(PATH_DEVICE_SETTINGS), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handleDeviceSettings();
    else
      handleDeviceSettingsSave();
  });
}

void handleDeviceSettings(int save) {
  WebServer->sendHeaderStart();

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_DEVICE_SETTINGS);

  addForm(webContentBuffer, F("post"), PATH_DEVICE_SETTINGS);
  addFormHeader(webContentBuffer, S_TEMPLATE_BOARD);
  uint8_t selected = ConfigManager->get(KEY_BOARD)->getValueInt();
  addListBox(webContentBuffer, INPUT_BOARD, S_TYPE, BOARD_P, MAX_MODULE, selected);
  addFormHeaderEnd(webContentBuffer);
  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);

  addFormHeader(webContentBuffer, S_DEVICE_SETTINGS);
#if defined(SUPLA_RELAY)
  addButton(webContentBuffer, S_RELAYS, PATH_RELAY);
#endif

#ifdef SUPLA_BUTTON
  addButton(webContentBuffer, S_BUTTONS, PATH_CONTROL);
#endif

#ifdef SUPLA_LIMIT_SWITCH
  addButton(webContentBuffer, S_LIMIT_SWITCHES, PATH_SWITCH);
#endif

#ifdef GUI_SENSOR_1WIRE
  addButton(webContentBuffer, S_SENSORS_1WIRE, PATH_1WIRE);
#endif

#ifdef GUI_SENSOR_I2C
  addButton(webContentBuffer, S_SENSORS_I2C, PATH_I2C);
#endif

#ifdef GUI_SENSOR_SPI
  addButton(webContentBuffer, S_SENSORS_SPI, PATH_SPI);
#endif

#ifdef GUI_OTHER
  addButton(webContentBuffer, S_SENSORS_OTHER, PATH_OTHER);
#endif

#if defined(GUI_SENSOR_1WIRE) || defined(GUI_SENSOR_I2C) || defined(GUI_SENSOR_SPI)
  addButton(webContentBuffer, S_CORRECTION, PATH_CORRECTION);
#endif

#ifdef SUPLA_CONFIG
  addButton(webContentBuffer, S_LED_BUTTON_CFG, PATH_CONFIG);
#endif
  addFormHeaderEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, "");

  WebServer->sendHeaderEnd();
}

void handleDeviceSettingsSave() {
  String input = INPUT_BOARD;

  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    saveChooseTemplateBoard(WebServer->httpServer->arg(input).toInt());
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleDeviceSettings(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleDeviceSettings(2);
      break;
  }
}