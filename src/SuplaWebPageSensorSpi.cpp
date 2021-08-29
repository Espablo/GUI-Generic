#include "SuplaWebPageSensorSpi.h"

#ifdef GUI_SENSOR_SPI
void createWebPageSensorSpi() {
  WebServer->httpServer->on(getURL(PATH_SPI), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handleSensorSpi();
    else
      handleSensorSpiSave();
  });
}

void handleSensorSpi(int save) {
  uint8_t selected;

  WebServer->sendHeaderStart();
  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_SPI);

  addForm(webContentBuffer, F("post"), PATH_SPI);
#if defined(SUPLA_MAX6675)
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_SPI);
  addListGPIOBox(webContentBuffer, INPUT_CLK_GPIO, S_CLK, FUNCTION_CLK);
  addListGPIOBox(webContentBuffer, INPUT_CS_GPIO, S_CS, FUNCTION_CS);
  addListGPIOBox(webContentBuffer, INPUT_D0_GPIO, S_D0, FUNCTION_D0);

  if (ConfigESP->getGpio(FUNCTION_CLK) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_CS) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_D0) != OFF_GPIO) {
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MAX6675).toInt();
    addListBox(webContentBuffer, INPUT_MAX6675, S_MAX6675_MAX31855, STATE_P, 2, selected);
  }
  addFormHeaderEnd(webContentBuffer);
#endif
  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);
  WebServer->sendHeaderEnd();
}

void handleSensorSpiSave() {
  String input;

  if (!WebServer->saveGPIO(INPUT_CLK_GPIO, FUNCTION_CLK) || !WebServer->saveGPIO(INPUT_CS_GPIO, FUNCTION_CS) ||
      !WebServer->saveGPIO(INPUT_D0_GPIO, FUNCTION_D0)) {
    handleSensorSpi(6);
    return;
  }

  input = INPUT_MAX6675;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_MAX6675, WebServer->httpServer->arg(input).toInt());
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleSensorSpi(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleSensorSpi(2);
      break;
  }
}
#endif