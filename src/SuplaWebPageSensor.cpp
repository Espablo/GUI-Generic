#include "SuplaWebPageSensor.h"

#include "SuplaDeviceGUI.h"
#include "SuplaWebServer.h"
#include "SuplaCommonPROGMEM.h"
#include "GUIGenericCommon.h"
#include "Markup.h"

SuplaWebPageSensor *WebPageSensor = new SuplaWebPageSensor();

SuplaWebPageSensor::SuplaWebPageSensor() {
}

void SuplaWebPageSensor::createWebPageSensor() {
  String path;

#if defined(SUPLA_DS18B20) || defined(SUPLA_DHT11) || defined(SUPLA_DHT22) || defined(SUPLA_SI7021_SONOFF)
  path = PATH_START;
  path += PATH_1WIRE;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageSensor::handle1Wire, this));
  path = PATH_START;
  path += PATH_SAVE_1WIRE;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageSensor::handle1WireSave, this));
#ifdef SUPLA_DS18B20
  path = PATH_START;
  path += PATH_MULTI_DS;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageSensor::handleSearchDS, this));
  path = PATH_START;
  path += PATH_SAVE_MULTI_DS;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageSensor::handleDSSave, this));
#endif
#endif

#if defined(SUPLA_BME280) || defined(SUPLA_SHT3x) || defined(SUPLA_SI7021) || defined(SUPLA_MCP23017)
  path = PATH_START;
  path += PATH_I2C;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageSensor::handlei2c, this));
  path = PATH_START;
  path += PATH_SAVE_I2C;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageSensor::handlei2cSave, this));
#endif

#if defined(SUPLA_MAX6675)
  path = PATH_START;
  path += PATH_SPI;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageSensor::handleSpi, this));
  path = PATH_START;
  path += PATH_SAVE_SPI;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageSensor::handleSpiSave, this));
#endif

#if defined(SUPLA_HC_SR04) || defined(SUPLA_IMPULSE_COUNTER)
  path = PATH_START;
  path += PATH_OTHER;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageSensor::handleOther, this));
  path = PATH_START;
  path += PATH_SAVE_OTHER;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageSensor::handleOtherSave, this));

#if defined(SUPLA_IMPULSE_COUNTER)
  for (uint8_t i = 1; i <= ConfigManager->get(KEY_MAX_IMPULSE_COUNTER)->getValueInt(); i++) {
    path = PATH_START;
    path += PATH_IMPULSE_COUNTER_SET;
    path += i;
    WebServer->httpServer.on(path, std::bind(&SuplaWebPageSensor::handleImpulseCounterSet, this));

    path = PATH_START;
    path += PATH_SAVE_IMPULSE_COUNTER_SET;
    path += i;
    WebServer->httpServer.on(path, std::bind(&SuplaWebPageSensor::handleImpulseCounterSaveSet, this));
  }
#endif

#if defined(SUPLA_HLW8012)
  path = PATH_START;
  path += PATH_HLW8012_CALIBRATE;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageSensor::handleHLW8012Calibrate, this));

  path = PATH_START;
  path += PATH_SAVE_HLW8012_CALIBRATE;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageSensor::handleHLW8012CalibrateSave, this));
#endif
#endif
}

#ifdef SUPLA_DS18B20
void SuplaWebPageSensor::handleSearchDS() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  supla_webpage_search(0);
}

void SuplaWebPageSensor::handleDSSave() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  for (uint8_t i = 0; i < ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt(); i++) {
    String dsAddr = INPUT_DS18B20_ADDR;
    String dsName = INPUT_DS18B20_NAME;
    dsAddr += i;
    dsName += i;

    ConfigManager->setElement(KEY_ADDR_DS18B20, i, WebServer->httpServer.arg(dsAddr).c_str());

    if (strcmp(WebServer->httpServer.arg(dsName).c_str(), "") != 0) {
      ConfigManager->setElement(KEY_NAME_SENSOR, i, WebServer->httpServer.arg(dsName).c_str());
    }

    Supla::GUI::sensorDS[i]->setDeviceAddress(HexToBytes(ConfigManager->get(KEY_ADDR_DS18B20)->getElement(i)));
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      // Serial.println(F("E_CONFIG_OK: Config save"));
      supla_webpage_search(1);
      // WebServer->rebootESP();
      break;
    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      supla_webpage_search(2);
      break;
  }
}

