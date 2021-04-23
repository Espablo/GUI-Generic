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

#ifdef SUPLA_PZEM_V_3
#include <supla/sensor/PzemV3.h>
#include <supla/sensor/three_phase_PzemV3.h>
#endif

#define DRD_TIMEOUT 5  // Number of seconds after reset during which a subseqent reset will be considered a double reset.
#define DRD_ADDRESS 0  // RTC Memory Address for the DoubleResetDetector to use
DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

void setup() {
  Serial.begin(74880);
  uint8_t nr, gpio;

  ESP.wdtDisable();

  ConfigManager = new SuplaConfigManager();
  ConfigESP = new SuplaConfigESP();

  if (drd.detectDoubleReset()) {
    drd.stop();
    ConfigESP->factoryReset();
  }

#ifdef SUPLA_MCP23017
  if (ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO &&
      ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt()) {
    Wire.begin(ConfigESP->getGpio(FUNCTION_SDA), ConfigESP->getGpio(FUNCTION_SCL));
    Supla::Control::MCP_23017 *mcp = new Supla::Control::MCP_23017();

    for (nr = 1; nr <= ConfigManager->get(KEY_MAX_BUTTON)->getValueInt(); nr++) {
      gpio = ConfigESP->getGpio(nr, FUNCTION_BUTTON);
      mcp->setPullup(gpio, ConfigESP->getPullUp(gpio), ConfigESP->getInversed(gpio));
    }
  }
#endif

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
  uint8_t rollershutters = ConfigManager->get(KEY_MAX_ROLLERSHUTTER)->getValueInt();

  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_RELAY)->getValueInt(); nr++) {
    if (ConfigESP->getGpio(nr, FUNCTION_RELAY) != OFF_GPIO) {
      if (rollershutters > 0) {
#ifdef SUPLA_ROLLERSHUTTER
        Supla::GUI::addRolleShutter(nr);
#endif
        rollershutters--;
        nr++;
      }
      else {
#ifdef SUPLA_RELAY
        Supla::GUI::addRelayButton(nr);
#endif
      }
    }
  }
#endif

#ifdef SUPLA_LIMIT_SWITCH
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt(); nr++) {
    if (ConfigESP->getGpio(nr, FUNCTION_LIMIT_SWITCH) != OFF_GPIO) {
      new Supla::Sensor::Binary(ConfigESP->getGpio(nr, FUNCTION_LIMIT_SWITCH), true);
    }
  }
#endif

#ifdef SUPLA_CONFIG
  Supla::GUI::addConfigESP(ConfigESP->getGpio(FUNCTION_CFG_BUTTON), ConfigESP->getGpio(FUNCTION_CFG_LED),
                           ConfigManager->get(KEY_CFG_MODE)->getValueInt(), ConfigESP->getLevel(ConfigESP->getGpio(FUNCTION_CFG_LED)));
#endif

#ifdef SUPLA_DS18B20
  if (ConfigESP->getGpio(FUNCTION_DS18B20) != OFF_GPIO) {
    Supla::GUI::addDS18B20MultiThermometer(ConfigESP->getGpio(FUNCTION_DS18B20));
  }
#endif

#ifdef SUPLA_DHT11
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_DHT11)->getValueInt(); nr++) {
    if (ConfigESP->getGpio(nr, FUNCTION_DHT11) != OFF_GPIO) {
      auto dht11 = new Supla::Sensor::DHT(ConfigESP->getGpio(nr, FUNCTION_DHT11), DHT11);

      if (nr == 1) {
        Supla::GUI::addConditionsTurnON(SENSOR_DHT11, dht11);
        Supla::GUI::addConditionsTurnOFF(SENSOR_DHT11, dht11);
      }
    }
  }
#endif

#ifdef SUPLA_DHT22
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_DHT22)->getValueInt(); nr++) {
    if (ConfigESP->getGpio(nr, FUNCTION_DHT22) != OFF_GPIO) {
      auto dht22 = new Supla::Sensor::DHT(ConfigESP->getGpio(nr, FUNCTION_DHT22), DHT22);

      if (nr == 1) {
        Supla::GUI::addConditionsTurnON(SENSOR_DHT22, dht22);
        Supla::GUI::addConditionsTurnOFF(SENSOR_DHT22, dht22);
      }
    }
  }
#endif

#ifdef SUPLA_SI7021_SONOFF
  if (ConfigESP->getGpio(FUNCTION_SI7021_SONOFF) != OFF_GPIO) {
    auto si7021sonoff = new Supla::Sensor::Si7021Sonoff(ConfigESP->getGpio(FUNCTION_SI7021_SONOFF));
    Supla::GUI::addConditionsTurnON(SENSOR_SI7021_SONOFF, si7021sonoff);
    Supla::GUI::addConditionsTurnOFF(SENSOR_SI7021_SONOFF, si7021sonoff);
  }
#endif

