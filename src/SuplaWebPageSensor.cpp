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

#if defined(SUPLA_BME280) || defined(SUPLA_HC_SR04) || defined(SUPLA_SHT3x) || defined(SUPLA_SI7021)
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
    String ds_key = KEY_DS;
    String ds_name_key = KEY_DS_NAME;
    ds_key += i;
    ds_name_key += i;

    String ds = F("dschlid");
    String ds_name = F("dsnameid");
    ds += i;
    ds_name += i;

    ConfigManager->set(ds_key.c_str(), WebServer->httpServer.arg(ds).c_str());
    ConfigManager->set(ds_name_key.c_str(), WebServer->httpServer.arg(ds_name).c_str());

    Supla::GUI::sensorDS[i]->setDeviceAddress(ConfigManager->get(ds_key.c_str())->getValueBin(MAX_DS18B20_ADDRESS));
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
      String ds_key = KEY_DS;
      String ds_name_key = KEY_DS_NAME;
      ds_key += i;
      ds_name_key += i;

      double temp = Supla::GUI::sensorDS[i]->getValue();
      content += F("<i style='border-bottom:none !important;'><input name='dsnameid");
      content += i;
      content += F("' value='");
      content += String(ConfigManager->get(ds_name_key.c_str())->getValue());
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
      content += String(ConfigManager->get(ds_key.c_str())->getValue());
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
  current_value = WebServer->httpServer.arg(INPUT_MAX_DHT11).toInt();

  if (last_value > 0) {
    for (nr = 1; nr <= last_value; nr++) {
      input = INPUT_DHT11_GPIO;
      input += nr;
      key = GPIO;
      key += WebServer->httpServer.arg(input).toInt();
      if (ConfigESP->getGpio(nr, FUNCTION_DHT11) != WebServer->httpServer.arg(input).toInt() ||
          WebServer->httpServer.arg(input).toInt() == OFF_GPIO || ConfigManager->get(key.c_str())->getElement(NR).toInt() > current_value) {
        ConfigESP->clearGpio(ConfigESP->getGpio(nr, FUNCTION_DHT11));
      }
      if (WebServer->httpServer.arg(input).toInt() != OFF_GPIO) {
        key = GPIO;
        key += WebServer->httpServer.arg(input).toInt();
        if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_OFF ||
            (ConfigESP->getGpio(nr, FUNCTION_DHT11) == WebServer->httpServer.arg(input).toInt() &&
             ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_DHT11)) {
          ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), nr, FUNCTION_DHT11, 0);
        }
        else {
          WebServer->sendContent(supla_webpage_1wire(6));
          return;
        }
      }
    }
  }
  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_DHT11).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_DHT11, WebServer->httpServer.arg(INPUT_MAX_DHT11).c_str());
  }
#endif

#ifdef SUPLA_DHT22
  last_value = ConfigManager->get(KEY_MAX_DHT22)->getValueInt();
  current_value = WebServer->httpServer.arg(INPUT_MAX_DHT22).toInt();

  if (last_value > 0) {
    for (nr = 1; nr <= last_value; nr++) {
      input = INPUT_DHT22_GPIO;
      input += nr;
      key = GPIO;
      key += WebServer->httpServer.arg(input).toInt();
      if (ConfigESP->getGpio(nr, FUNCTION_DHT22) != WebServer->httpServer.arg(input).toInt() ||
          WebServer->httpServer.arg(input).toInt() == OFF_GPIO || ConfigManager->get(key.c_str())->getElement(NR).toInt() > current_value) {
        ConfigESP->clearGpio(ConfigESP->getGpio(nr, FUNCTION_DHT22));
      }
      if (WebServer->httpServer.arg(input).toInt() != OFF_GPIO) {
        key = GPIO;
        key += WebServer->httpServer.arg(input).toInt();
        if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_OFF ||
            (ConfigESP->getGpio(nr, FUNCTION_DHT22) == WebServer->httpServer.arg(input).toInt() &&
             ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_DHT22)) {
          ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), nr, FUNCTION_DHT22, 0);
        }
        else {
          WebServer->sendContent(supla_webpage_1wire(6));
          return;
        }
      }
    }
  }
  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_DHT22).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_DHT22, WebServer->httpServer.arg(INPUT_MAX_DHT22).c_str());
  }
#endif

