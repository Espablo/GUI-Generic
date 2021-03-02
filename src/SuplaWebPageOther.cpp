#include "SuplaWebPageOther.h"

void createWebPageOther() {
#if defined(SUPLA_HC_SR04) || defined(SUPLA_IMPULSE_COUNTER) || defined(SUPLA_HLW8012) || defined(SUPLA_RGBW) || defined(SUPLA_PUSHOVER)
  WebServer->httpServer->on(getURL(PATH_OTHER), handleOther);
  WebServer->httpServer->on(getURL(PATH_SAVE_OTHER), handleOtherSave);

#if defined(SUPLA_IMPULSE_COUNTER)
  for (uint8_t i = 1; i <= ConfigManager->get(KEY_MAX_IMPULSE_COUNTER)->getValueInt(); i++) {
    WebServer->httpServer->on(getURL(PATH_IMPULSE_COUNTER_SET, i), handleImpulseCounterSet);
    WebServer->httpServer->on(getURL(PATH_SAVE_IMPULSE_COUNTER_SET, i), handleImpulseCounterSaveSet);
  }
#endif

#if defined(SUPLA_HLW8012)
  if (ConfigESP->getGpio(FUNCTION_CF) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_CF1) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SEL) != OFF_GPIO) {
    WebServer->httpServer->on(getURL(PATH_HLW8012_CALIBRATE), handleHLW8012Calibrate);
    WebServer->httpServer->on(getURL(PATH_SAVE_HLW8012_CALIBRATE), handleHLW8012CalibrateSave);
  }
#endif
#endif
}

#if defined(SUPLA_HC_SR04) || defined(SUPLA_IMPULSE_COUNTER) || defined(SUPLA_HLW8012) || defined(SUPLA_RGBW) || defined(SUPLA_PUSHOVER)
void handleOther() {
  if (!WebServer->isLoggedIn()) {
    return;
  }
  suplaWebPageOther(0);
}

void handleOtherSave() {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  uint8_t nr, last_value;

#ifdef SUPLA_HC_SR04
  if (!WebServer->saveGPIO(INPUT_TRIG_GPIO, FUNCTION_TRIG)) {
    suplaWebPageOther(6);
    return;
  }
  if (!WebServer->saveGPIO(INPUT_ECHO_GPIO, FUNCTION_ECHO)) {
    suplaWebPageOther(6);
    return;
  }
  ConfigManager->set(KEY_HC_SR04_MAX_SENSOR_READ, WebServer->httpServer->arg(INPUT_HC_SR04_MAX_SENSOR_READ).c_str());
#endif

#ifdef SUPLA_IMPULSE_COUNTER
  // Supla::GUI::impulseCounter[0]->setCounter((unsigned long long)WebServer->httpServer->arg(INPUT_IMPULSE_COUNTER_CHANGE_VALUE).toInt());

  last_value = ConfigManager->get(KEY_MAX_IMPULSE_COUNTER)->getValueInt();
  for (nr = 1; nr <= last_value; nr++) {
    if (!WebServer->saveGPIO(INPUT_IMPULSE_COUNTER_GPIO, FUNCTION_IMPULSE_COUNTER, nr, INPUT_MAX_IMPULSE_COUNTER)) {
      suplaWebPageOther(6);
      return;
    }
  }

  if (strcmp(WebServer->httpServer->arg(INPUT_MAX_IMPULSE_COUNTER).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_IMPULSE_COUNTER, WebServer->httpServer->arg(INPUT_MAX_IMPULSE_COUNTER).c_str());
  }
#endif

#ifdef SUPLA_RGBW
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_RGBW)->getValueInt(); nr++) {
    if (!WebServer->saveGPIO(INPUT_RGBW_RED, FUNCTION_RGBW_RED, nr, INPUT_RGBW_MAX) ||
        !WebServer->saveGPIO(INPUT_RGBW_GREEN, FUNCTION_RGBW_GREEN, nr, INPUT_RGBW_MAX) ||
        !WebServer->saveGPIO(INPUT_RGBW_BLUE, FUNCTION_RGBW_BLUE, nr, INPUT_RGBW_MAX) ||
        !WebServer->saveGPIO(INPUT_RGBW_BRIGHTNESS, FUNCTION_RGBW_BRIGHTNESS, nr, INPUT_RGBW_MAX)) {
      suplaWebPageOther(6);
      return;
    }
  }
  ConfigManager->set(KEY_MAX_RGBW, WebServer->httpServer->arg(INPUT_RGBW_MAX).c_str());
