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

#include "SuplaDeviceGUI.h"
#include "SuplaWebServer.h"

#include <SPI.h>
#include <SuplaDevice.h>

#include <supla/control/relay.h>
#include <supla/control/button.h>
#include <supla/sensor/DS18B20.h>
#include <supla/sensor/DHT.h>
#include <supla/sensor/binary.h>
#include <supla/sensor/bme280.h>
#include <Wire.h>
#include <supla/sensor/HC_SR04.h>


void setup() {

  Serial.begin(74880);

  int busy_gpio_button[MAX_GPIO];
  for(int i=0; i <= MAX_GPIO; i++){
    busy_gpio_button[i] = OFF_GPIO;
  }
  int nr, gpio;
  String key;
  
  for(nr = 1; nr <= ConfigManager->get(KEY_MAX_BUTTON)->getValueInt(); nr++){
    key = KEY_BUTTON;
    key += nr;
    gpio = ConfigManager->get(key.c_str())->getValueInt();
    if(gpio >= 0 && gpio < OFF_GPIO){
      WebServer->setBusyGpio(gpio, true);
      busy_gpio_button[nr] = gpio;       
    }
  }
  
  for(nr = 1; nr <= ConfigManager->get(KEY_MAX_RELAY)->getValueInt(); nr++){
    key = KEY_RELAY;
    key += nr;
    gpio = ConfigManager->get(key.c_str())->getValueInt();
    if(gpio >= 0 && gpio < OFF_GPIO){
      WebServer->setBusyGpio(gpio, true);
      if (busy_gpio_button[nr] < OFF_GPIO){
         Supla::GUI::addRelayButton(gpio, busy_gpio_button[nr], ConfigManager->get(KEY_RELAY_LEVEL)->getValueInt());
      }
      else{
        new Supla::Control::Relay(gpio, ConfigManager->get(KEY_RELAY_LEVEL)->getValueInt());
      }      
    }
  }
  for(nr = 1; nr <= ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt(); nr++){
    key = KEY_LIMIT_SWITCH;
    key += nr;
    gpio = ConfigManager->get(key.c_str())->getValueInt();
    if(gpio >= 0 && gpio < OFF_GPIO){
      WebServer->setBusyGpio(gpio, true);
      new Supla::Sensor::Binary(gpio, true);
    }
  }

  for(nr = 1; nr <= ConfigManager->get(KEY_MAX_DHT11)->getValueInt(); nr++){
    key = KEY_DHT11;
    key += nr;
    gpio = ConfigManager->get(key.c_str())->getValueInt();
    if(gpio >= 0 && gpio < OFF_GPIO){
      WebServer->setBusyGpio(gpio, true);
      new Supla::Sensor::DHT(gpio, DHT11);
    }
  }
  for(nr = 1; nr <= ConfigManager->get(KEY_MAX_DHT22)->getValueInt(); nr++){
    key = KEY_DHT22;
    key += nr;
    gpio = ConfigManager->get(key.c_str())->getValueInt();
    if(gpio >= 0 && gpio < OFF_GPIO){
      WebServer->setBusyGpio(gpio, true);
      new Supla::Sensor::DHT(gpio, DHT22);
    }
  }

  gpio = ConfigManager->get(KEY_MULTI_DS)->getValueInt();
  if(gpio >= 0 && gpio < OFF_GPIO){
    WebServer->setBusyGpio(gpio, true);
    if(ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt()== 1){
      new Supla::Sensor::DS18B20(gpio);
    }
    else {
      Supla::GUI::addDS18B20MultiThermometer(gpio);  
    }    
  }
  gpio = ConfigManager->get(KEY_CFG_BTN)->getValueInt();
  if(gpio >= 0 && gpio < OFF_GPIO) WebServer->setBusyGpio(gpio, true);  
  gpio = ConfigManager->get(KEY_CFG_LED)->getValueInt();
  if(gpio >= 0 && gpio < OFF_GPIO) WebServer->setBusyGpio(gpio, true);
  Supla::GUI::addConfigESP(ConfigManager->get(KEY_CFG_BTN)->getValueInt(), ConfigManager->get(KEY_CFG_LED)->getValueInt(), CONFIG_MODE_10_ON_PRESSES, ConfigManager->get(KEY_CFG_LED_LEVEL)->getValueInt());

  int sdaPin = ConfigManager->get(KEY_SDA)->getValueInt();
  int sclPin = ConfigManager->get(KEY_SCL)->getValueInt();
  if(sdaPin != OFF_GPIO && sclPin != OFF_GPIO){
    Wire.begin(sdaPin, sclPin);

    int bme280_adress = ConfigManager->get(KEY_BME280)->getValueInt();
    if(bme280_adress != 0){
      new Supla::Sensor::BME280(bme280_adress, ConfigManager->get(KEY_ALTITUDE_BME280)->getValueInt());
    }
  }
  int trigPin = ConfigManager->get(KEY_TRIG)->getValueInt();
  int echoPin = ConfigManager->get(KEY_ECHO)->getValueInt();
  if(trigPin != OFF_GPIO && echoPin != OFF_GPIO){
     new Supla::Sensor::HC_SR04(trigPin,echoPin);
  }

  Supla::GUI::begin();
}

void loop() {
  SuplaDevice.iterate();
}