void SuplaWebPageSensor::supla_webpage_search(int save) {
  uint8_t count = 0;
  uint8_t pin = ConfigESP->getGpio(FUNCTION_DS18B20);

  OneWire ow(pin);
  DallasTemperature sensors;
  DeviceAddress address;
  char strAddr[64];
  uint8_t i;

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_MULTI_DS);
  webContentBuffer += F("<center>");
  if (ConfigESP->getGpio(FUNCTION_DS18B20) < OFF_GPIO) {
    webContentBuffer += F("<form method='post' action='");
    webContentBuffer += PATH_SAVE_MULTI_DS;
    webContentBuffer += F("'>");
    this->showDS18B20();
    webContentBuffer += F("<button type='submit'>");
    webContentBuffer += S_SAVE;
    webContentBuffer += F("</button></form>");
    webContentBuffer += F("<br>");
  }
  webContentBuffer += F("<form method='post' action='");
  webContentBuffer += PATH_SAVE_MULTI_DS;
  webContentBuffer += F("'>");
  webContentBuffer += F("<div class='w'>");
  webContentBuffer += F("<h3>");
  webContentBuffer += S_FOUND;
  webContentBuffer += F(" DS18b20</h3>");
  sensors.setOneWire(&ow);
  sensors.begin();
  if (sensors.isParasitePowerMode()) {
    supla_log(LOG_DEBUG, "OneWire(pin %d) Parasite power is ON", pin);
  }
  else {
    supla_log(LOG_DEBUG, "OneWire(pin %d) Parasite power is OFF", pin);
  }
  // report parasite power requirements
  for (i = 0; i < sensors.getDeviceCount(); i++) {
    if (!sensors.getAddress(address, i)) {
      supla_log(LOG_DEBUG, "Unable to find address for Device %d", i);
    }
    else {
      sprintf(strAddr, "%02X%02X%02X%02X%02X%02X%02X%02X", address[0], address[1], address[2], address[3], address[4], address[5], address[6],
              address[7]);
      supla_log(LOG_DEBUG, "Index %d - address %s", i, strAddr);

      webContentBuffer += F("<i><input name='");
      webContentBuffer += INPUT_DS18B20_ADDR;
      webContentBuffer += count;

      webContentBuffer += F("' value='");
      webContentBuffer += String(strAddr);
      webContentBuffer += F("' maxlength=");
      webContentBuffer += MAX_DS18B20_ADDRESS_HEX;
      webContentBuffer += F(" readonly><label></i>");

      count++;
    }
    delay(0);
  }

  if (count == 0) {
    webContentBuffer += F("<i><label>");
    webContentBuffer += S_NO_SENSORS_CONNECTED;
    webContentBuffer += F("</label></i>");
  }
  webContentBuffer += F("</div>");
  webContentBuffer += F("</center>");
  webContentBuffer += F("<button type='submit'>");
  webContentBuffer += S_SAVE_FOUND;
  webContentBuffer += F("DS18b20</button></form>");
  webContentBuffer += F("<br><br>");
  webContentBuffer += F("<a href='");
  webContentBuffer += PATH_START;
  webContentBuffer += PATH_1WIRE;
  webContentBuffer += F("'><button>");
  webContentBuffer += S_RETURN;
  webContentBuffer += F("</button></a><br><br>");
  WebServer->sendContent();
}