#endif

#ifdef SUPLA_HLW8012
  if (!WebServer->saveGPIO(INPUT_CF, FUNCTION_CF) || !WebServer->saveGPIO(INPUT_CF1, FUNCTION_CF1) || !WebServer->saveGPIO(INPUT_SEL, FUNCTION_SEL)) {
    suplaWebPageOther(6);
    return;
  }
#endif

#if defined(SUPLA_PUSHOVER)
  if (strcmp(WebServer->httpServer->arg(INPUT_PUSHOVER_TOKEN).c_str(), "") != 0) {
    ConfigManager->set(KEY_PUSHOVER_TOKEN, WebServer->httpServer->arg(INPUT_PUSHOVER_TOKEN).c_str());
  }

  if (strcmp(WebServer->httpServer->arg(INPUT_PUSHOVER_USER).c_str(), "") != 0) {
    ConfigManager->set(KEY_PUSHOVER_USER, WebServer->httpServer->arg(INPUT_PUSHOVER_USER).c_str());
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      suplaWebPageOther(1);
      break;
    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      suplaWebPageOther(2);
      break;
  }
}

void suplaWebPageOther(int save) {
  uint8_t nr, suported, selected;

  WebServer->sendHeaderStart();
  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_OTHER);

  addForm(webContentBuffer, F("post"), PATH_SAVE_OTHER);
#ifdef SUPLA_HC_SR04
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_HC_SR04);
  addListGPIOBox(webContentBuffer, INPUT_TRIG_GPIO, F("TRIG"), FUNCTION_TRIG);
  addListGPIOBox(webContentBuffer, INPUT_ECHO_GPIO, F("ECHO"), FUNCTION_ECHO);
  addNumberBox(webContentBuffer, INPUT_HC_SR04_MAX_SENSOR_READ, S_DEPTH_CM, S_SENSOR_READING_DISTANCE, false,
               ConfigManager->get(KEY_HC_SR04_MAX_SENSOR_READ)->getValue());
  addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_IMPULSE_COUNTER
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_IMPULSE_COUNTER);
  addNumberBox(webContentBuffer, INPUT_MAX_IMPULSE_COUNTER, S_QUANTITY, KEY_MAX_IMPULSE_COUNTER, ConfigESP->countFreeGpio(FUNCTION_IMPULSE_COUNTER));
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_IMPULSE_COUNTER)->getValueInt(); nr++) {
    addListGPIOLinkBox(webContentBuffer, INPUT_IMPULSE_COUNTER_GPIO, F("IC GPIO"), FUNCTION_IMPULSE_COUNTER, PATH_IMPULSE_COUNTER_SET, nr);
  }
  addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_HLW8012
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_HLW8012);
  addListGPIOBox(webContentBuffer, INPUT_CF, F("CF"), FUNCTION_CF);
  addListGPIOBox(webContentBuffer, INPUT_CF1, F("CF1"), FUNCTION_CF1);
  addListGPIOBox(webContentBuffer, INPUT_SEL, F("SELi"), FUNCTION_SEL);
  if (ConfigESP->getGpio(FUNCTION_CF) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_CF1) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SEL) != OFF_GPIO) {
    addLinkBox(webContentBuffer, S_CALIBRATION, PATH_HLW8012_CALIBRATE);
  }
  addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_RGBW
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_RGBW_RGB_DIMMER);
  addNumberBox(webContentBuffer, INPUT_RGBW_MAX, S_QUANTITY, KEY_MAX_RGBW, ConfigESP->countFreeGpio());
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_RGBW)->getValueInt(); nr++) {
    addListGPIOBox(webContentBuffer, INPUT_RGBW_RED, F("RED"), FUNCTION_RGBW_RED, nr, false);
    addListGPIOBox(webContentBuffer, INPUT_RGBW_GREEN, F("GREEN"), FUNCTION_RGBW_GREEN, nr, false);
    addListGPIOBox(webContentBuffer, INPUT_RGBW_BLUE, F("BLUE"), FUNCTION_RGBW_BLUE, nr, false);
    addListGPIOBox(webContentBuffer, INPUT_RGBW_BRIGHTNESS, F("WHITE / DIMMER"), FUNCTION_RGBW_BRIGHTNESS, nr, false);
  }
  addFormHeaderEnd(webContentBuffer);
