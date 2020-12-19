#include "SuplaWebPageSensor.h"

#include "SuplaDeviceGUI.h"
#include "SuplaWebServer.h"
#include "SuplaCommonPROGMEM.h"
#include "GUIGenericCommon.h"
#include "Markup.h"
#include "SuplaOled.h"

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

#if defined(SUPLA_BME280) || defined(SUPLA_SHT30) || defined(SUPLA_SI7021)
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
  for (uint8_t i = 1; i <= MAX_GPIO; i++) {
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

#endif
}
#ifdef SUPLA_DS18B20
void SuplaWebPageSensor::handleSearchDS() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  WebServer->sendContent(supla_webpage_search(0));
}

void SuplaWebPageSensor::handleDSSave() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  for (uint8_t i = 0; i < ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt(); i++) {
    uint8_t ds_key = KEY_DS + i;
    uint8_t ds_name_key = KEY_DS_NAME + i;

    String ds = F("dschlid");
    String ds_name = F("dsnameid");
    ds += i;
    ds_name += i;

    ConfigManager->set(ds_key, WebServer->httpServer.arg(ds).c_str());
    ConfigManager->set(ds_name_key, WebServer->httpServer.arg(ds_name).c_str());

    Supla::GUI::sensorDS[i]->setDeviceAddress(ConfigManager->get(ds_key)->getValueBin(MAX_DS18B20_ADDRESS));
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      // Serial.println(F("E_CONFIG_OK: Config save"));
      WebServer->sendContent(supla_webpage_search(1));
      // WebServer->rebootESP();
      break;
    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      WebServer->sendContent(supla_webpage_search(2));
      break;
  }
}

String SuplaWebPageSensor::supla_webpage_search(int save) {
  String content = "";
  uint8_t count = 0;
  uint8_t pin = ConfigESP->getGpio(FUNCTION_DS18B20);

  OneWire ow(pin);
  DallasTemperature sensors;
  DeviceAddress address;
  char strAddr[64];
  uint8_t i;

  content += WebServer->SuplaSaveResult(save);
  content += WebServer->SuplaJavaScript(PATH_MULTI_DS);
  content += F("<center>");
  if (ConfigESP->getGpio(FUNCTION_DS18B20) < OFF_GPIO || !Supla::GUI::sensorDS.empty()) {
    content += F("<form method='post' action='");
    content += PATH_SAVE_MULTI_DS;
    content += F("'>");
    this->showDS18B20(content);
    content += F("<button type='submit'>");
    content += S_SAVE;
    content += F("</button></form>");
    content += F("<br>");
  }
  content += F("<form method='post' action='");
  content += PATH_SAVE_MULTI_DS;
  content += F("'>");
  content += F("<div class='w'>");
  content += F("<h3>");
  content += S_FOUND;
  content += F(" DS18b20</h3>");
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

      content += F("<i><input name='dschlid");
      content += count;

      content += F("' value='");
      content += String(strAddr);
      content += F("' maxlength=");
      content += MAX_DS18B20_ADDRESS_HEX;
      content += F(" readonly><label></i>");

      count++;
    }
    delay(0);
  }

  if (count == 0) {
    content += F("<i><label>");
    content += S_NO_SENSORS_CONNECTED;
    content += F("</label></i>");
  }
  content += F("</div>");
  content += F("</center>");
  content += F("<button type='submit'>");
  content += S_SAVE_FOUND;
  content += F("DS18b20</button></form>");
  content += F("<br><br>");
  content += F("<a href='");
  content += PATH_START;
  content += PATH_1WIRE;
  content += F("'><button>");
  content += S_RETURN;
  content += F("</button></a></div>");

  return content;
}