void SuplaWebPageSensor::showDS18B20(bool readonly) {
  if (ConfigESP->getGpio(FUNCTION_DS18B20) != OFF_GPIO) {
    webContentBuffer += F("<div class='w'>");
    webContentBuffer += F("<h3>");
    webContentBuffer += S_TEMPERATURE;
    webContentBuffer += F("</h3>");
    for (uint8_t i = 0; i < ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt(); i++) {
      double temp = Supla::GUI::sensorDS[i]->getValue();
      webContentBuffer += F("<i style='border-bottom:none !important;'><input name='");
      webContentBuffer += INPUT_DS18B20_NAME;
      webContentBuffer += i;
      webContentBuffer += F("' value='");
      webContentBuffer += String(ConfigManager->get(KEY_NAME_SENSOR)->getElement(i));
      webContentBuffer += F("' maxlength=");
      webContentBuffer += MAX_DS18B20_NAME;
      if (readonly) {
        webContentBuffer += F(" readonly");
      }
      webContentBuffer += F("><label>");
      webContentBuffer += S_NAME;
      webContentBuffer += i + 1;
      webContentBuffer += F("</label></i>");
      webContentBuffer += F("<i><input name='");
      webContentBuffer += INPUT_DS18B20_ADDR;
      webContentBuffer += i;
      webContentBuffer += F("' value='");
      webContentBuffer += String(ConfigManager->get(KEY_ADDR_DS18B20)->getElement(i));
      webContentBuffer += F("' maxlength=");
      webContentBuffer += MAX_DS18B20_ADDRESS_HEX;
      if (readonly) {
        webContentBuffer += F(" readonly");
      }
      webContentBuffer += F("><label>");
      if (temp != -275) {
        webContentBuffer += temp;
      }
      else {
        webContentBuffer += F("--.--");
      }
      webContentBuffer += F(" <b>&deg;C</b> ");
      webContentBuffer += F("</label></i>");
      delay(0);
    }
    webContentBuffer += F("</div>");
  }
}
#endif

#if defined(SUPLA_DS18B20) || defined(SUPLA_DHT11) || defined(SUPLA_DHT22) || defined(SUPLA_SI7021_SONOFF)
void SuplaWebPageSensor::handle1Wire() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  supla_webpage_1wire(0);
}

void SuplaWebPageSensor::handle1WireSave() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  uint8_t nr, last_value;

#ifdef SUPLA_DHT11
  last_value = ConfigManager->get(KEY_MAX_DHT11)->getValueInt();
  for (nr = 1; nr <= last_value; nr++) {
    if (!WebServer->saveGPIO(INPUT_DHT11_GPIO, FUNCTION_DHT11, nr, INPUT_MAX_DHT11)) {
      supla_webpage_1wire(6);
      return;
    }
  }

  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_DHT11).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_DHT11, WebServer->httpServer.arg(INPUT_MAX_DHT11).c_str());
  }
#endif

#ifdef SUPLA_DHT22
  last_value = ConfigManager->get(KEY_MAX_DHT22)->getValueInt();
  for (nr = 1; nr <= last_value; nr++) {
    if (!WebServer->saveGPIO(INPUT_DHT22_GPIO, FUNCTION_DHT22, nr, INPUT_MAX_DHT22)) {
      supla_webpage_1wire(6);
      return;
    }
  }

  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_DHT22).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_DHT22, WebServer->httpServer.arg(INPUT_MAX_DHT22).c_str());
  }
#endif

#ifdef SUPLA_DS18B20
  if (!WebServer->saveGPIO(INPUT_MULTI_DS_GPIO, FUNCTION_DS18B20)) {
    supla_webpage_1wire(6);
    return;
  }
  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_DS18B20).c_str(), "") > 0) {
    ConfigManager->set(KEY_MULTI_MAX_DS18B20, WebServer->httpServer.arg(INPUT_MAX_DS18B20).c_str());
  }
#endif

#ifdef SUPLA_SI7021_SONOFF
  if (!WebServer->saveGPIO(INPUT_SI7021_SONOFF, FUNCTION_SI7021_SONOFF)) {
    supla_webpage_1wire(6);
    return;
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      supla_webpage_1wire(1);
      break;
    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      supla_webpage_1wire(2);
      break;
  }
}

void SuplaWebPageSensor::supla_webpage_1wire(int save) {
  uint8_t nr, max;

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_1WIRE);
  webContentBuffer += F("<form method='post' action='");
  webContentBuffer += PATH_SAVE_1WIRE;
  webContentBuffer += F("'>");