#endif

#if defined(SUPLA_PUSHOVER)
  addFormHeader(webContentBuffer, String(S_SETTING_FOR) + S_SPACE + S_PUSHOVER);
  addTextBox(webContentBuffer, INPUT_PUSHOVER_TOKEN, F("Token"), KEY_PUSHOVER_TOKEN, 0, MAX_TOKEN_SIZE, false);
  addTextBox(webContentBuffer, INPUT_PUSHOVER_USER, F("Users"), KEY_PUSHOVER_USER, 0, MAX_USER_SIZE, false);
  addFormHeaderEnd(webContentBuffer);
#endif

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);
  WebServer->sendHeaderEnd();
}
#endif

#ifdef SUPLA_IMPULSE_COUNTER
void handleImpulseCounterSet() {
  if (!WebServer->isLoggedIn()) {
    return;
  }
  supla_impulse_counter_set(0);
}

void handleImpulseCounterSaveSet() {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  String readUrl, nr, input;
  uint8_t place;

  String path = PATH_START;
  path += PATH_SAVE_IMPULSE_COUNTER_SET;
  readUrl = WebServer->httpServer->uri();

  place = readUrl.indexOf(path);
  nr = readUrl.substring(place + path.length(), place + path.length() + 3);
  uint8_t key = KEY_GPIO + ConfigESP->getGpio(nr.toInt(), FUNCTION_IMPULSE_COUNTER);

  input = INPUT_IMPULSE_COUNTER_PULL_UP;
  input += nr;

  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(key, MEMORY, 1);
  }
  else {
    ConfigManager->setElement(key, MEMORY, 0);
  }

  input = INPUT_IMPULSE_COUNTER_RAISING_EDGE;
  input += nr;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(key, LEVEL_RELAY, 1);
  }
  else {
    ConfigManager->setElement(key, LEVEL_RELAY, 0);
  }

  ConfigManager->set(KEY_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT, WebServer->httpServer->arg(INPUT_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT).c_str());
  Supla::GUI::impulseCounter[nr.toInt() - 1]->setCounter((unsigned long long)WebServer->httpServer->arg(INPUT_IMPULSE_COUNTER_CHANGE_VALUE).toInt());
  Supla::Storage::ScheduleSave(2000);

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      //      Serial.println(F("E_CONFIG_OK: Dane zapisane"));
      suplaWebPageOther(1);
      break;

    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      suplaWebPageOther(2);
      break;
  }
}