void SuplaWebPageSensor::showDS18B20(String &content, bool readonly) {
  if (ConfigESP->getGpio(FUNCTION_DS18B20) != OFF_GPIO) {
    content += F("<div class='w'>");
    content += F("<h3>");
    content += S_TEMPERATURE;
    content += F("</h3>");
    for (uint8_t i = 0; i < ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt(); i++) {
      uint8_t ds_key = KEY_DS + i;
      uint8_t ds_name_key = KEY_DS_NAME + i;

      double temp = Supla::GUI::sensorDS[i]->getValue();
      content += F("<i style='border-bottom:none !important;'><input name='dsnameid");
      content += i;
      content += F("' value='");
      content += String(ConfigManager->get(ds_name_key)->getValue());
      content += F("' maxlength=");
      content += MAX_DS18B20_NAME;
      if (readonly) {
        content += F(" readonly");
      }
      content += F("><label>");
      content += S_NAME;
      content += i + 1;
      content += F("</label></i>");
      content += F("<i><input name='dschlid");
      content += i;
      content += F("' value='");
      content += String(ConfigManager->get(ds_key)->getValue());
      content += F("' maxlength=");
      content += MAX_DS18B20_ADDRESS_HEX;
      if (readonly) {
        content += F(" readonly");
      }
      content += F("><label>");
      if (temp != -275) {
        content += temp;
      }
      else {
        content += F("--.--");
      }
      content += F(" <b>&deg;C</b> ");
      content += F("</label></i>");
      delay(0);
    }
    content += F("</div>");
  }
}
#endif

#if defined(SUPLA_DS18B20) || defined(SUPLA_DHT11) || defined(SUPLA_DHT22) || defined(SUPLA_SI7021_SONOFF)
void SuplaWebPageSensor::handle1Wire() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  WebServer->sendContent(supla_webpage_1wire(0));
}

void SuplaWebPageSensor::handle1WireSave() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  String key, input;
  uint8_t nr, current_value, last_value;

#ifdef SUPLA_DHT11
  last_value = ConfigManager->get(KEY_MAX_DHT11)->getValueInt();
  for (nr = 1; nr <= last_value; nr++) {
    if (!WebServer->saveGPIO(INPUT_DHT11_GPIO, FUNCTION_DHT11, nr, INPUT_MAX_DHT11)) {
      WebServer->sendContent(supla_webpage_1wire(6));
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
      WebServer->sendContent(supla_webpage_1wire(6));
      return;
    }
  }

  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_DHT22).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_DHT22, WebServer->httpServer.arg(INPUT_MAX_DHT22).c_str());
  }
#endif

#ifdef SUPLA_DS18B20
  if (!WebServer->saveGPIO(INPUT_MULTI_DS_GPIO, FUNCTION_DS18B20)) {
    WebServer->sendContent(supla_webpage_1wire(6));
    return;
  }
  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_DS18B20).c_str(), "") > 0) {
    ConfigManager->set(KEY_MULTI_MAX_DS18B20, WebServer->httpServer.arg(INPUT_MAX_DS18B20).c_str());
  }
#endif

#ifdef SUPLA_SI7021_SONOFF
  if (!WebServer->saveGPIO(INPUT_SI7021_SONOFF, FUNCTION_SI7021_SONOFF)) {
    WebServer->sendContent(supla_webpage_1wire(6));
    return;
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      WebServer->sendContent(supla_webpage_1wire(1));
      break;
    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      WebServer->sendContent(supla_webpage_1wire(2));
      break;
  }
}

String SuplaWebPageSensor::supla_webpage_1wire(int save) {
  uint8_t nr, suported, selected;
  uint16_t max;
  String page, key;
  page += WebServer->SuplaSaveResult(save);
  page += WebServer->SuplaJavaScript(PATH_1WIRE);
  page += F("<form method='post' action='");
  page += PATH_SAVE_1WIRE;
  page += F("'>");
#ifdef SUPLA_DHT11
  addFormHeader(page, String(S_GPIO_SETTINGS_FOR) + " DHT11");
  addNumberBox(page, INPUT_MAX_DHT11, S_QUANTITY, KEY_MAX_DHT11, ConfigESP->countFreeGpio(FUNCTION_DHT11));
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_DHT11)->getValueInt(); nr++) {
    addListGPIOBox(page, INPUT_DHT11_GPIO, "DHT11", FUNCTION_DHT11, nr);
  }
  addFormHeaderEnd(page);