#ifdef SUPLA_DHT11
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + F(" DHT11"));
  addNumberBox(webContentBuffer, INPUT_MAX_DHT11, S_QUANTITY, KEY_MAX_DHT11, ConfigESP->countFreeGpio(FUNCTION_DHT11));
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_DHT11)->getValueInt(); nr++) {
    addListGPIOBox(webContentBuffer, INPUT_DHT11_GPIO, F("DHT11"), FUNCTION_DHT11, nr);
  }
  addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_DHT22
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + F(" DHT22"));
  addNumberBox(webContentBuffer, INPUT_MAX_DHT22, S_QUANTITY, KEY_MAX_DHT22, ConfigESP->countFreeGpio(FUNCTION_DHT22));
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_DHT22)->getValueInt(); nr++) {
    addListGPIOBox(webContentBuffer, INPUT_DHT22_GPIO, F("DHT22"), FUNCTION_DHT22, nr);
  }
  addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_SI7021_SONOFF
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + F(" Si7021 Sonoff"));
  addListGPIOBox(webContentBuffer, INPUT_SI7021_SONOFF, F("Si7021 Sonoff"), FUNCTION_SI7021_SONOFF);
  addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_DS18B20
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + F(" Multi DS18B20"));
  addNumberBox(webContentBuffer, INPUT_MAX_DS18B20, S_QUANTITY, KEY_MULTI_MAX_DS18B20, MAX_DS18B20);
  if (ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt() > 1) {
    addListGPIOLinkBox(webContentBuffer, INPUT_MULTI_DS_GPIO, F("MULTI DS18B20"), FUNCTION_DS18B20, PATH_MULTI_DS);
  }
  else {
    addListGPIOBox(webContentBuffer, INPUT_MULTI_DS_GPIO, F("MULTI DS18B20"), FUNCTION_DS18B20);
  }
  addFormHeaderEnd(webContentBuffer);
#endif

  webContentBuffer += F("<button type='submit'>");
  webContentBuffer += S_SAVE;
  webContentBuffer += F("</button></form>");
  webContentBuffer += F("<br>");
  webContentBuffer += F("<a href='");
  webContentBuffer += PATH_START;
  webContentBuffer += PATH_DEVICE_SETTINGS;
  webContentBuffer += F("'><button>");
  webContentBuffer += S_RETURN;
  webContentBuffer += F("</button></a><br><br>");

  WebServer->sendContent();
}
#endif

#if defined(SUPLA_BME280) || defined(SUPLA_SI7021) || defined(SUPLA_SHT3x) || defined(SUPLA_OLED) || defined(SUPLA_MCP23017)
void SuplaWebPageSensor::handlei2c() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  supla_webpage_i2c(0);
}

void SuplaWebPageSensor::handlei2cSave() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  String input;
  uint8_t key;

  if (!WebServer->saveGPIO(INPUT_SDA_GPIO, FUNCTION_SDA)) {
    supla_webpage_i2c(6);
    return;
  }
  if (!WebServer->saveGPIO(INPUT_SCL_GPIO, FUNCTION_SCL)) {
    supla_webpage_i2c(6);
    return;
  }

#ifdef SUPLA_BME280
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_BME280;
  if (strcmp(WebServer->httpServer.arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_BME280, WebServer->httpServer.arg(input).toInt());
  }

  key = KEY_ALTITUDE_BME280;
  input = INPUT_ALTITUDE_BME280;
  if (strcmp(WebServer->httpServer.arg(INPUT_ALTITUDE_BME280).c_str(), "") != 0) {
    ConfigManager->set(key, WebServer->httpServer.arg(input).c_str());
  }
#endif

#ifdef SUPLA_SHT3x
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_SHT3x;
  if (strcmp(WebServer->httpServer.arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_SHT3x, WebServer->httpServer.arg(input).toInt());
  }
#endif

#ifdef SUPLA_SI7021
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_SI7021;
  if (strcmp(WebServer->httpServer.arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_SI7021, WebServer->httpServer.arg(input).toInt());
  }
#endif

#ifdef SUPLA_OLED
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_OLED;
  if (strcmp(WebServer->httpServer.arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_OLED, WebServer->httpServer.arg(input).toInt());
  }

  for (uint8_t i = 0; i < getNumberChannels(); i++) {
    input = INPUT_DS18B20_NAME;
    input += i;
    if (strcmp(WebServer->httpServer.arg(input).c_str(), "") != 0) {
      ConfigManager->setElement(KEY_NAME_SENSOR, i, WebServer->httpServer.arg(input).c_str());
    }
  }
#endif

#ifdef SUPLA_MCP23017
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_MCP23017;
  if (strcmp(WebServer->httpServer.arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_MCP23017, WebServer->httpServer.arg(input).toInt());

    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_MCP23017).toInt()) {
      ConfigESP->clearFunctionGpio(FUNCTION_RELAY);
      ConfigESP->clearFunctionGpio(FUNCTION_BUTTON);
    }
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      supla_webpage_i2c(1);
      break;
    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      supla_webpage_i2c(2);
      break;
  }
}