void supla_impulse_counter_set(int save) {
  String readUrl, nr;
  uint8_t place, selected, suported;

  String path = PATH_START;
  path += PATH_IMPULSE_COUNTER_SET;
  readUrl = WebServer->httpServer->uri();

  place = readUrl.indexOf(path);
  nr = readUrl.substring(place + path.length(), place + path.length() + 3);

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_OTHER);

  if (nr.toInt() <= ConfigManager->get(KEY_MAX_IMPULSE_COUNTER)->getValueInt() &&
      ConfigESP->getGpio(nr.toInt(), FUNCTION_IMPULSE_COUNTER) != OFF_GPIO) {
    addForm(webContentBuffer, F("post"), PATH_SAVE_IMPULSE_COUNTER_SET + nr);
    addFormHeader(webContentBuffer, S_IMPULSE_COUNTER_SETTINGS_NR + nr);

    selected = ConfigESP->getMemory(nr.toInt(), FUNCTION_IMPULSE_COUNTER);
    addCheckBox(webContentBuffer, INPUT_IMPULSE_COUNTER_PULL_UP + nr, S_IMPULSE_COUNTER_PULL_UP, selected);

    selected = ConfigESP->getLevel(nr.toInt(), FUNCTION_IMPULSE_COUNTER);
    addCheckBox(webContentBuffer, INPUT_IMPULSE_COUNTER_RAISING_EDGE + nr, S_IMPULSE_COUNTER_RAISING_EDGE, selected);

    addNumberBox(webContentBuffer, INPUT_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT, S_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT, KEY_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT);

    uint32_t count = Supla::GUI::impulseCounter[nr.toInt() - 1]->getCounter();
    addNumberBox(webContentBuffer, INPUT_IMPULSE_COUNTER_CHANGE_VALUE, S_IMPULSE_COUNTER_CHANGE_VALUE, F(""), false, String(count));

    addFormHeaderEnd(webContentBuffer);
    addButtonSubmit(webContentBuffer, S_SAVE);
    addFormEnd(webContentBuffer);
  }

  addButton(webContentBuffer, S_RETURN, PATH_OTHER);
  WebServer->sendContent();
}
#endif

#if defined(SUPLA_HLW8012)
void handleHLW8012Calibrate() {
  if (!WebServer->isLoggedIn()) {
    return;
  }
  suplaWebpageHLW8012Calibrate(0);
}

void handleHLW8012CalibrateSave() {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  double calibPower, calibVoltage = 0;
  String input = INPUT_CALIB_POWER;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    calibPower = WebServer->httpServer->arg(input).toDouble();
  }

  input = INPUT_CALIB_VOLTAGE;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    calibVoltage = WebServer->httpServer->arg(input).toDouble();
  }

  if (calibPower && calibVoltage) {
#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
    for (int i = 0; i < Supla::GUI::relay.size(); i++) {
      Supla::GUI::relay[i]->turnOn();
    }
#endif
    Supla::GUI::counterHLW8012->calibrate(calibPower, calibVoltage);
    suplaWebpageHLW8012Calibrate(1);
  }
  else {
    suplaWebpageHLW8012Calibrate(6);
  }
}

void suplaWebpageHLW8012Calibrate(uint8_t save) {
  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_HLW8012_CALIBRATE);

  addFormHeader(webContentBuffer);
  webContentBuffer += F("<p style='color:#000;'>Current Multi: ");
  webContentBuffer += Supla::GUI::counterHLW8012->getCurrentMultiplier();
  webContentBuffer += F("<br>Voltage Multi: ");
  webContentBuffer += Supla::GUI::counterHLW8012->getVoltageMultiplier();
  webContentBuffer += F("<br>Power Multi: ");
  webContentBuffer += Supla::GUI::counterHLW8012->getPowerMultiplier();
  webContentBuffer += F("</p>");
  addFormHeaderEnd(webContentBuffer);

  addForm(webContentBuffer, F("post"), PATH_SAVE_HLW8012_CALIBRATE);
  addFormHeader(webContentBuffer, S_CALIBRATION_SETTINGS);
  addNumberBox(webContentBuffer, INPUT_CALIB_POWER, S_BULB_POWER_W, F("25"), true);
  addNumberBox(webContentBuffer, INPUT_CALIB_VOLTAGE, S_VOLTAGE_V, F("230"), true);
  addFormHeaderEnd(webContentBuffer);

  addButtonSubmit(webContentBuffer, S_CALIBRATION);
  addFormEnd(webContentBuffer);

  addButton(webContentBuffer, S_RETURN, PATH_OTHER);
  WebServer->sendContent();
}
#endif
