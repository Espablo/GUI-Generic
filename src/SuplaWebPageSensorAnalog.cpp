#include "SuplaWebPageSensorAnalog.h"

#ifdef GUI_SENSOR_ANALOG
void createWebPageSensorAnalog() {
  WebServer->httpServer->on(getURL(PATH_ANALOG), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

#ifdef SUPLA_MPX_5XXX
    if (WebServer->httpServer->arg(ARG_PARM_URL) == PATH_MPX_5XX_EMPTY) {
      Supla::GUI::mpx->calibrateEmptyThank();
      handleSensorAnalog(1);
      return;
    }
    else if (WebServer->httpServer->arg(ARG_PARM_URL) == PATH_MPX_5XX_FULL) {
      Supla::GUI::mpx->calibrateFullThank();
      handleSensorAnalog(1);
      return;
    }
#endif

#ifdef SUPLA_ANALOG_READING_MAP
    if (WebServer->httpServer->arg(ARG_PARM_URL) == PATH_ANALOG_READING_MAP_MIN) {
      Supla::GUI::analog->calibrateMinValue();
      handleSensorAnalog(1);
      return;
    }
    else if (WebServer->httpServer->arg(ARG_PARM_URL) == PATH_ANALOG_READING_MAP_MAX) {
      Supla::GUI::analog->calibrateMaxValue();
      handleSensorAnalog(1);
      return;
    }
#endif

    if (WebServer->httpServer->method() == HTTP_GET)
      handleSensorAnalog();
    else
      handleSensorAnalogSave();
  });
}

void handleSensorAnalog(int save) {
  WebServer->sendHeaderStart();

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_ANALOG);

  addForm(webContentBuffer, F("post"), PATH_ANALOG);

#ifdef SUPLA_NTC_10K
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_NTC_10K);
  addListGPIOBox(webContentBuffer, INPUT_NTC_10K, F("ADC Pin"), FUNCTION_NTC_10K);
  addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_MPX_5XXX
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_MPX_5XXX);
  addListGPIOBox(webContentBuffer, INPUT_MPX_5XXX, F("ADC Pin"), FUNCTION_MPX_5XXX);
  if (ConfigESP->getGpio(FUNCTION_MPX_5XXX) != OFF_GPIO) {
    int16_t thankHeight = Supla::GUI::mpx->getThankHeight();
    addNumberBox(webContentBuffer, INPUT_THANK_HEIGHT, String(F("Głębokość zbiornika")) + F("[cm]"), F("cm"), false, String(thankHeight));
    int16_t thankEmpty = Supla::GUI::mpx->getEmptyValue();
    addNumberBox(webContentBuffer, INPUT_THANK_EMPTY, F("Pusty zbiornik"), F("wartość kalibracji"), false, String(thankEmpty));
    int16_t thankFull = Supla::GUI::mpx->getFullValue();
    addNumberBox(webContentBuffer, INPUT_THANK_FULL, F("Pełny zbiornik"), F("wartość kalibracji"), false, String(thankFull));
    addLinkBox(webContentBuffer, String(S_CALIBRATION) + S_SPACE + F("dla pustego zbiornika"),
               getParameterRequest(PATH_ANALOG, ARG_PARM_URL) + PATH_MPX_5XX_EMPTY);
    addLinkBox(webContentBuffer, String(S_CALIBRATION) + S_SPACE + F("dla pełnego zbiornika"),
               getParameterRequest(PATH_ANALOG, ARG_PARM_URL) + PATH_MPX_5XX_FULL);
  }
  addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_ANALOG_READING_MAP
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + S_SPACE + "pomiaru Analog");
  addListGPIOBox(webContentBuffer, INPUT_ANALOG_READING_MAP, F("ADC Pin"), FUNCTION_ANALOG_READING);

  if (ConfigESP->getGpio(FUNCTION_ANALOG_READING) != OFF_GPIO) {
    int16_t value = Supla::GUI::analog->getMinValue();
    addNumberBox(webContentBuffer, INPUT_ANALOG_READING_MAP_MIN, F("MIN IN"), F("wartość kalibracji min"), false, String(value));

    value = Supla::GUI::analog->getMaxValue();
    addNumberBox(webContentBuffer, INPUT_ANALOG_READING_MAP_MAX, F("MAX IN"), F("wartość kalibracji max"), false, String(value));

    value = Supla::GUI::analog->getMinDesiredValue();
    addNumberBox(webContentBuffer, INPUT_ANALOG_READING_MAP_MIN_DESIRED, F("MIN OUT"), F("wartość porządana min"), false, String(value));

    value = Supla::GUI::analog->getMaxDesiredValue();
    addNumberBox(webContentBuffer, INPUT_ANALOG_READING_MAP_MAX_DESIRED, F("MAX OUT"), F("wartość porządana max"), false, String(value));

    addLinkBox(webContentBuffer, String(S_CALIBRATION) + S_SPACE + F("dla MIN IN"),
               getParameterRequest(PATH_ANALOG, ARG_PARM_URL) + PATH_ANALOG_READING_MAP_MIN);
    addLinkBox(webContentBuffer, String(S_CALIBRATION) + S_SPACE + F("dla MAX IN"),
               getParameterRequest(PATH_ANALOG, ARG_PARM_URL) + PATH_ANALOG_READING_MAP_MAX);
  }
  addFormHeaderEnd(webContentBuffer);