void SuplaWebPageSensor::supla_webpage_i2c(int save) {
  uint8_t selected;

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_I2C);

  addForm(webContentBuffer, F("post"), PATH_SAVE_I2C);
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + F(" i2c"));
  addListGPIOBox(webContentBuffer, INPUT_SDA_GPIO, F("SDA"), FUNCTION_SDA);
  addListGPIOBox(webContentBuffer, INPUT_SCL_GPIO, F("SCL"), FUNCTION_SCL);
  addFormHeaderEnd(webContentBuffer);

  if (ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO) {
#ifdef SUPLA_BME280
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_BME280).toInt();
    addFormHeader(webContentBuffer);
    addListBox(webContentBuffer, INPUT_BME280, F("BME280 adres"), BME280_P, 4, selected);
    addNumberBox(webContentBuffer, INPUT_ALTITUDE_BME280, S_ALTITUDE_ABOVE_SEA_LEVEL, KEY_ALTITUDE_BME280, 1500);
    addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_SHT3x
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_SHT3x).toInt();
    addFormHeader(webContentBuffer);
    addListBox(webContentBuffer, INPUT_SHT3x, F("SHT3x"), SHT3x_P, 4, selected);
    addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_SI7021
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_SI7021).toInt();
    addFormHeader(webContentBuffer);
    addListBox(webContentBuffer, INPUT_SI7021, F("SI7021"), STATE_P, 2, selected);
    addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_OLED
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_OLED).toInt();
    addFormHeader(webContentBuffer);
    addListBox(webContentBuffer, INPUT_OLED, F("OLED"), OLED_P, 4, selected);
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_OLED).toInt()) {
      String name, sensorName, input;

      for (uint8_t i = 0; i < getNumberChannels(); i++) {
        sensorName = String(ConfigManager->get(KEY_NAME_SENSOR)->getElement(i));
        input = INPUT_DS18B20_NAME;
        input += i;
        name = F("Ekran ");
        name += i + 1;
        addTextBox(webContentBuffer, input, name, sensorName, 0, MAX_DS18B20_NAME, false);
      }
    }
    addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_MCP23017
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_MCP23017).toInt();
    addFormHeader(webContentBuffer);
    addListBox(webContentBuffer, INPUT_MCP23017, F("MCP23017"), STATE_P, 2, selected);
    addFormHeaderEnd(webContentBuffer);
#endif
  }

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);

  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);
  WebServer->sendContent();
}
#endif

#if defined(SUPLA_MAX6675)
void SuplaWebPageSensor::handleSpi() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  supla_webpage_spi(0);
}

void SuplaWebPageSensor::handleSpiSave() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  String input;
  uint8_t key;

#if defined(SUPLA_MAX6675)
  if (!WebServer->saveGPIO(INPUT_CLK_GPIO, FUNCTION_CLK)) {
    supla_webpage_spi(6);
    return;
  }
  if (!WebServer->saveGPIO(INPUT_CS_GPIO, FUNCTION_CS)) {
    supla_webpage_spi(6);
    return;
  }
  if (!WebServer->saveGPIO(INPUT_D0_GPIO, FUNCTION_D0)) {
    supla_webpage_spi(6);
    return;
  }
#endif

#ifdef SUPLA_MAX6675
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_MAX6675;
  if (strcmp(WebServer->httpServer.arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_MAX6675, WebServer->httpServer.arg(input).toInt());
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      supla_webpage_spi(1);
      break;
    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      supla_webpage_spi(2);
      break;
  }
}

void SuplaWebPageSensor::supla_webpage_spi(int save) {
  uint8_t nr, suported, selected;

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_SPI);
  webContentBuffer += F("<form method='post' action='");
  webContentBuffer += PATH_SAVE_SPI;
  webContentBuffer += F("'>");