#ifdef SUPLA_HC_SR04
  if (ConfigESP->getGpio(FUNCTION_TRIG) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_ECHO) != OFF_GPIO) {
    Supla::Sensor::HC_SR04 *hcsr04;
    if (ConfigManager->get(KEY_HC_SR04_MAX_SENSOR_READ)->getValueInt() > 0) {
      hcsr04 = new Supla::Sensor::HC_SR04(ConfigESP->getGpio(FUNCTION_TRIG), ConfigESP->getGpio(FUNCTION_ECHO), 0,
                                          ConfigManager->get(KEY_HC_SR04_MAX_SENSOR_READ)->getValueInt(),
                                          ConfigManager->get(KEY_HC_SR04_MAX_SENSOR_READ)->getValueInt(), 0);
    }
    else {
      hcsr04 = new Supla::Sensor::HC_SR04(ConfigESP->getGpio(FUNCTION_TRIG), ConfigESP->getGpio(FUNCTION_ECHO));
    }

    Supla::GUI::addConditionsTurnON(SENSOR_HC_SR04, hcsr04);
    Supla::GUI::addConditionsTurnOFF(SENSOR_HC_SR04, hcsr04);
  }
#endif

#ifdef SUPLA_MAX6675
  if (ConfigESP->getGpio(FUNCTION_CLK) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_CS) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_D0) != OFF_GPIO) {
    auto thermocouple =
        new Supla::Sensor::MAXThermocouple(ConfigESP->getGpio(FUNCTION_CLK), ConfigESP->getGpio(FUNCTION_CS), ConfigESP->getGpio(FUNCTION_D0));
    Supla::GUI::addConditionsTurnON(SENSOR_MAX6675, thermocouple);
    Supla::GUI::addConditionsTurnOFF(SENSOR_MAX6675, thermocouple);
  }
#endif

#ifdef SUPLA_NTC_10K
  if (ConfigESP->getGpio(FUNCTION_NTC_10K) != OFF_GPIO) {
    auto ntc10k = new Supla::Sensor::NTC10K(A0);
    Supla::GUI::addConditionsTurnON(SENSOR_NTC_10K, ntc10k);
    Supla::GUI::addConditionsTurnOFF(SENSOR_NTC_10K, ntc10k);
  }
#endif

#ifdef SUPLA_RGBW
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_RGBW)->getValueInt(); nr++) {
    Supla::GUI::addRGBWLeds(nr);
  }
#endif

#ifdef SUPLA_IMPULSE_COUNTER
  if (ConfigManager->get(KEY_MAX_IMPULSE_COUNTER)->getValueInt() > 0) {
    for (nr = 1; nr <= ConfigManager->get(KEY_MAX_IMPULSE_COUNTER)->getValueInt(); nr++) {
      gpio = ConfigESP->getGpio(nr, FUNCTION_IMPULSE_COUNTER);
      if (gpio != OFF_GPIO) {
        Supla::GUI::addImpulseCounter(gpio, ConfigESP->getLevel(gpio), ConfigESP->getMemory(gpio),
                                      ConfigManager->get(KEY_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT)->getValueInt());
      }
    }
  }

#endif

#ifdef SUPLA_HLW8012
  if (ConfigESP->getGpio(FUNCTION_CF) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_CF1) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SEL) != OFF_GPIO) {
    Supla::GUI::addHLW8012(ConfigESP->getGpio(FUNCTION_CF), ConfigESP->getGpio(FUNCTION_CF1), ConfigESP->getGpio(FUNCTION_SEL));
  }
#endif

#ifdef SUPLA_PZEM_V_3
  int8_t pinRX1 = ConfigESP->getGpio(1, FUNCTION_PZEM_RX);
  int8_t pinTX1 = ConfigESP->getGpio(1, FUNCTION_PZEM_TX);
  int8_t pinRX2 = ConfigESP->getGpio(2, FUNCTION_PZEM_RX);
  int8_t pinTX2 = ConfigESP->getGpio(2, FUNCTION_PZEM_TX);
  int8_t pinRX3 = ConfigESP->getGpio(3, FUNCTION_PZEM_RX);
  int8_t pinTX3 = ConfigESP->getGpio(3, FUNCTION_PZEM_TX);

  if (pinRX1 != OFF_GPIO && pinTX1 != OFF_GPIO && pinRX2 != OFF_GPIO && pinTX2 != OFF_GPIO && pinRX3 != OFF_GPIO && pinTX3 != OFF_GPIO) {
    new Supla::Sensor::ThreePhasePZEMv3(pinRX1, pinTX1, pinRX2, pinTX2, pinRX3, pinTX3);
  }
  else if (pinRX1 != OFF_GPIO && pinTX1 != OFF_GPIO && pinTX2 != OFF_GPIO && pinTX3 != OFF_GPIO) {
    new Supla::Sensor::ThreePhasePZEMv3(pinRX1, pinTX1, pinRX1, pinTX2, pinRX1, pinTX3);
  }
  else if (pinRX1 != OFF_GPIO && pinTX1 != OFF_GPIO) {
    new Supla::Sensor::PZEMv3(pinRX1, pinTX1);
  }
