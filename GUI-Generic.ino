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
#include "GUI-Generic_Config.h"

#include <DoubleResetDetector.h>
#include <EEPROM.h>
#include <SPI.h>
#include <SuplaDevice.h>
#include <Wire.h>
#include <supla/control/button.h>
#include <supla/control/relay.h>
#include <supla/sensor/DHT.h>
#include <supla/sensor/DS18B20.h>
#include <supla/sensor/HC_SR04.h>
#include <supla/sensor/binary.h>
#ifdef SUPLA_BME280
#include <supla/sensor/bme280.h>
#include "SuplaWebPageSensor.h"
#endif
#ifdef SUPLA_SHT30
#include <supla/sensor/SHT3x.h>
#endif
#ifdef SUPLA_SI7021
#include <supla/sensor/Si7021.h>
#endif
#include <supla/storage/eeprom.h>

#include "FS.h"
#include "SuplaDeviceGUI.h"
#include "SuplaWebServer.h"

#define DRD_TIMEOUT 5  // Number of seconds after reset during which a subseqent reset will be considered a double reset.
#define DRD_ADDRESS 0  // RTC Memory Address for the DoubleResetDetector to use
DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

void setup() {
  Serial.begin(74880);

  if (drd.detectDoubleReset()) {
    Serial.println("FACTORY RESET!!!");
    drd.stop();
    ConfigESP->factoryReset();
  }

  uint8_t nr, gpio;
  String key;

#ifdef SUPLA_BUTTON
  ConfigESP->sort(FUNCTION_BUTTON);
#endif

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
  ConfigESP->sort(FUNCTION_RELAY);
  uint8_t rollershutters = ConfigManager->get(KEY_MAX_ROLLERSHUTTER)->getValueInt();

  if (ConfigManager->get(KEY_MAX_RELAY)->getValueInt() > 0) {
    for (nr = 1; nr <= ConfigManager->get(KEY_MAX_RELAY)->getValueInt(); nr++) {
#ifdef SUPLA_ROLLERSHUTTER
      if (rollershutters > 0) {
#ifdef SUPLA_BUTTON
        if (ConfigESP->getLevel(nr, FUNCTION_BUTTON) == Supla::ON_CHANGE && ConfigESP->getLevel(nr + 1, FUNCTION_BUTTON) == Supla::ON_CHANGE) {
          Supla::GUI::addRolleShutterMomentary(ConfigESP->getGpio(nr, FUNCTION_RELAY), ConfigESP->getGpio(nr + 1, FUNCTION_RELAY),
                                               ConfigESP->getGpio(nr, FUNCTION_BUTTON), ConfigESP->getGpio(nr + 1, FUNCTION_BUTTON),
                                               ConfigESP->getLevel(nr, FUNCTION_RELAY));
        }
        else {
#endif
          Supla::GUI::addRolleShutter(ConfigESP->getGpio(nr, FUNCTION_RELAY), ConfigESP->getGpio(nr + 1, FUNCTION_RELAY),
                                      ConfigESP->getGpio(nr, FUNCTION_BUTTON), ConfigESP->getGpio(nr + 1, FUNCTION_BUTTON),
                                      ConfigESP->getLevel(nr, FUNCTION_RELAY));
#ifdef SUPLA_BUTTON
        }
#endif
        rollershutters--;
        nr++;
      }
      else {
#endif
        Supla::GUI::addRelayButton(ConfigESP->getGpio(nr, FUNCTION_RELAY), ConfigESP->getGpio(nr, FUNCTION_BUTTON),
                                   ConfigESP->getLevel(nr, FUNCTION_RELAY));
#ifdef SUPLA_ROLLERSHUTTER
      }
#endif
    }
  }
#endif

#ifdef SUPLA_LIMIT_SWITCH
  ConfigESP->sort(FUNCTION_LIMIT_SWITCH);
  if (ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt() > 0) {
    for (nr = 1; nr <= ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt(); nr++) {
      new Supla::Sensor::Binary(ConfigESP->getGpio(nr, FUNCTION_LIMIT_SWITCH), true);
    }
  }
#endif

#ifdef SUPLA_DHT11
  ConfigESP->sort(FUNCTION_DHT11);
  if (ConfigManager->get(KEY_MAX_DHT11)->getValueInt() > 0) {
    for (nr = 1; nr <= ConfigManager->get(KEY_MAX_DHT11)->getValueInt(); nr++) {
      new Supla::Sensor::DHT(ConfigESP->getGpio(nr, FUNCTION_DHT11), DHT11);
    }
  }

#endif

#ifdef SUPLA_DHT22
  ConfigESP->sort(FUNCTION_DHT22);
  if (ConfigManager->get(KEY_MAX_DHT22)->getValueInt() > 0) {
    for (nr = 1; nr <= ConfigManager->get(KEY_MAX_DHT22)->getValueInt(); nr++) {
      new Supla::Sensor::DHT(ConfigESP->getGpio(nr, FUNCTION_DHT22), DHT22);
    }
  }
#endif

#ifdef SUPLA_DS18B20
  if (ConfigESP->sort(FUNCTION_DS18B20)) {
    if (ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt() > 0) {
      if (ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt() > 1) {
        Supla::GUI::addDS18B20MultiThermometer(ConfigESP->getGpio(1, FUNCTION_DS18B20));
      }
      else {
        new Supla::Sensor::DS18B20(ConfigESP->getGpio(1, FUNCTION_DS18B20));
      }
    }
  }
#endif

#ifdef SUPLA_CONFIG
  if (ConfigESP->sort(FUNCTION_CFG_LED) || ConfigESP->sort(FUNCTION_CFG_BUTTON)) {
    ConfigESP->sort(FUNCTION_CFG_BUTTON);
#ifdef SUPLA_BUTTON
    if (ConfigESP->getCfgFlag() != OFF_GPIO) {
      Supla::GUI::addConfigESP(ConfigESP->getCfgFlag(), ConfigESP->getGpio(1, FUNCTION_CFG_LED), CONFIG_MODE_10_ON_PRESSES,
                               ConfigESP->getLevel(1, FUNCTION_CFG_LED));
    }
    else
#endif
      Supla::GUI::addConfigESP(ConfigESP->getGpio(1, FUNCTION_CFG_BUTTON), ConfigESP->getGpio(1, FUNCTION_CFG_LED), CONFIG_MODE_10_ON_PRESSES,
                               ConfigESP->getLevel(1, FUNCTION_CFG_LED));
  }
#endif

#if defined(SUPLA_BME280) || defined(SUPLA_SHT30) || defined(SUPLA_SI7021)
  ConfigESP->sort(FUNCTION_SDA);
  ConfigESP->sort(FUNCTION_SCL);

  if (ConfigESP->sort(FUNCTION_SDA) && ConfigESP->sort(FUNCTION_SCL)) {
    Wire.begin(ConfigESP->getGpio(1, FUNCTION_SDA), ConfigESP->getGpio(1, FUNCTION_SCL));
  }
#endif

#ifdef SUPLA_BME280
  switch (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_BME280).toInt()) {
    case BME280_ADDRESS_0X76:
      new Supla::Sensor::BME280(0x76, ConfigManager->get(KEY_ALTITUDE_BME280)->getValueInt());
      break;
    case BME280_ADDRESS_0X77:
      new Supla::Sensor::BME280(0x77, ConfigManager->get(KEY_ALTITUDE_BME280)->getValueInt());
      break;
    case BME280_ADDRESS_0X76_AND_0X77:
      new Supla::Sensor::BME280(0x76, ConfigManager->get(KEY_ALTITUDE_BME280)->getValueInt());
      new Supla::Sensor::BME280(0x77, ConfigManager->get(KEY_ALTITUDE_BME280)->getValueInt());
      break;
  }
#endif

#ifdef SUPLA_SHT30
  switch (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_SHT30).toInt()) {
    case SHT30_ADDRESS_0X44:
      new Supla::Sensor::SHT3x(0x44);
      break;
    case SHT30_ADDRESS_0X45:
      new Supla::Sensor::SHT3x(0x45);
      break;
    case SHT30_ADDRESS_0X44_AND_0X45:
      new Supla::Sensor::SHT3x(0x44);
      new Supla::Sensor::SHT3x(0x45);
      break;      
  }
#endif

#ifdef SUPLA_SI7021
  new Supla::Sensor::Si7021();
#endif

#ifdef SUPLA_HC_SR04
  ConfigESP->sort(FUNCTION_TRIG);
  ConfigESP->sort(FUNCTION_ECHO);
  if (ConfigESP->sort(FUNCTION_TRIG) && ConfigESP->sort(FUNCTION_ECHO)) {
    new Supla::Sensor::HC_SR04(ConfigESP->getGpio(1, FUNCTION_TRIG), ConfigESP->getGpio(1, FUNCTION_ECHO));
  }
#endif

  Supla::GUI::begin();
}

void loop() {
  SuplaDevice.iterate();
  drd.loop();
}