#if defined(SUPLA_MAX6675)
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + F(" SPI"));
  addListGPIOBox(webContentBuffer, INPUT_CLK_GPIO, F("CLK"), FUNCTION_CLK);
  addListGPIOBox(webContentBuffer, INPUT_CS_GPIO, F("CS"), FUNCTION_CS);
  addListGPIOBox(webContentBuffer, INPUT_D0_GPIO, F("D0"), FUNCTION_D0);

  if (ConfigESP->getGpio(FUNCTION_CLK) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_CS) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_D0) != OFF_GPIO) {
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_MAX6675).toInt();
    addListBox(webContentBuffer, INPUT_MAX6675, F("MAX6675"), STATE_P, 2, selected);
  }
  addFormHeaderEnd(webContentBuffer);
#endif
  webContentBuffer += F("<button type='submit'>");
  webContentBuffer += S_SAVE;
  webContentBuffer += F("</button></form>");
  webContentBuffer += F("<br>");
  webContentBuffer += F("<a href='");
  webContentBuffer += PATH_START;
  webContentBuffer += PATH_DEVICE_SETTINGS;
  webContentBuffer += F("'><button>");
  webContentBuffer += S_RETURN;
  webContentBuffer += F("</button></a><br><br>");
  WebServer->sendContent();
}
#endif

#if defined(SUPLA_HC_SR04) || defined(SUPLA_IMPULSE_COUNTER) || defined(SUPLA_HLW8012)
void SuplaWebPageSensor::handleOther() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  supla_webpage_other(0);
}

void SuplaWebPageSensor::handleOtherSave() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  uint8_t nr, current_value, last_value;

#ifdef SUPLA_HC_SR04
  if (!WebServer->saveGPIO(INPUT_TRIG_GPIO, FUNCTION_TRIG)) {
    supla_webpage_other(6);
    return;
  }
  if (!WebServer->saveGPIO(INPUT_ECHO_GPIO, FUNCTION_ECHO)) {
    supla_webpage_other(6);
    return;
  }
#endif

#ifdef SUPLA_IMPULSE_COUNTER
  //Supla::GUI::impulseCounter[0]->setCounter((unsigned long long)WebServer->httpServer.arg(INPUT_IMPULSE_COUNTER_CHANGE_VALUE).toInt());

  last_value = ConfigManager->get(KEY_MAX_IMPULSE_COUNTER)->getValueInt();
  for (nr = 1; nr <= last_value; nr++) {
    if (!WebServer->saveGPIO(INPUT_IMPULSE_COUNTER_GPIO, FUNCTION_IMPULSE_COUNTER, nr, INPUT_MAX_IMPULSE_COUNTER)) {
      supla_webpage_other(6);
      return;
    }
  }

  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_IMPULSE_COUNTER).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_IMPULSE_COUNTER, WebServer->httpServer.arg(INPUT_MAX_IMPULSE_COUNTER).c_str());
  }
#endif

#ifdef SUPLA_HLW8012
  if (!WebServer->saveGPIO(INPUT_CF, FUNCTION_CF)) {
    supla_webpage_other(6);
    return;
  }
  if (!WebServer->saveGPIO(INPUT_CF1, FUNCTION_CF1)) {
    supla_webpage_other(6);
    return;
  }
  if (!WebServer->saveGPIO(INPUT_SEL, FUNCTION_SEL)) {
    supla_webpage_other(6);
    return;
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      supla_webpage_other(1);
      break;
    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      supla_webpage_other(2);
      break;
  }
}

void SuplaWebPageSensor::supla_webpage_other(int save) {
  uint8_t nr, suported, selected;

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_OTHER);

  addForm(webContentBuffer, F("post"), PATH_SAVE_OTHER);
