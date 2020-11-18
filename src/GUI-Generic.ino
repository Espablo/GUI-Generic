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
#ifdef SUPLA_SI7021_SONOFF
#include <supla/sensor/Si7021_sonoff.h>
#endif
#ifdef SUPLA_MAX6675
#include <supla/sensor/MAX6675_K.h>
#endif

#include "SuplaDeviceGUI.h"
#include "SuplaWebServer.h"

#define DRD_TIMEOUT 5  // Number of seconds after reset during which a subseqent reset will be considered a double reset.
#define DRD_ADDRESS 0  // RTC Memory Address for the DoubleResetDetector to use
DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

void setup() {
  Serial.begin(74880);

  if (drd.detectDoubleReset()) {
    drd.stop();
    ConfigESP->factoryReset();
  }

  uint8_t nr, gpio;
  String key;

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
  uint8_t rollershutters = ConfigManager->get(KEY_MAX_ROLLERSHUTTER)->getValueInt();

  if (ConfigESP->getGpio(FUNCTION_RELAY) != OFF_GPIO && ConfigManager->get(KEY_MAX_RELAY)->getValueInt() > 0) {
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
  if (ConfigESP->getGpio(FUNCTION_LIMIT_SWITCH) != OFF_GPIO && ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt() > 0) {
    for (nr = 1; nr <= ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt(); nr++) {
      new Supla::Sensor::Binary(ConfigESP->getGpio(nr, FUNCTION_LIMIT_SWITCH), true);
    }
  }
#endif

#ifdef SUPLA_CONFIG
  Supla::GUI::addConfigESP(ConfigESP->getGpio(FUNCTION_CFG_BUTTON), ConfigESP->getGpio(FUNCTION_CFG_LED),
                           ConfigManager->get(KEY_CFG_MODE)->getValueInt(), ConfigESP->getLevel(FUNCTION_CFG_LED));
#endif

#ifdef SUPLA_DHT11
  if (ConfigESP->getGpio(FUNCTION_DHT11) != OFF_GPIO && ConfigManager->get(KEY_MAX_DHT11)->getValueInt() > 0) {
    for (nr = 1; nr <= ConfigManager->get(KEY_MAX_DHT11)->getValueInt(); nr++) {
      new Supla::Sensor::DHT(ConfigESP->getGpio(nr, FUNCTION_DHT11), DHT11);
    }
  }
#endif

#ifdef SUPLA_DHT22
  if (ConfigESP->getGpio(FUNCTION_DHT22) != OFF_GPIO && ConfigManager->get(KEY_MAX_DHT22)->getValueInt() > 0) {
    for (nr = 1; nr <= ConfigManager->get(KEY_MAX_DHT22)->getValueInt(); nr++) {
      new Supla::Sensor::DHT(ConfigESP->getGpio(nr, FUNCTION_DHT22), DHT22);
    }
  }
#endif

#ifdef SUPLA_DS18B20
  if (ConfigESP->getGpio(FUNCTION_DS18B20) != OFF_GPIO) {
    Supla::GUI::addDS18B20MultiThermometer(ConfigESP->getGpio(FUNCTION_DS18B20));
  }
#endif

#ifdef SUPLA_SI7021_SONOFF
  if (ConfigESP->getGpio(FUNCTION_SI7021_SONOFF) != OFF_GPIO) {
    new Supla::Sensor::Si7021Sonoff(ConfigESP->getGpio(FUNCTION_SI7021_SONOFF));
  }
#endif

#ifdef SUPLA_HC_SR04
  if (ConfigESP->getGpio(FUNCTION_TRIG) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_ECHO) != OFF_GPIO) {
    new Supla::Sensor::HC_SR04(ConfigESP->getGpio(FUNCTION_TRIG), ConfigESP->getGpio(FUNCTION_ECHO));
  }
#endif

#if defined(SUPLA_BME280) || defined(SUPLA_SI7021) || defined(SUPLA_SHT30) || defined(SUPLA_HTU21D) || defined(SUPLA_SHT71) || \
    defined(SUPLA_BH1750) || defined(SUPLA_MAX44009)
  if (ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO) {
    Wire.begin(ConfigESP->getGpio(FUNCTION_SDA), ConfigESP->getGpio(FUNCTION_SCL));

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
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_SI7021).toInt()) {
      new Supla::Sensor::Si7021();
    }
#endif
  }
#endif

#ifdef SUPLA_MAX6675
  if (ConfigESP->getGpio(FUNCTION_CLK) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_CS) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_D0) != OFF_GPIO) {
    new Supla::Sensor::MAX6675_K(ConfigESP->getGpio(FUNCTION_CLK), ConfigESP->getGpio(FUNCTION_CS), ConfigESP->getGpio(FUNCTION_D0));
  }

#endif

  Supla::GUI::begin();
}

void loop() {
  SuplaDevice.iterate();
  delay(25);
  drd.loop();
}