#endif

#ifdef SUPLA_CSE7766
  if (ConfigESP->getGpio(FUNCTION_CSE7766_RX) != OFF_GPIO) {
    new Supla::Sensor::CSE_7766(ConfigESP->getGpio(FUNCTION_CSE7766_RX));
  }
#endif

#ifdef GUI_SENSOR_I2C
  if (ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO) {
    Wire.begin(ConfigESP->getGpio(FUNCTION_SDA), ConfigESP->getGpio(FUNCTION_SCL));

#ifdef SUPLA_BME280
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_BME280).toInt()) {
      Supla::Sensor::BME280 *bme280;
      switch (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_BME280).toInt()) {
        case BMx280_ADDRESS_0X76:
          bme280 = new Supla::Sensor::BME280(0x76, ConfigManager->get(KEY_ALTITUDE_BMX280)->getValueInt());
          break;
        case BMx280_ADDRESS_0X77:
          bme280 = new Supla::Sensor::BME280(0x77, ConfigManager->get(KEY_ALTITUDE_BMX280)->getValueInt());
          break;
        case BMx280_ADDRESS_0X76_AND_0X77:
          bme280 = new Supla::Sensor::BME280(0x76, ConfigManager->get(KEY_ALTITUDE_BMX280)->getValueInt());
          new Supla::Sensor::BME280(0x77, ConfigManager->get(KEY_ALTITUDE_BMX280)->getValueInt());
          break;
      }
      Supla::GUI::addConditionsTurnON(SENSOR_BME280, bme280);
      Supla::GUI::addConditionsTurnOFF(SENSOR_BME280, bme280);
    }
#endif

#ifdef SUPLA_BMP280
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_BMP280).toInt()) {
      Supla::Sensor::BMP280 *bmp280;
      switch (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_BMP280).toInt()) {
        case BMx280_ADDRESS_0X76:
          bmp280 = new Supla::Sensor::BMP280(0x76, ConfigManager->get(KEY_ALTITUDE_BMX280)->getValueInt());
          break;
        case BMx280_ADDRESS_0X77:
          bmp280 = new Supla::Sensor::BMP280(0x77, ConfigManager->get(KEY_ALTITUDE_BMX280)->getValueInt());
          break;
        case BMx280_ADDRESS_0X76_AND_0X77:
          bmp280 = new Supla::Sensor::BMP280(0x76, ConfigManager->get(KEY_ALTITUDE_BMX280)->getValueInt());
          new Supla::Sensor::BMP280(0x77, ConfigManager->get(KEY_ALTITUDE_BMX280)->getValueInt());
          break;
      }
      Supla::GUI::addConditionsTurnON(SENSOR_BMP280, bmp280);
      Supla::GUI::addConditionsTurnOFF(SENSOR_BMP280, bmp280);
    }
#endif

#ifdef SUPLA_SHT3x

    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_SHT3x).toInt()) {
      Supla::Sensor::SHT3x *sht3x;
      switch (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_SHT3x).toInt()) {
        case SHT3x_ADDRESS_0X44:
          sht3x = new Supla::Sensor::SHT3x(0x44);
          break;
        case SHT3x_ADDRESS_0X45:
          sht3x = new Supla::Sensor::SHT3x(0x45);
          break;
        case SHT3x_ADDRESS_0X44_AND_0X45:
          sht3x = new Supla::Sensor::SHT3x(0x44);
          new Supla::Sensor::SHT3x(0x45);
          break;
      }
      Supla::GUI::addConditionsTurnON(SENSOR_SHT3x, sht3x);
      Supla::GUI::addConditionsTurnOFF(SENSOR_SHT3x, sht3x);
    }
#endif

#ifdef SUPLA_SI7021
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_SI7021).toInt()) {
      auto si7021 = new Supla::Sensor::Si7021();
      Supla::GUI::addConditionsTurnON(SENSOR_SI7021, si7021);
      Supla::GUI::addConditionsTurnOFF(SENSOR_SI7021, si7021);
    }
#endif

#ifdef SUPLA_OLED
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_OLED).toInt()) {
      SuplaOled *oled = new SuplaOled();
      oled->addButtonOled(ConfigESP->getGpio(FUNCTION_CFG_BUTTON));
    }
#endif
  }
#endif

#ifdef DEBUG_MODE
  new Supla::Sensor::EspFreeHeap();
#endif

  Supla::GUI::begin();

  Supla::GUI::addCorrectionSensor();

  ESP.wdtEnable(100);
}

void loop() {
  SuplaDevice.iterate();
  delay(25);
  drd.loop();
}