#ifdef SUPLA_HC_SR04
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + F(" HC-SR04"));
  addListGPIOBox(webContentBuffer, INPUT_TRIG_GPIO, F("TRIG"), FUNCTION_TRIG);
  addListGPIOBox(webContentBuffer, INPUT_ECHO_GPIO, F("ECHO"), FUNCTION_ECHO);
  addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_IMPULSE_COUNTER
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + " " + S_IMPULSE_COUNTER);
  addNumberBox(webContentBuffer, INPUT_MAX_IMPULSE_COUNTER, S_QUANTITY, KEY_MAX_IMPULSE_COUNTER, ConfigESP->countFreeGpio(FUNCTION_IMPULSE_COUNTER));
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_IMPULSE_COUNTER)->getValueInt(); nr++) {
    addListGPIOLinkBox(webContentBuffer, INPUT_IMPULSE_COUNTER_GPIO, F("IC GPIO"), FUNCTION_IMPULSE_COUNTER, PATH_IMPULSE_COUNTER_SET, nr);
  }
  addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_HLW8012
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + F(" HLW8012"));
  addListGPIOBox(webContentBuffer, INPUT_CF, F("CF"), FUNCTION_CF);
  addListGPIOBox(webContentBuffer, INPUT_CF1, F("CF1"), FUNCTION_CF1);
  addListGPIOBox(webContentBuffer, INPUT_SEL, F("SELi"), FUNCTION_SEL);
  if (ConfigESP->getGpio(FUNCTION_CF) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_CF1) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SEL) != OFF_GPIO) {
    addLinkBox(webContentBuffer, F("Kalibracja"), PATH_HLW8012_CALIBRATE);
  }
  addFormHeaderEnd(webContentBuffer);
#endif

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);
  WebServer->sendContent();
}
#endif

#ifdef SUPLA_IMPULSE_COUNTER
void SuplaWebPageSensor::handleImpulseCounterSet() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  supla_impulse_counter_set(0);
}

void SuplaWebPageSensor::handleImpulseCounterSaveSet() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  String readUrl, nr, input;
  uint8_t place;

  String path = PATH_START;
  path += PATH_SAVE_IMPULSE_COUNTER_SET;
  readUrl = WebServer->httpServer.uri();

  place = readUrl.indexOf(path);
  nr = readUrl.substring(place + path.length(), place + path.length() + 3);
  uint8_t key = KEY_GPIO + ConfigESP->getGpio(nr.toInt(), FUNCTION_IMPULSE_COUNTER);

  input = INPUT_IMPULSE_COUNTER_PULL_UP;
  input += nr;
  ConfigManager->setElement(key, MEMORY, WebServer->httpServer.arg(input).toInt());

  input = INPUT_IMPULSE_COUNTER_RAISING_EDGE;
  input += nr;
  ConfigManager->setElement(key, LEVEL, WebServer->httpServer.arg(input).toInt());

  ConfigManager->set(KEY_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT, WebServer->httpServer.arg(INPUT_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT).c_str());
  Supla::GUI::impulseCounter[nr.toInt() - 1]->setCounter((unsigned long long)WebServer->httpServer.arg(INPUT_IMPULSE_COUNTER_CHANGE_VALUE).toInt());

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      //      Serial.println(F("E_CONFIG_OK: Dane zapisane"));
      supla_webpage_other(1);
      break;

    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      supla_webpage_other(2);
      break;
  }
}

