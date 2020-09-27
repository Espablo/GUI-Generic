#include "SuplaWebServer.h"
#include "SuplaDeviceGUI.h"
#include "SuplaWebPageSensor.h"

SuplaWebPageSensor *WebPageSensor = new SuplaWebPageSensor();

SuplaWebPageSensor::SuplaWebPageSensor() {

}

void SuplaWebPageSensor::createWebPageSensor() {
  String path;
  path += PATH_START;
  path += PATH_SENSOR;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageSensor::handleSensor, this));
  path = PATH_START;
  path += PATH_SAVE_SENSOR;
  WebServer->httpServer.on(path, std::bind(&SuplaWebPageSensor::handleSensorSave, this));
}

void SuplaWebPageSensor::handleSensor() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  WebServer->httpServer.send(200, "text/html", supla_webpage_sensor(0));
}

void SuplaWebPageSensor::handleSensorSave() {
  Serial.println(F("HTTP_POST - metoda handleSensorSave"));

  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!WebServer->httpServer.authenticate(WebServer->www_username, WebServer->www_password))
      return WebServer->httpServer.requestAuthentication();
  }
  String key, input, key_max, input_max;
  int nr, set_input,get_input, current_value, last_value;

  last_value = ConfigManager->get(KEY_MAX_DHT11)->getValueInt();
  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_DHT11).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_DHT11, WebServer->httpServer.arg(INPUT_MAX_DHT11).c_str());
  }
  current_value = ConfigManager->get(KEY_MAX_DHT11)->getValueInt();
  if(current_value >= last_value){
    for(nr = 1; nr <= last_value; nr++){
      key = KEY_DHT11;
      input = INPUT_DHT11_GPIO;
      key += nr;
      input += nr;
      set_input = WebServer->httpServer.arg(input).toInt();
      get_input = ConfigManager->get(key.c_str())->getValueInt();
      if(get_input != set_input){
        if(WebServer->getBusyGpio(set_input) == false){
          ConfigManager->set(key.c_str(), WebServer->httpServer.arg(input).c_str());
          WebServer->setBusyGpio(get_input, false);
          WebServer->setBusyGpio(set_input, true);
        }
        else {
          Serial.println(F("ERROR!!!"));
          WebServer->httpServer.send(200, "text/html", supla_webpage_sensor(6));
          return;
        }
      } 
    }
  }
  else if(current_value < last_value){
    for(nr = current_value + 1; nr <= last_value; nr++){
      key = KEY_DHT11;
      key += nr;
      get_input = ConfigManager->get(key.c_str())->getValueInt();
      ConfigManager->set(key.c_str(), "17");
      WebServer->setBusyGpio(get_input, false);
    }
  }
  
  last_value = ConfigManager->get(KEY_MAX_DHT22)->getValueInt();
  if (strcmp(WebServer->httpServer.arg(INPUT_MAX_DHT22).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_DHT22, WebServer->httpServer.arg(INPUT_MAX_DHT22).c_str());
  }
  current_value = ConfigManager->get(KEY_MAX_DHT22)->getValueInt();
  if(current_value >= last_value){
    for(nr = 1; nr <= last_value; nr++){
      key = KEY_DHT22;
      input = INPUT_DHT22_GPIO;
      key += nr;
      input += nr;
      set_input = WebServer->httpServer.arg(input).toInt();
      get_input = ConfigManager->get(key.c_str())->getValueInt();
      if(get_input != set_input){
        if(WebServer->getBusyGpio(set_input) == false){
          ConfigManager->set(key.c_str(), WebServer->httpServer.arg(input).c_str());
          WebServer->setBusyGpio(get_input, false);
          WebServer->setBusyGpio(set_input, true);
        }
        else {
          Serial.println(F("ERROR!!!"));
          WebServer->httpServer.send(200, "text/html", supla_webpage_sensor(6));
          return;
        }
      } 
    }
  }
  else if(current_value < last_value){
    for(nr = current_value + 1; nr <= last_value; nr++){
      key = KEY_DHT22;
      key += nr;
      get_input = ConfigManager->get(key.c_str())->getValueInt();
      ConfigManager->set(key.c_str(), "17");
      WebServer->setBusyGpio(get_input, false);
    }
  }

  ConfigManager->set(KEY_MULTI_DS, WebServer->httpServer.arg(INPUT_MULTI_DS_GPIO).c_str());
  
  last_value = ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt();
  if (strcmp(WebServer->httpServer.arg("maxds").c_str(), "") != 0) {
    ConfigManager->set(KEY_MULTI_MAX_DS18B20, WebServer->httpServer.arg("maxds").c_str());    
  }
  if(ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt() >= last_value){
    int gpio = ConfigManager->get(KEY_MULTI_DS)->getValueInt();
    if(gpio >= 0 && gpio < OFF_GPIO){
      WebServer->setBusyGpio(gpio, true);
      Supla::GUI::addDS18B20MultiThermometer(gpio);  
    }
    else WebServer->setBusyGpio(gpio, false);
  }

    key = KEY_SDA;
    input = INPUT_SDA_GPIO;
    set_input = WebServer->httpServer.arg(input).toInt();
    get_input = ConfigManager->get(key.c_str())->getValueInt();
    if(get_input != set_input){
      if(WebServer->getBusyGpio(set_input) == false){
        ConfigManager->set(key.c_str(), WebServer->httpServer.arg(input).c_str());
        WebServer->setBusyGpio(get_input, false);
        WebServer->setBusyGpio(set_input, true);
      }
      else {
        Serial.println(F("ERROR!!!"));
        WebServer->httpServer.send(200, "text/html", supla_webpage_sensor(6));
        return;
      }
    } 
    
    key = KEY_SCL;
    input = INPUT_SCL_GPIO;
    set_input = WebServer->httpServer.arg(input).toInt();
    get_input = ConfigManager->get(key.c_str())->getValueInt();
    if(get_input != set_input){
      if(WebServer->getBusyGpio(set_input) == false){
        ConfigManager->set(key.c_str(), WebServer->httpServer.arg(input).c_str());
        WebServer->setBusyGpio(get_input, false);
        WebServer->setBusyGpio(set_input, true);
      }
      else {
        Serial.println(F("ERROR!!!"));
        WebServer->httpServer.send(200, "text/html", supla_webpage_sensor(6));
        return;
      }
    } 
    
    key = KEY_BME280;
    input = INPUT_BME280;
    ConfigManager->set(key.c_str(), WebServer->httpServer.arg(input).c_str());

    key = KEY_ALTITUDE_BME280;
    input = INPUT_ALTITUDE_BME280;
    ConfigManager->set(key.c_str(), WebServer->httpServer.arg(input).c_str());

    key = KEY_TRIG;
    input = INPUT_TRIG_GPIO;
    set_input = WebServer->httpServer.arg(input).toInt();
    get_input = ConfigManager->get(key.c_str())->getValueInt();
    if(get_input != set_input){
      if(WebServer->getBusyGpio(set_input) == false){
        ConfigManager->set(key.c_str(), WebServer->httpServer.arg(input).c_str());
        WebServer->setBusyGpio(get_input, false);
        WebServer->setBusyGpio(set_input, true);
      }
      else {
        Serial.println(F("ERROR!!!"));
        WebServer->httpServer.send(200, "text/html", supla_webpage_sensor(6));
        return;
      }
    } 

    key = KEY_ECHO;
    input = INPUT_ECHO_GPIO;
    set_input = WebServer->httpServer.arg(input).toInt();
    get_input = ConfigManager->get(key.c_str())->getValueInt();
    if(get_input != set_input){
      if(WebServer->getBusyGpio(set_input) == false){
        ConfigManager->set(key.c_str(), WebServer->httpServer.arg(input).c_str());
        WebServer->setBusyGpio(get_input, false);
        WebServer->setBusyGpio(set_input, true);
      }
      else {
        Serial.println(F("ERROR!!!"));
        WebServer->httpServer.send(200, "text/html", supla_webpage_sensor(6));
        return;
      }
    } 

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      Serial.println(F("E_CONFIG_OK: Config save"));
      WebServer->sendContent(supla_webpage_sensor(1));
      break;
    case E_CONFIG_FILE_OPEN:
      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      WebServer->httpServer.send(200, "text/html", supla_webpage_sensor(2));
      break;  
  }
}