#endif

#ifdef SUPLA_DHT22
  addFormHeader(page, String(S_GPIO_SETTINGS_FOR) + " DHT22");
  addNumberBox(page, INPUT_MAX_DHT22, S_QUANTITY, KEY_MAX_DHT22, ConfigESP->countFreeGpio(FUNCTION_DHT22));
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_DHT22)->getValueInt(); nr++) {
    addListGPIOBox(page, INPUT_DHT22_GPIO, "DHT22", FUNCTION_DHT22, nr);
  }
  addFormHeaderEnd(page);
#endif

#ifdef SUPLA_SI7021_SONOFF
  addFormHeader(page, String(S_GPIO_SETTINGS_FOR) + " Si7021 Sonoff");
  addListGPIOBox(page, INPUT_SI7021_SONOFF, "Si7021 Sonoff", FUNCTION_SI7021_SONOFF);
  addFormHeaderEnd(page);
#endif

#ifdef SUPLA_DS18B20
  addFormHeader(page, String(S_GPIO_SETTINGS_FOR) + " Multi DS18B20");
  addNumberBox(page, INPUT_MAX_DS18B20, S_QUANTITY, KEY_MULTI_MAX_DS18B20, MAX_DS18B20);
  if (ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt() > 1) {
    addListGPIOLinkBox(page, INPUT_MULTI_DS_GPIO, "MULTI DS18B20", FUNCTION_DS18B20, PATH_MULTI_DS);
  }
  else {
    addListGPIOBox(page, INPUT_MULTI_DS_GPIO, "MULTI DS18B20", FUNCTION_DS18B20);
  }
  addFormHeaderEnd(page);
#endif

  page += F("<button type='submit'>");
  page += S_SAVE;
  page += F("</button></form>");
  page += F("<br>");
  page += F("<form method='post' action='");
  page += PATH_REBOT;
  page += F("'>");
  page += F("<button type='submit'>");
  page += S_RESTART;
  page += F("</button></form>");
  page += F("<br>");
  page += F("<a href='");
  page += PATH_START;
  page += PATH_DEVICE_SETTINGS;
  page += F("'><button>");
  page += S_RETURN;
  page += F("</button></a></div>");
  return page;
}
#endif

#if defined(SUPLA_BME280) || defined(SUPLA_SHT30) || defined(SUPLA_SI7021)
void SuplaWebPageSensor::handlei2c() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  WebServer->sendContent(supla_webpage_i2c(0));
}

void SuplaWebPageSensor::handlei2cSave() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  String input;
  uint8_t key, nr, current_value, last_value;

#if defined(SUPLA_BME280) || defined(SUPLA_SI7021) || defined(SUPLA_SHT3x)
  if (!WebServer->saveGPIO(INPUT_SDA_GPIO, FUNCTION_SDA)) {
    WebServer->sendContent(supla_webpage_i2c(6));
    return;
  }
  if (!WebServer->saveGPIO(INPUT_SCL_GPIO, FUNCTION_SCL)) {
    WebServer->sendContent(supla_webpage_i2c(6));
    return;
  }
#endif

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
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      WebServer->sendContent(supla_webpage_i2c(1));
      break;
    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      WebServer->sendContent(supla_webpage_i2c(2));
      break;
  }
}