void SuplaWebPageSensor::supla_impulse_counter_set(int save) {
  String readUrl, nr;
  uint8_t place, selected, suported;

  String path = PATH_START;
  path += PATH_IMPULSE_COUNTER_SET;
  readUrl = WebServer->httpServer.uri();

  place = readUrl.indexOf(path);
  nr = readUrl.substring(place + path.length(), place + path.length() + 3);

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_OTHER);
  uint8_t relays = ConfigManager->get(KEY_MAX_IMPULSE_COUNTER)->getValueInt();
  if (nr.toInt() <= relays && ConfigESP->getGpio(nr.toInt(), FUNCTION_IMPULSE_COUNTER) != OFF_GPIO) {
    webContentBuffer += F("<form method='post' action='");
    webContentBuffer += PATH_SAVE_IMPULSE_COUNTER_SET;
    webContentBuffer += nr;
    webContentBuffer += F("'><div class='w'><h3>");
    webContentBuffer += S_IMPULSE_COUNTER_SETTINGS_NR;
    webContentBuffer += F(" ");
    webContentBuffer += nr;
    webContentBuffer += F("</h3>");
    webContentBuffer += F("<i><label>");
    webContentBuffer += S_IMPULSE_COUNTER_PULL_UP;
    webContentBuffer += F("</label><select name='");
    webContentBuffer += INPUT_IMPULSE_COUNTER_PULL_UP;
    webContentBuffer += nr;
    webContentBuffer += F("'>");
    selected = ConfigESP->getMemory(nr.toInt(), FUNCTION_IMPULSE_COUNTER);
    for (suported = 0; suported < 2; suported++) {
      webContentBuffer += F("<option value='");
      webContentBuffer += suported;
      if (selected == suported) {
        webContentBuffer += F("' selected>");
      }
      else
        webContentBuffer += F("'>");
      webContentBuffer += PGMT(STATE_P[suported]);
    }
    webContentBuffer += F("</select></i>");
    webContentBuffer += F("<i><label>");
    webContentBuffer += S_IMPULSE_COUNTER_RAISING_EDGE;
    webContentBuffer += F("</label><select name='");
    webContentBuffer += INPUT_IMPULSE_COUNTER_RAISING_EDGE;
    webContentBuffer += nr;
    webContentBuffer += F("'>");
    selected = ConfigESP->getLevel(nr.toInt(), FUNCTION_IMPULSE_COUNTER);
    for (suported = 0; suported < 2; suported++) {
      webContentBuffer += F("<option value='");
      webContentBuffer += suported;
      if (selected == suported) {
        webContentBuffer += F("' selected>");
      }
      else
        webContentBuffer += F("'>");
      webContentBuffer += PGMT(STATE_P[suported]);
    }
    webContentBuffer += F("</select></i>");
    addNumberBox(webContentBuffer, INPUT_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT, S_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT, KEY_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT,
                 99999999);
    webContentBuffer += F("<i><label>");
    webContentBuffer += S_IMPULSE_COUNTER_CHANGE_VALUE;
    webContentBuffer += F("</label><input name='");
    webContentBuffer += INPUT_IMPULSE_COUNTER_CHANGE_VALUE;
    webContentBuffer += F("' type='number' placeholder='0' step='1' min='0' max='");
    webContentBuffer += 100;
    webContentBuffer += F("' value='");
    uint32_t count = Supla::GUI::impulseCounter[nr.toInt() - 1]->getCounter();
    webContentBuffer += count;
    webContentBuffer += F("'></i>");

    webContentBuffer += F("</div><button type='submit'>");
    webContentBuffer += S_SAVE;
    webContentBuffer += F("</button></form>");
  }
  else {
    webContentBuffer += F("<div class='w'><h3>");
    webContentBuffer += S_NO_IMPULSE_COUNTER_NR;
    webContentBuffer += F(" ");
    webContentBuffer += nr;
    webContentBuffer += F("</h3>");
  }
  webContentBuffer += F("<br>");
  webContentBuffer += F("<a href='");
  webContentBuffer += PATH_START;
  webContentBuffer += PATH_OTHER;
  webContentBuffer += F("'><button>");
  webContentBuffer += S_RETURN;
  webContentBuffer += F("</button></a><br><br>");
  WebServer->sendContent();
}
#endif

#if defined(SUPLA_HLW8012)
void SuplaWebPageSensor::handleHLW8012Calibrate() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  suplaWebpageHLW8012Calibrate(0);
}

void SuplaWebPageSensor::handleHLW8012CalibrateSave() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  double calibPower, calibVoltage = 0;
  String input = INPUT_CALIB_POWER;
  if (strcmp(WebServer->httpServer.arg(input).c_str(), "") != 0) {
    calibPower = WebServer->httpServer.arg(input).toDouble();
  }

  input = INPUT_CALIB_VOLTAGE;
  if (strcmp(WebServer->httpServer.arg(input).c_str(), "") != 0) {
    calibVoltage = WebServer->httpServer.arg(input).toDouble();
  }

  if (calibPower && calibVoltage) {
    Supla::GUI::counterHLW8012->calibrate(calibPower, calibVoltage);
    suplaWebpageHLW8012Calibrate(1);
  }
  else {
    suplaWebpageHLW8012Calibrate(6);
  }
}

void SuplaWebPageSensor::suplaWebpageHLW8012Calibrate(uint8_t save) {
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
  addFormHeader(webContentBuffer, F("Ustawienia kalibracji"));
  addNumberBox(webContentBuffer, INPUT_CALIB_POWER, F("Moc żarówki [W]"), F("25"), true);
  addNumberBox(webContentBuffer, INPUT_CALIB_VOLTAGE, F("Napięcie [V]"), F("230"), true);
  addFormHeaderEnd(webContentBuffer);

  addButtonSubmit(webContentBuffer, F("Kalibracja"));
  addFormEnd(webContentBuffer);

  addButton(webContentBuffer, S_RETURN, PATH_OTHER);
  WebServer->sendContent();
}
#endif