String SuplaWebPageSensor::supla_webpage_sensor(int save) {
  int nr, suported, selected;
  String page,key;
  
  page += WebServer->SuplaMetas();
  page += WebServer->SuplaStyle();
  page += WebServer->SuplaSaveResult(save);
  page += F("</div>");
  page += WebServer->SuplaJavaScript(PATH_SENSOR);
  page += F("<div class='s'>");
  page += WebServer->SuplaLogo();
  page += WebServer->SuplaSummary();
  page += F("<form method='post' action='");
  page += PATH_SAVE_SENSOR;
  page += F("'><div class='w'><h3>Ustawienie GPIO dla DHT11</h3>");
  page += F("<i><label>ILOŚĆ</label><input name='");
  page += INPUT_MAX_DHT11;
  page += F("' type='number' placeholder='0' step='1' min='0' max='");
  page += MAX_GPIO - ConfigManager->get(KEY_MAX_RELAY)->getValueInt()- ConfigManager->get(KEY_MAX_BUTTON)->getValueInt()- ConfigManager->get(KEY_MAX_DHT22)->getValueInt();
  page += F("' value='");
  page += String(ConfigManager->get(KEY_MAX_DHT11)->getValue());
  page += F("'></i>");
  for(nr = 1; nr <= ConfigManager->get(KEY_MAX_DHT11)->getValueInt(); nr++){
    page += F("<i><label>");
    page += nr;
    page += F(". DHT11</label><select name='");
    page += INPUT_DHT11_GPIO;
    page += nr;
    page += F("'>");
    key = KEY_DHT11;
    key += nr;
    selected = ConfigManager->get(key.c_str())->getValueInt();
    for (suported = 0; suported < sizeof(WebServer->Supported_Gpio) / sizeof(char*); suported++) {
      if(WebServer->getBusyGpio(suported) == false || selected == suported){
        page += F("<option value='");
        page += suported;
        if (selected == suported) {
          page += F("' selected>");
        }
        else page += F("'>");
        page += (WebServer->Supported_Gpio[suported][0]);
      }
    }
    page += F("</select></i>");    
  }
  page += F("</div>");
  page += F("<div class='w'><h3>Ustawienie GPIO dla DHT22</h3>");
  page += F("<i><label>ILOŚĆ</label><input name='");
  page += INPUT_MAX_DHT22;
  page += F("' type='number' placeholder='0' step='1' min='0' max='");
  page += MAX_GPIO - ConfigManager->get(KEY_MAX_RELAY)->getValueInt()- ConfigManager->get(KEY_MAX_BUTTON)->getValueInt() - ConfigManager->get(KEY_MAX_DHT11)->getValueInt();
  page += F("' value='");
  page += String(ConfigManager->get(KEY_MAX_DHT22)->getValue());
  page += F("'></i>");
  for(nr = 1; nr <= ConfigManager->get(KEY_MAX_DHT22)->getValueInt(); nr++){
    page += F("<i><label>");
    page += nr;
    page += F(". DHT22</label><select name='");
    page += INPUT_DHT22_GPIO;
    page += nr;
    page += F("'>");
    key = KEY_DHT22;
    key += nr;
    selected = ConfigManager->get(key.c_str())->getValueInt();
    for (suported = 0; suported < sizeof(WebServer->Supported_Gpio) / sizeof(char*); suported++) {
      if(WebServer->getBusyGpio(suported) == false || selected == suported){
        page += F("<option value='");
        page += suported;
        if (selected == suported) {
          page += F("' selected>");
        }
        else page += F("'>");
        page += (WebServer->Supported_Gpio[suported][0]);
      }
    }
    page += F("</select></i>");    
  }
  page += F("</div>");

  page += F("<div class='w'><h3>Ustawienie GPIO dla Multi DS18B20</h3>");
  page += F("<i><label>");
  page += F("MULTI DS18B20</label><select name='");
  page += INPUT_MULTI_DS_GPIO;
  page += F("'>");
  key = KEY_MULTI_DS;
  selected = ConfigManager->get(key.c_str())->getValueInt();
  for (suported = 0; suported < sizeof(WebServer->Supported_Gpio) / sizeof(char*); suported++) {
    if(WebServer->getBusyGpio(suported) == false || selected == suported){
      page += F("<option value='");
      page += suported;
      if (selected == suported) {
        page += F("' selected>");
      }
      else page += F("'>");
      page += (WebServer->Supported_Gpio[suported][0]);
    }
  }
  page += F("</select></i>");
  if (ConfigManager->get(KEY_MULTI_DS)->getValueInt() < OFF_GPIO) {
    page += F("<i><label>ILOŚĆ</label><input name='maxds' type='number' placeholder='1' step='1' min='1' max='16' value='");
    page += String(ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValue());
    page += F("'></i>");
  }
  page += F("</div>");

  page += F("<div class='w'><h3>Ustawienie GPIO dla i2c</h3>");
  page += F("<i><label>");
  page += F("SDA</label><select name='");
  page += INPUT_SDA_GPIO;
  page += F("'>");
  key = KEY_SDA;
  selected = ConfigManager->get(key.c_str())->getValueInt();
  for (suported = 0; suported < sizeof(WebServer->Supported_Gpio) / sizeof(char*); suported++) {
    if(WebServer->getBusyGpio(suported) == false || selected == suported){
      page += F("<option value='");
      page += suported;
      if (selected == suported) {
        page += F("' selected>");
      }
      else page += F("'>");
      page += (WebServer->Supported_Gpio[suported][0]);
    }
  }
  page += F("</select></i>");
  page += F("<i><label>");
  page += F("SCL</label><select name='");
  page += INPUT_SCL_GPIO;
  page += F("'>");
  key = KEY_SCL;
  selected = ConfigManager->get(key.c_str())->getValueInt();
 for (suported = 0; suported < sizeof(WebServer->Supported_Gpio) / sizeof(char*); suported++) {
    if(WebServer->getBusyGpio(suported) == false || selected == suported){
      page += F("<option value='");
      page += suported;
      if (selected == suported) {
        page += F("' selected>");
      }
      else page += F("'>");
      page += (WebServer->Supported_Gpio[suported][0]);
    }
  }
  page += F("</select></i>");
  if(ConfigManager->get(KEY_SDA)->getValueInt() != OFF_GPIO && ConfigManager->get(KEY_SCL)->getValueInt() != OFF_GPIO){
    page += F("<i><label>");
    page += F("BME280 adres</label><select name='");
    page += INPUT_BME280;
    page += F("'>");
    key = KEY_BME280;
    selected = ConfigManager->get(key.c_str())->getValueInt();
    for (suported = 0; suported < sizeof(Supported_Bme280) / sizeof(char*); suported++) {
        page += F("<option value='");
        page += suported;
        if (selected == suported) {
          page += F("' selected>");
        }
        else page += F("'>");
        page += (Supported_Bme280[suported]);
    }
    page += F("</select></i>");
    if(selected){
        page += F("<i><input name='");
        page += INPUT_ALTITUDE_BME280;
        page += F("' value='");
        page += ConfigManager->get(KEY_ALTITUDE_BME280)->getValue();
        page += F("' ");
        page += F("><label>Wysokość m n.p.m.</label></i>");    
      }
    }
  page += F("</div>");  
  page += F("<div class='w'><h3>Ustawienie GPIO dla HC-SR04</h3>");
  page += F("<i><label>");
  page += F("TRIG</label><select name='");
  page += INPUT_TRIG_GPIO;
  page += F("'>");
  key = KEY_TRIG;
  selected = ConfigManager->get(key.c_str())->getValueInt();
  for (suported = 0; suported < sizeof(WebServer->Supported_Gpio) / sizeof(char*); suported++) {
    if(WebServer->getBusyGpio(suported) == false || selected == suported){
      page += F("<option value='");
      page += suported;
      if (selected == suported) {
        page += F("' selected>");
      }
      else page += F("'>");
      page += (WebServer->Supported_Gpio[suported][0]);
    }
  }
  page += F("</select></i>");
  page += F("<i><label>");
  page += F("ECHO</label><select name='");
  page += INPUT_ECHO_GPIO;
  page += F("'>");
  key = KEY_ECHO;
  selected = ConfigManager->get(key.c_str())->getValueInt();
  for (suported = 0; suported < sizeof(WebServer->Supported_Gpio) / sizeof(char*); suported++) {
    if(WebServer->getBusyGpio(suported) == false || selected == suported){
      page += F("<option value='");
      page += suported;
      if (selected == suported) {
        page += F("' selected>");
      }
      else page += F("'>");
      page += (WebServer->Supported_Gpio[suported][0]);
    }
  }
  page += F("</select></i>");
  page += F("</div>");
  
  page += F("<button type='submit'>Zapisz</button></form>");
  page += F("<br><br>");

  if (!Supla::GUI::sensorDS.empty() || ConfigManager->get(KEY_MULTI_DS)->getValueInt() < OFF_GPIO) {
    page += F("<a href='");
    page += PATH_START;
    page += PATH_SERCH;
    page += F("'><button>Szukaj DS</button></a>");
    page += F("<br><br>");
  }

  page += F("<a href='/'><button>Powrót</button></a></div>");
  return page;
}