#endif

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);
  WebServer->sendHeaderEnd();
}

void handleSensorAnalogSave() {
#ifdef SUPLA_NTC_10K
  if (!WebServer->saveGPIO(INPUT_NTC_10K, FUNCTION_NTC_10K)) {
    handleSensorAnalog(6);
    return;
  }
#endif

#ifdef SUPLA_MPX_5XXX
  if (!WebServer->saveGPIO(INPUT_MPX_5XXX, FUNCTION_MPX_5XXX)) {
    handleSensorAnalog(6);
    return;
  }
  else {
    if (ConfigESP->getGpio(FUNCTION_MPX_5XXX) != OFF_GPIO) {
      if (Supla::GUI::mpx == NULL) {
        Supla::GUI::mpx = new Supla::Sensor::MPX_5XXX(A0);
      }

      if (strcmp(WebServer->httpServer->arg(INPUT_THANK_HEIGHT).c_str(), "") != 0) {
        Supla::GUI::mpx->setThankHeight(WebServer->httpServer->arg(INPUT_THANK_HEIGHT).toInt());
      }
      if (strcmp(WebServer->httpServer->arg(INPUT_THANK_EMPTY).c_str(), "") != 0) {
        Supla::GUI::mpx->setEmptyValue(WebServer->httpServer->arg(INPUT_THANK_EMPTY).toInt());
      }
      if (strcmp(WebServer->httpServer->arg(INPUT_THANK_FULL).c_str(), "") != 0) {
        Supla::GUI::mpx->setFullValue(WebServer->httpServer->arg(INPUT_THANK_FULL).toInt());
      }
    }
  }

#endif

#ifdef SUPLA_ANALOG_READING_MAP
  if (!WebServer->saveGPIO(INPUT_ANALOG_READING_MAP, FUNCTION_ANALOG_READING)) {
    handleSensorAnalog(6);
    return;
  }
  else {
    if (ConfigESP->getGpio(FUNCTION_ANALOG_READING) != OFF_GPIO) {
      if (Supla::GUI::analog == NULL) {
        Supla::GUI::analog = new Supla::Sensor::AnalogRedingMap(A0);
      }

      if (strcmp(WebServer->httpServer->arg(INPUT_ANALOG_READING_MAP_MIN).c_str(), "") != 0) {
        Supla::GUI::analog->setMinValue(WebServer->httpServer->arg(INPUT_ANALOG_READING_MAP_MIN).toInt());
      }
      if (strcmp(WebServer->httpServer->arg(INPUT_ANALOG_READING_MAP_MAX).c_str(), "") != 0) {
        Supla::GUI::analog->setMaxValue(WebServer->httpServer->arg(INPUT_ANALOG_READING_MAP_MAX).toInt());
      }
      if (strcmp(WebServer->httpServer->arg(INPUT_ANALOG_READING_MAP_MIN_DESIRED).c_str(), "") != 0) {
        Supla::GUI::analog->setMinDesiredValue(WebServer->httpServer->arg(INPUT_ANALOG_READING_MAP_MIN_DESIRED).toInt());
      }
      if (strcmp(WebServer->httpServer->arg(INPUT_ANALOG_READING_MAP_MAX_DESIRED).c_str(), "") != 0) {
        Supla::GUI::analog->setMaxDesiredValue(WebServer->httpServer->arg(INPUT_ANALOG_READING_MAP_MAX_DESIRED).toInt());
      }
    }
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleSensorAnalog(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleSensorAnalog(2);
      break;
  }
}
#endif