#ifdef SUPLA_DS18B20
  input = INPUT_MULTI_DS_GPIO;
  key = GPIO;
  key += WebServer->httpServer.arg(input).toInt();
  if (ConfigESP->getGpio(FUNCTION_DS18B20) != WebServer->httpServer.arg(input).toInt() || WebServer->httpServer.arg(input).toInt() == OFF_GPIO) {
    ConfigESP->clearGpio(ConfigESP->getGpio(FUNCTION_DS18B20));
  }
  if (WebServer->httpServer.arg(input).toInt() != OFF_GPIO) {
    key = GPIO;
    key += WebServer->httpServer.arg(input).toInt();
    if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_OFF ||
        (ConfigESP->getGpio(FUNCTION_DS18B20) == WebServer->httpServer.arg(input).toInt() &&
         ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_DS18B20)) {
      ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), FUNCTION_DS18B20);
    }
    else {
      WebServer->sendContent(supla_webpage_1wire(6));
      return;
    }
  }

  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_DS18B20).c_str(), "") > 0) {
    ConfigManager->set(KEY_MULTI_MAX_DS18B20, WebServer->httpServer.arg(INPUT_MAX_DS18B20).c_str());
  }
#endif

#ifdef SUPLA_SI7021_SONOFF
  input = INPUT_SI7021_SONOFF;
  key = GPIO;
  key += WebServer->httpServer.arg(input).toInt();
  if (ConfigESP->getGpio(FUNCTION_SI7021_SONOFF) != WebServer->httpServer.arg(input).toInt() ||
      WebServer->httpServer.arg(input).toInt() == OFF_GPIO) {
    ConfigESP->clearGpio(ConfigESP->getGpio(FUNCTION_SI7021_SONOFF));
  }
  if (WebServer->httpServer.arg(input).toInt() != OFF_GPIO) {
    key = GPIO;
    key += WebServer->httpServer.arg(input).toInt();
    if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_OFF ||
        (ConfigESP->getGpio(FUNCTION_SI7021_SONOFF) == WebServer->httpServer.arg(input).toInt() &&
         ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_SI7021_SONOFF)) {
      ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), FUNCTION_SI7021_SONOFF);
    }
    else {
      WebServer->sendContent(supla_webpage_1wire(6));
      return;
    }
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
  max = ConfigESP->countFreeGpio(FUNCTION_DHT11);
  addNumberBox(page, INPUT_MAX_DHT11, S_QUANTITY, KEY_MAX_DHT11, max);
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_DHT11)->getValueInt(); nr++) {
    addListGPIOBox(page, INPUT_DHT11_GPIO, "DHT11", FUNCTION_DHT11, nr);
  }
  addFormHeaderEnd(page);
#endif

#ifdef SUPLA_DHT22
  addFormHeader(page, String(S_GPIO_SETTINGS_FOR) + " DHT22");
  max = ConfigESP->countFreeGpio(FUNCTION_DHT22);
  addNumberBox(page, INPUT_MAX_DHT22, S_QUANTITY, KEY_MAX_DHT22, FUNCTION_DHT22);
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
  max = ConfigESP->countFreeGpio(FUNCTION_DS18B20);
  addNumberBox(page, INPUT_MAX_DS18B20, S_QUANTITY, KEY_MULTI_MAX_DS18B20, max);
  page += F("<i>");
  if (ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt() > 1) {
    page += F("<label>");
    if (ConfigESP->getGpio(FUNCTION_DS18B20) != OFF_GPIO) {
      page += F("<a href='");
      page += PATH_START;
      page += PATH_MULTI_DS;
      page += F("'>");
    }
    page += F("MULTI DS18B20 ");
    if (ConfigESP->getGpio(FUNCTION_DS18B20) != OFF_GPIO) {
      page += WebServer->SuplaIconEdit();
      page += F("</a>");
    }
    page += F("</label>");
  }
  page += addListGPIOSelect(INPUT_MULTI_DS_GPIO, FUNCTION_DS18B20);
  page += F("</i>");

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

#if defined(SUPLA_BME280) || defined(SUPLA_HC_SR04) || defined(SUPLA_SHT3x) || defined(SUPLA_SI7021)
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

  String key, input;
  uint8_t nr, current_value, last_value;