String SuplaWebPageSensor::supla_webpage_i2c(int save) {
  uint8_t nr, suported, selected, size;
  String page, key;
  page += WebServer->SuplaSaveResult(save);
  page += WebServer->SuplaJavaScript(PATH_I2C);

  addForm(page, F("post"), PATH_SAVE_I2C);
#if defined(SUPLA_BME280) || defined(SUPLA_SI7021) || defined(SUPLA_SHT3x) || defined(SUPLA_OLED)
  addFormHeader(page, String(S_GPIO_SETTINGS_FOR) + " i2c");
  addListGPIOBox(page, INPUT_SDA_GPIO, "SDA", FUNCTION_SDA);
  addListGPIOBox(page, INPUT_SCL_GPIO, "SCL", FUNCTION_SCL);
  addFormHeaderEnd(page);

  if (ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO) {
#ifdef SUPLA_BME280
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_BME280).toInt();
    addFormHeader(page);
    addListBox(page, INPUT_BME280, "BME280 adres", BME280_P, 4, selected);
    addNumberBox(page, INPUT_ALTITUDE_BME280, S_ALTITUDE_ABOVE_SEA_LEVEL, KEY_ALTITUDE_BME280, 1500);
    addFormHeaderEnd(page);
#endif

#ifdef SUPLA_SHT3x
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_SHT3x).toInt();
    addFormHeader(page);
    addListBox(page, INPUT_SHT3x, "SHT3x", SHT3x_P, 4, selected);
    addFormHeaderEnd(page);
#endif

#ifdef SUPLA_SI7021
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_SI7021).toInt();
    addFormHeader(page);
    addListBox(page, INPUT_SI7021, "SI7021", STATE_P, 2, selected);
    addFormHeaderEnd(page);
#endif

#ifdef SUPLA_OLED
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_OLED).toInt();
    addFormHeader(page);
    addListBox(page, INPUT_OLED, "OLED", OLED_P, 2, selected);
    addFormHeaderEnd(page);
#endif
  }
#endif

  addButtonSubmit(page, S_SAVE);
  addFormEnd(page);

  addButton(page, S_RETURN, PATH_DEVICE_SETTINGS);
  addButton(page, S_RESTART, PATH_REBOT);

  return page;
}
#endif

#if defined(SUPLA_MAX6675)
void SuplaWebPageSensor::handleSpi() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  WebServer->sendContent(supla_webpage_spi(0));
}

void SuplaWebPageSensor::handleSpiSave() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  String key, input;
  uint8_t nr, current_value, last_value;

#if defined(SUPLA_MAX6675)
  if (!WebServer->saveGPIO(INPUT_CLK_GPIO, FUNCTION_CLK)) {
    WebServer->sendContent(supla_webpage_spi(6));
    return;
  }
  if (!WebServer->saveGPIO(INPUT_CS_GPIO, FUNCTION_CS)) {
    WebServer->sendContent(supla_webpage_spi(6));
    return;
  }
  if (!WebServer->saveGPIO(INPUT_D0_GPIO, FUNCTION_D0)) {
    WebServer->sendContent(supla_webpage_spi(6));
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
      WebServer->sendContent(supla_webpage_spi(1));
      break;
    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      WebServer->sendContent(supla_webpage_spi(2));
      break;
  }
}

String SuplaWebPageSensor::supla_webpage_spi(int save) {
  uint8_t nr, suported, selected;
  String page, key;
  page += WebServer->SuplaSaveResult(save);
  page += WebServer->SuplaJavaScript(PATH_SPI);
  page += F("<form method='post' action='");
  page += PATH_SAVE_SPI;
  page += F("'>");

#if defined(SUPLA_MAX6675)
  addFormHeader(page, String(S_GPIO_SETTINGS_FOR) + " SPI");
  addListGPIOBox(page, INPUT_CLK_GPIO, "CLK", FUNCTION_CLK);
  addListGPIOBox(page, INPUT_CS_GPIO, "CS", FUNCTION_CS);
  addListGPIOBox(page, INPUT_D0_GPIO, "D0", FUNCTION_D0);

  if (ConfigESP->getGpio(FUNCTION_CLK) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_CS) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_D0) != OFF_GPIO) {
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_MAX6675).toInt();
    addListBox(page, INPUT_MAX6675, "MAX6675", STATE_P, 2, selected);
  }
  addFormHeaderEnd(page);
#endif
  page += F("<button type='submit'>");
  page += S_SAVE;
  page += F("</button></form>");
  page += F("<br>");
  page += F("<form method='post' action='");
  page += PATH_REBOT;
  page += F("'>");
  page += F("<button type='submit'>");
  page += S_RESTART;
  page += F("</button></form>");
  page += F("<br>");
  page += F("<a href='");
  page += PATH_START;
  page += PATH_DEVICE_SETTINGS;
  page += F("'><button>");
  page += S_RETURN;
  page += F("</button></a></div>");
  return page;
}
#endif