#if defined(SUPLA_BME280) || defined(SUPLA_SI7021) || defined(SUPLA_SHT3x)
  input = INPUT_SDA_GPIO;
  key = GPIO;
  key += WebServer->httpServer.arg(input).toInt();
  if (ConfigESP->getGpio(FUNCTION_SDA) != WebServer->httpServer.arg(input).toInt() || WebServer->httpServer.arg(input).toInt() == OFF_GPIO) {
    ConfigESP->clearGpio(ConfigESP->getGpio(FUNCTION_SDA));
  }
  if (WebServer->httpServer.arg(input).toInt() != OFF_GPIO) {
    key = GPIO;
    key += WebServer->httpServer.arg(input).toInt();
    if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_OFF ||
        (ConfigESP->getGpio(FUNCTION_SDA) == WebServer->httpServer.arg(input).toInt() &&
         ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_SDA)) {
      ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), FUNCTION_SDA);
    }
    else {
      WebServer->sendContent(supla_webpage_i2c(6));
      return;
    }
  }

  input = INPUT_SCL_GPIO;
  key = GPIO;
  key += WebServer->httpServer.arg(input).toInt();
  if (ConfigESP->getGpio(FUNCTION_SCL) != WebServer->httpServer.arg(input).toInt() || WebServer->httpServer.arg(input).toInt() == OFF_GPIO) {
    ConfigESP->clearGpio(ConfigESP->getGpio(FUNCTION_SCL));
  }
  if (WebServer->httpServer.arg(input).toInt() != OFF_GPIO) {
    key = GPIO;
    key += WebServer->httpServer.arg(input).toInt();
    if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_OFF ||
        (ConfigESP->getGpio(FUNCTION_SCL) == WebServer->httpServer.arg(input).toInt() &&
         ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_SCL)) {
      ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), FUNCTION_SCL);
    }
    else {
      WebServer->sendContent(supla_webpage_i2c(6));
      return;
    }
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
    ConfigManager->set(key.c_str(), WebServer->httpServer.arg(input).c_str());
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

#ifdef SUPLA_HC_SR04
  input = INPUT_TRIG_GPIO;
  key = GPIO;
  key += WebServer->httpServer.arg(input).toInt();
  if (ConfigESP->getGpio(FUNCTION_TRIG) != WebServer->httpServer.arg(input).toInt() || WebServer->httpServer.arg(input).toInt() == OFF_GPIO) {
    ConfigESP->clearGpio(ConfigESP->getGpio(FUNCTION_TRIG));
  }
  if (WebServer->httpServer.arg(input).toInt() != OFF_GPIO) {
    key = GPIO;
    key += WebServer->httpServer.arg(input).toInt();
    if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_OFF ||
        (ConfigESP->getGpio(FUNCTION_TRIG) == WebServer->httpServer.arg(input).toInt() &&
         ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_TRIG)) {
      ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), FUNCTION_TRIG);
    }
    else {
      WebServer->sendContent(supla_webpage_i2c(6));
      return;
    }
  }

  input = INPUT_ECHO_GPIO;
  key = GPIO;
  key += WebServer->httpServer.arg(input).toInt();
  if (ConfigESP->getGpio(FUNCTION_ECHO) != WebServer->httpServer.arg(input).toInt() || WebServer->httpServer.arg(input).toInt() == OFF_GPIO) {
    ConfigESP->clearGpio(ConfigESP->getGpio(FUNCTION_ECHO));
  }
  if (WebServer->httpServer.arg(input).toInt() != OFF_GPIO) {
    key = GPIO;
    key += WebServer->httpServer.arg(input).toInt();
    if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_OFF ||
        (ConfigESP->getGpio(FUNCTION_ECHO) == WebServer->httpServer.arg(input).toInt() &&
         ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_ECHO)) {
      ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), FUNCTION_ECHO);
    }
    else {
      WebServer->sendContent(supla_webpage_i2c(6));
      return;
    }
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
  page += F("<form method='post' action='");
  page += PATH_SAVE_I2C;
  page += F("'>");

#if defined(SUPLA_BME280) || defined(SUPLA_SI7021) || defined(SUPLA_SHT3x)
  addFormHeader(page, String(S_GPIO_SETTINGS_FOR) + " i2c");
  addListGPIOBox(page, INPUT_SDA_GPIO, "SDA", FUNCTION_SDA);
  addListGPIOBox(page, INPUT_SCL_GPIO, "SCL", FUNCTION_SCL);

  if (ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO) {
#ifdef SUPLA_BME280
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_BME280).toInt();
    size = sizeof(BME280_P) / sizeof(BME280_P[0]);
    addListBox(page, INPUT_BME280, "BME280 adres", BME280_P, size, selected);
    addNumberBox(page, INPUT_ALTITUDE_BME280, S_ALTITUDE_ABOVE_SEA_LEVEL, KEY_ALTITUDE_BME280, 1500);
#endif

#ifdef SUPLA_SHT3x
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_SHT3x).toInt();
    size = sizeof(SHT3x_P) / sizeof(SHT3x_P[0]);
    addListBox(page, INPUT_SHT3x, "SHT3x", SHT3x_P, size, selected);
#endif