#if defined(SUPLA_HC_SR04) || defined(SUPLA_IMPULSE_COUNTER)
void SuplaWebPageSensor::handleOther() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  WebServer->sendContent(supla_webpage_other(0));
}

void SuplaWebPageSensor::handleOtherSave() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }

  String key, input;
  uint8_t nr, current_value, last_value;

#ifdef SUPLA_HC_SR04
  if (!WebServer->saveGPIO(INPUT_TRIG_GPIO, FUNCTION_TRIG)) {
    WebServer->sendContent(supla_webpage_other(6));
    return;
  }
  if (!WebServer->saveGPIO(INPUT_ECHO_GPIO, FUNCTION_ECHO)) {
    WebServer->sendContent(supla_webpage_other(6));
    return;
  }
#endif

#ifdef SUPLA_IMPULSE_COUNTER
  // Supla::GUI::impulseCounter[0]->setCounter((unsigned long long)WebServer->httpServer.arg(INPUT_IMPULSE_COUNTER_CHANGE_VALUE).toInt());

  last_value = ConfigManager->get(KEY_MAX_IMPULSE_COUNTER)->getValueInt();
  for (nr = 1; nr <= last_value; nr++) {
    if (!WebServer->saveGPIO(INPUT_IMPULSE_COUNTER_GPIO, FUNCTION_IMPULSE_COUNTER, nr, INPUT_MAX_IMPULSE_COUNTER)) {
      WebServer->sendContent(supla_webpage_other(6));
      return;
    }
  }

  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_IMPULSE_COUNTER).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_IMPULSE_COUNTER, WebServer->httpServer.arg(INPUT_MAX_IMPULSE_COUNTER).c_str());
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      WebServer->sendContent(supla_webpage_other(1));
      break;
    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      WebServer->sendContent(supla_webpage_other(2));
      break;
  }
}

String SuplaWebPageSensor::supla_webpage_other(int save) {
  uint8_t nr, suported, selected;
  String page, key;
  page += WebServer->SuplaSaveResult(save);
  page += WebServer->SuplaJavaScript(PATH_OTHER);
  page += F("<form method='post' action='");
  page += PATH_SAVE_OTHER;
  page += F("'>");

#ifdef SUPLA_HC_SR04
  addFormHeader(page, String(S_GPIO_SETTINGS_FOR) + " HC-SR04");
  addListGPIOBox(page, INPUT_TRIG_GPIO, "TRIG", FUNCTION_TRIG);
  addListGPIOBox(page, INPUT_ECHO_GPIO, "ECHO", FUNCTION_ECHO);
  addFormHeaderEnd(page);
#endif

#ifdef SUPLA_IMPULSE_COUNTER
  addFormHeader(page, String(S_GPIO_SETTINGS_FOR) + " " + S_IMPULSE_COUNTER);
  addNumberBox(page, INPUT_MAX_IMPULSE_COUNTER, S_QUANTITY, KEY_MAX_IMPULSE_COUNTER, ConfigESP->countFreeGpio(FUNCTION_IMPULSE_COUNTER));
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_IMPULSE_COUNTER)->getValueInt(); nr++) {
    addListGPIOLinkBox(page, INPUT_IMPULSE_COUNTER_GPIO, "IC GPIO", FUNCTION_IMPULSE_COUNTER, PATH_IMPULSE_COUNTER_SET, nr);
  }
  addFormHeaderEnd(page);
#endif
  page += F("<button type='submit'>");
  page += S_SAVE;
  page += F("</button></form>");
  page += F("<br>");
  page += F("<form method='post' action='");
  page += PATH_REBOT;
  page += F("'>");
  page += F("<button type='submit'>");
  page += S_RESTART;
  page += F("</button></form>");
  page += F("<br>");
  page += F("<a href='");
  page += PATH_START;
  page += PATH_DEVICE_SETTINGS;
  page += F("'><button>");
  page += S_RETURN;
  page += F("</button></a></div>");
  return page;
}
#endif

#ifdef SUPLA_IMPULSE_COUNTER
void SuplaWebPageSensor::handleImpulseCounterSet() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  WebServer->sendContent(supla_impulse_counter_set(0));
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
      WebServer->sendContent(supla_webpage_other(1));
      break;

    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      WebServer->sendContent(supla_webpage_other(2));
      break;
  }
}

String SuplaWebPageSensor::supla_impulse_counter_set(int save) {
  String readUrl, nr, key;
  uint8_t place, selected, suported;

  String path = PATH_START;
  path += PATH_IMPULSE_COUNTER_SET;
  readUrl = WebServer->httpServer.uri();

  place = readUrl.indexOf(path);
  nr = readUrl.substring(place + path.length(), place + path.length() + 3);

  String page = "";
  page += WebServer->SuplaSaveResult(save);
  page += WebServer->SuplaJavaScript(PATH_OTHER);
  uint8_t relays = ConfigManager->get(KEY_MAX_IMPULSE_COUNTER)->getValueInt();
  if (nr.toInt() <= relays && ConfigESP->getGpio(nr.toInt(), FUNCTION_IMPULSE_COUNTER) != OFF_GPIO) {
    page += F("<form method='post' action='");
    page += PATH_SAVE_IMPULSE_COUNTER_SET;
    page += nr;
    page += F("'><div class='w'><h3>");
    page += S_IMPULSE_COUNTER_SETTINGS_NR;
    page += F(" ");
    page += nr;
    page += F("</h3>");
    page += F("<i><label>");
    page += S_IMPULSE_COUNTER_PULL_UP;
    page += F("</label><select name='");
    page += INPUT_IMPULSE_COUNTER_PULL_UP;
    page += nr;
    page += F("'>");
    selected = ConfigESP->getMemory(nr.toInt(), FUNCTION_IMPULSE_COUNTER);
    for (suported = 0; suported < 2; suported++) {
      page += F("<option value='");
      page += suported;
      if (selected == suported) {
        page += F("' selected>");
      }
      else
        page += F("'>");
      page += StateString(suported);
    }
    page += F("</select></i>");
    page += F("<i><label>");
    page += S_IMPULSE_COUNTER_RAISING_EDGE;
    page += F("</label><select name='");
    page += INPUT_IMPULSE_COUNTER_RAISING_EDGE;
    page += nr;
    page += F("'>");
    selected = ConfigESP->getLevel(nr.toInt(), FUNCTION_IMPULSE_COUNTER);
    for (suported = 0; suported < 2; suported++) {
      page += F("<option value='");
      page += suported;
      if (selected == suported) {
        page += F("' selected>");
      }
      else
        page += F("'>");
      page += StateString(suported);
    }
    page += F("</select></i>");
    addNumberBox(page, INPUT_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT, S_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT, KEY_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT, 99999999);
    page += F("<i><label>");
    page += S_IMPULSE_COUNTER_CHANGE_VALUE;
    page += F("</label><input name='");
    page += INPUT_IMPULSE_COUNTER_CHANGE_VALUE;
    page += F("' type='number' placeholder='0' step='1' min='0' max='");
    page += 100;
    page += F("' value='");
    uint32_t count = Supla::GUI::impulseCounter[nr.toInt() - 1]->getCounter();
    page += count;
    page += F("'></i>");

    page += F("</div><button type='submit'>");
    page += S_SAVE;
    page += F("</button></form>");
  }
  else {
    page += F("<div class='w'><h3>");
    page += S_NO_IMPULSE_COUNTER_NR;
    page += F(" ");
    page += nr;
    page += F("</h3>");
  }
  page += F("<br>");
  page += F("<a href='");
  page += PATH_START;
  page += PATH_OTHER;
  page += F("'><button>");
  page += S_RETURN;
  page += F("</button></a></div>");

  return page;
}
#endif