#ifdef SUPLA_SI7021
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_SI7021).toInt();
     size = sizeof(STATE_P) / sizeof(STATE_P[0]);
    addListBox(page, INPUT_SI7021, "Si7021", STATE_P, size, selected);
#endif
  }
  addFormHeaderEnd(page);
#endif

#ifdef SUPLA_HC_SR04
  addFormHeader(page, String(S_GPIO_SETTINGS_FOR) + " HC-SR04");
  addListGPIOBox(page, INPUT_TRIG_GPIO, "TRIG", FUNCTION_TRIG);
  addListGPIOBox(page, INPUT_ECHO_GPIO, "ECHO", FUNCTION_ECHO);
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
  input = INPUT_CLK_GPIO;
  key = GPIO;
  key += WebServer->httpServer.arg(input).toInt();
  if (ConfigESP->getGpio(FUNCTION_CLK) != WebServer->httpServer.arg(input).toInt() || WebServer->httpServer.arg(input).toInt() == OFF_GPIO) {
    ConfigESP->clearGpio(ConfigESP->getGpio(FUNCTION_CLK));
  }
  if (WebServer->httpServer.arg(input).toInt() != OFF_GPIO) {
    key = GPIO;
    key += WebServer->httpServer.arg(input).toInt();
    if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_OFF ||
        (ConfigESP->getGpio(FUNCTION_CLK) == WebServer->httpServer.arg(input).toInt() &&
         ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_CLK)) {
      ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), FUNCTION_CLK);
    }
    else {
      WebServer->sendContent(supla_webpage_spi(6));
      return;
    }
  }

  input = INPUT_CS_GPIO;
  key = GPIO;
  key += WebServer->httpServer.arg(input).toInt();
  if (ConfigESP->getGpio(FUNCTION_CS) != WebServer->httpServer.arg(input).toInt() || WebServer->httpServer.arg(input).toInt() == OFF_GPIO) {
    ConfigESP->clearGpio(ConfigESP->getGpio(FUNCTION_CS));
  }
  if (WebServer->httpServer.arg(input).toInt() != OFF_GPIO) {
    key = GPIO;
    key += WebServer->httpServer.arg(input).toInt();
    if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_OFF ||
        (ConfigESP->getGpio(FUNCTION_CS) == WebServer->httpServer.arg(input).toInt() &&
         ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_CS)) {
      ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), FUNCTION_CS);
    }
    else {
      WebServer->sendContent(supla_webpage_spi(6));
      return;
    }
  }

  input = INPUT_D0_GPIO;
  key = GPIO;
  key += WebServer->httpServer.arg(input).toInt();
  if (ConfigESP->getGpio(FUNCTION_D0) != WebServer->httpServer.arg(input).toInt() || WebServer->httpServer.arg(input).toInt() == OFF_GPIO) {
    ConfigESP->clearGpio(ConfigESP->getGpio(FUNCTION_D0));
  }
  if (WebServer->httpServer.arg(input).toInt() != OFF_GPIO) {
    key = GPIO;
    key += WebServer->httpServer.arg(input).toInt();
    if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_OFF ||
        (ConfigESP->getGpio(FUNCTION_D0) == WebServer->httpServer.arg(input).toInt() &&
         ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_D0)) {
      ConfigESP->setGpio(WebServer->httpServer.arg(input).toInt(), FUNCTION_D0);
    }
    else {
      WebServer->sendContent(supla_webpage_spi(6));
      return;
    }
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
  page += F("<div class='w'><h3>");
  page += S_GPIO_SETTINGS_FOR;
  page += F(" SPI</h3>");
  page += F("<i><label>CLK</label>");
  page += addListGPIOSelect(INPUT_CLK_GPIO, FUNCTION_CLK);
  page += F("</i>");
  page += F("<i><label>CS</label>");
  page += addListGPIOSelect(INPUT_CS_GPIO, FUNCTION_CS);
  page += F("</i>");
  page += F("<i><label>D0</label>");
  page += addListGPIOSelect(INPUT_D0_GPIO, FUNCTION_D0);
  page += F("</i>");

  if (ConfigESP->getGpio(FUNCTION_CLK) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_CS) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_D0) != OFF_GPIO) {
#ifdef SUPLA_MAX6675
    page += F("<i><label>");
    page += F("MAX6675</label><select name='");
    page += INPUT_MAX6675;
    page += F("'>");

    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_MAX6675).toInt();
    for (suported = 0; suported < 2; suported++) {
      page += F("<option value='");
      page += suported;
      if (selected == suported) {
        page += F("' selected>");
      }
      else {
        page += F("'>");
      }
      page += StateString(suported);
    }
    page += F("</select></i>");
#endif
  }
  page += F("</div>");
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
