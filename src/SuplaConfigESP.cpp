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
#include "SuplaConfigESP.h"
#include "SuplaDeviceGUI.h"
#include <HardwareSerial.h>

SuplaConfigESP::SuplaConfigESP() {
  configModeESP = Supla::DEVICE_MODE_NORMAL;

  if (ConfigManager->isDeviceConfigured()) {
    if (strcmp(ConfigManager->get(KEY_SUPLA_GUID)->getValue(), "") == 0 || strcmp(ConfigManager->get(KEY_SUPLA_AUTHKEY)->getValue(), "") == 0) {
      clearEEPROM();
      ConfigManager->setGUIDandAUTHKEY();
    }

    if (strcmp(ConfigManager->get(KEY_LOGIN)->getValue(), "") == 0)
      ConfigManager->set(KEY_LOGIN, DEFAULT_LOGIN);

    if (strcmp(ConfigManager->get(KEY_LOGIN_PASS)->getValue(), "") == 0)
      ConfigManager->set(KEY_LOGIN_PASS, DEFAULT_LOGIN_PASS);

    if (strcmp(ConfigManager->get(KEY_HOST_NAME)->getValue(), "") == 0)
      ConfigManager->set(KEY_HOST_NAME, DEFAULT_HOSTNAME);

    if (strcmp(ConfigManager->get(KEY_SUPLA_SERVER)->getValue(), "") == 0)
      ConfigManager->set(KEY_SUPLA_SERVER, DEFAULT_SERVER);

    if (strcmp(ConfigManager->get(KEY_SUPLA_EMAIL)->getValue(), "") == 0)
      ConfigManager->set(KEY_SUPLA_EMAIL, DEFAULT_EMAIL);

    if (strcmp(ConfigManager->get(KEY_ENABLE_GUI)->getValue(), "") == 0)
      ConfigManager->set(KEY_ENABLE_GUI, getDefaultEnableGUI());

    if (strcmp(ConfigManager->get(KEY_ENABLE_SSL)->getValue(), "") == 0)
      ConfigManager->set(KEY_ENABLE_SSL, getDefaultEnableSSL());

#ifdef TEMPLATE_BOARD_JSON
    if (strcmp(ConfigManager->get(KEY_BOARD)->getValue(), "") == 0) {
      Supla::TanplateBoard::addTemplateBoard();
    }
#elif defined(TEMPLATE_BOARD_OLD)
    if (strcmp(ConfigManager->get(KEY_BOARD)->getValue(), "") == 0) {
      chooseTemplateBoard(getDefaultTamplateBoard());
    }
#endif

    if (ConfigESP->getGpio(FUNCTION_CFG_BUTTON) == OFF_GPIO)
      ConfigESP->setGpio(0, FUNCTION_CFG_BUTTON);

    ConfigManager->save();

    configModeInit();
  }

  SuplaDevice.setStatusFuncImpl(&status_func);
}

bool SuplaConfigESP::getDefaultEnableSSL() {
#ifdef SUPLA_ENABLE_SSL
  return true;
#else
  return false;
#endif
}

bool SuplaConfigESP::getDefaultEnableGUI() {
#ifdef SUPLA_ENABLE_GUI
  return true;
#else
  return false;
#endif
}

uint8_t SuplaConfigESP::getDefaultTamplateBoard() {
#ifdef DEFAULT_TEMPLATE_BOARD
  return DEFAULT_TEMPLATE_BOARD;
#else
  return 0;
#endif
}

void SuplaConfigESP::addConfigESP(int _pinNumberConfig, int _pinLedConfig) {
  uint8_t pinNumberConfig = _pinNumberConfig;
  uint8_t pinLedConfig = _pinLedConfig;
  uint8_t modeConfigButton = ConfigManager->get(KEY_CFG_MODE)->getValueInt();

  if (pinLedConfig != OFF_GPIO) {
    pinMode(pinLedConfig, OUTPUT);
    digitalWrite(pinLedConfig, ConfigESP->getLevel(pinLedConfig) ? LOW : HIGH);
  }

  if (pinNumberConfig != OFF_GPIO) {
    bool pullUp = true, invertLogic = true;

    for (uint8_t nr = 0; nr < ConfigManager->get(KEY_MAX_BUTTON)->getValueInt(); nr++) {
      if (ConfigESP->getGpio(nr, FUNCTION_BUTTON) == pinNumberConfig) {
        pullUp = ConfigESP->getPullUp(pinNumberConfig);
        invertLogic = ConfigESP->getInversed(pinNumberConfig);
      }
    }

    Supla::Control::Button *buttonConfig = new Supla::Control::Button(pinNumberConfig, pullUp, invertLogic);
    buttonConfig->setMulticlickTime(800);
    buttonConfig->addAction(Supla::TURN_ON, *ConfigESP, Supla::ON_CLICK_1);

    if (modeConfigButton == CONFIG_MODE_10_ON_PRESSES) {
      buttonConfig->addAction(CONFIG_MODE_10_ON_PRESSES, *ConfigESP, Supla::ON_CLICK_10);
    }
    if (modeConfigButton == CONFIG_MODE_5SEK_HOLD) {
      buttonConfig->setHoldTime(5000);
      buttonConfig->addAction(CONFIG_MODE_5SEK_HOLD, *ConfigESP, Supla::ON_HOLD);
    }
  }
}

void SuplaConfigESP::handleAction(int event, int action) {
  if (action == CONFIG_MODE_10_ON_PRESSES) {
    if (event == Supla::ON_CLICK_10) {
      configModeInit();
    }
  }
  if (action == CONFIG_MODE_5SEK_HOLD) {
    if (event == Supla::ON_HOLD) {
      configModeInit();
    }
  }

  if (configModeESP == Supla::DEVICE_MODE_CONFIG) {
    if (event == Supla::ON_CLICK_1) {
      rebootESP();
    }
  }
}

void SuplaConfigESP::rebootESP() {
  // WebServer->httpServer->send(302, "text/plain", "");
  // WebServer->sendContent();
#ifdef ARDUINO_ARCH_ESP8266
  WiFi.forceSleepBegin();
  wdt_reset();
  ESP.restart();
  while (1) wdt_reset();
#else
  ESP.restart();
#endif
}

void SuplaConfigESP::configModeInit() {
  configModeESP = Supla::DEVICE_MODE_CONFIG;
  ledBlinking(100);

  Supla::Network::SetConfigMode();

  Supla::GUI::enableConnectionSSL(false);
  Supla::GUI::setupConnection();

  if (getCountChannels() > 0) {
    SuplaDevice.enterConfigMode();
  }
}

bool SuplaConfigESP::checkSSL() {
  return ConfigManager->get(KEY_ENABLE_SSL)->getValueInt();
}

void SuplaConfigESP::iterateAlways() {
  if (configModeESP == Supla::DEVICE_MODE_CONFIG) {
#ifdef SUPLA_MDNS
    if (WiFi.status() == WL_CONNECTED) {
      if (!MDNSConfigured) {
#ifdef ARDUINO_ARCH_ESP8266
        MDNSConfigured = MDNS.begin("supla", WiFi.localIP());
#elif ARDUINO_ARCH_ESP32
        MDNSConfigured = MDNS.begin("supla");
#endif
        if (MDNSConfigured) {
          Serial.print(F("MDNS started IP: "));
          Serial.println(WiFi.localIP());
          MDNS.addService("http", "tcp", 80);
        }
      }
#ifdef ARDUINO_ARCH_ESP8266
      MDNS.update();
#endif
    }
#endif
  }
}

const String SuplaConfigESP::getConfigNameAP() {
  String name;
#ifdef ARDUINO_ARCH_ESP8266
  name = F("SUPLA-ESP8266-");
#elif ARDUINO_ARCH_ESP32
  name = F("SUPLA-ESP32-");
#endif
  return name += getMacAddress(false);
}
const char *SuplaConfigESP::getLastStatusMessageSupla() {
  return supla_status.msg;
}

int SuplaConfigESP::getLastStatusSupla() {
  return supla_status.status;
}

void SuplaConfigESP::ledBlinking(int time) {
  if (ConfigESP->getGpio(FUNCTION_CFG_LED) != OFF_GPIO)
    led.attach_ms(time, ledBlinkingTicker);
}

void SuplaConfigESP::ledBlinkingStop(void) {
  if (ConfigESP->getGpio(FUNCTION_CFG_LED) != OFF_GPIO) {
    led.detach();
    digitalWrite(ConfigESP->getGpio(FUNCTION_CFG_LED), ConfigESP->getLevel(ConfigESP->getGpio(FUNCTION_CFG_LED)) ? LOW : HIGH);
  }
}

String SuplaConfigESP::getMacAddress(bool formating) {
  byte mac[6];
  WiFi.macAddress(mac);
  char baseMacChr[18] = {0};

  if (formating)
    sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  else
    sprintf(baseMacChr, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  return String(baseMacChr);
}

void ledBlinkingTicker() {
  int val = digitalRead(ConfigESP->getGpio(FUNCTION_CFG_LED));
  digitalWrite(ConfigESP->getGpio(FUNCTION_CFG_LED), val == HIGH ? LOW : HIGH);
}

void status_func(int status, const char *msg) {
  switch (status) {
    case STATUS_ALREADY_INITIALIZED:
      ConfigESP->supla_status.msg = S_STATUS_ALREADY_INITIALIZED;
      break;
    case STATUS_INVALID_GUID:
      ConfigESP->supla_status.msg = S_STATUS_INVALID_GUID;
      break;
    case STATUS_UNKNOWN_SERVER_ADDRESS:
      ConfigESP->supla_status.msg = S_STATUS_UNKNOWN_SERVER_ADDRESS;
      break;
    case STATUS_UNKNOWN_LOCATION_ID:
      ConfigESP->supla_status.msg = S_STATUS_UNKNOWN_LOCATION_ID;
      break;
    case STATUS_INITIALIZED:
      ConfigESP->supla_status.msg = S_STATUS_INITIALIZED;
      break;
    case STATUS_CHANNEL_LIMIT_EXCEEDED:
      ConfigESP->supla_status.msg = S_STATUS_CHANNEL_LIMIT_EXCEEDED;
      break;
    case STATUS_SERVER_DISCONNECTED:
      ConfigESP->supla_status.msg = S_STATUS_SERVER_DISCONNECTED;
      break;
    case STATUS_REGISTER_IN_PROGRESS:
      ConfigESP->supla_status.msg = S_STATUS_REGISTER_IN_PROGRESS;
      break;
    case STATUS_PROTOCOL_VERSION_ERROR:
      ConfigESP->supla_status.msg = S_STATUS_PROTOCOL_VERSION_ERROR;
      break;
    case STATUS_BAD_CREDENTIALS:
      ConfigESP->supla_status.msg = S_STATUS_BAD_CREDENTIALS;
      break;
    case STATUS_TEMPORARILY_UNAVAILABLE:
      ConfigESP->supla_status.msg = S_STATUS_TEMPORARILY_UNAVAILABLE;
      break;
    case STATUS_LOCATION_CONFLICT:
      ConfigESP->supla_status.msg = S_STATUS_LOCATION_CONFLICT;
      break;
    case STATUS_CHANNEL_CONFLICT:
      ConfigESP->supla_status.msg = S_STATUS_CHANNEL_CONFLICT;
      break;
    case STATUS_REGISTERED_AND_READY:
      ConfigESP->supla_status.msg = S_STATUS_REGISTERED_AND_READY;
      break;
    case STATUS_DEVICE_IS_DISABLED:
      ConfigESP->supla_status.msg = S_STATUS_DEVICE_IS_DISABLED;
      break;
    case STATUS_LOCATION_IS_DISABLED:
      ConfigESP->supla_status.msg = S_STATUS_LOCATION_IS_DISABLED;
      break;
    case STATUS_DEVICE_LIMIT_EXCEEDED:
      ConfigESP->supla_status.msg = S_STATUS_DEVICE_LIMIT_EXCEEDED;
      break;
    case STATUS_REGISTRATION_DISABLED:
      ConfigESP->supla_status.msg = S_STATUS_REGISTRATION_DISABLED;
      break;
    case STATUS_MISSING_CREDENTIALS:
      ConfigESP->supla_status.msg = S_STATUS_MISSING_CREDENTIALS;
      break;
    case STATUS_INVALID_AUTHKEY:
      ConfigESP->supla_status.msg = S_STATUS_INVALID_AUTHKEY;
      break;
    case STATUS_NO_LOCATION_AVAILABLE:
      ConfigESP->supla_status.msg = S_STATUS_NO_LOCATION_AVAILABLE;
      break;
    case STATUS_UNKNOWN_ERROR:
      ConfigESP->supla_status.msg = S_STATUS_UNKNOWN_ERROR;
      break;
    case STATUS_NETWORK_DISCONNECTED:
      ConfigESP->supla_status.msg = S_STATUS_NETWORK_DISCONNECTED;
      break;
    case STATUS_CONFIG_MODE:
      ConfigESP->configModeESP = Supla::DEVICE_MODE_CONFIG;
      ConfigESP->ledBlinking(100);
      break;
    default:
      ConfigESP->supla_status.msg = msg;
      break;
  }
  ConfigESP->supla_status.status = status;

  if (ConfigESP->configModeESP == Supla::DEVICE_MODE_NORMAL) {
    if (status == STATUS_REGISTERED_AND_READY)
      ConfigESP->ledBlinkingStop();
    else
      ConfigESP->ledBlinking(500);
  }
}

int SuplaConfigESP::getGpio(int nr, int function) {
  for (uint8_t gpio = 0; gpio <= OFF_GPIO; gpio++) {
    uint8_t key = KEY_GPIO + gpio;

    if ((function == FUNCTION_CFG_BUTTON || function == FUNCTION_CFG_LED) && checkBusyCfg(gpio, function))
      return gpio;

#ifdef GUI_SENSOR_I2C_EXPENDER
    if (ConfigManager->get(KEY_ACTIVE_EXPENDER)->getElement(function).toInt()) {
      if (ConfigManager->get(key)->getElement(MCP23017_FUNCTION_1).toInt() == function &&
          ConfigManager->get(key)->getElement(MCP23017_NR_1).toInt() == nr) {
        return gpio;
      }

      if (ConfigManager->get(key)->getElement(MCP23017_FUNCTION_2).toInt() == function &&
          ConfigManager->get(key)->getElement(MCP23017_NR_2).toInt() == nr) {
        return gpio;
      }

      if (ConfigManager->get(key)->getElement(MCP23017_FUNCTION_3).toInt() == function &&
          ConfigManager->get(key)->getElement(MCP23017_NR_3).toInt() == nr) {
        return gpio;
      }

      if (ConfigManager->get(key)->getElement(MCP23017_FUNCTION_4).toInt() == function &&
          ConfigManager->get(key)->getElement(MCP23017_NR_4).toInt() == nr) {
        return gpio;
      }
    }
#endif

    if (ConfigManager->get(key)->getElement(FUNCTION).toInt() == function && ConfigManager->get(key)->getElement(NR).toInt() == (nr + 1)) {
      return gpio;
    }
    delay(0);
  }

  if (function == FUNCTION_RELAY && ConfigManager->get(KEY_VIRTUAL_RELAY)->getElement(nr).toInt()) {
    return GPIO_VIRTUAL_RELAY;
  }

#ifdef ARDUINO_ARCH_ESP8266
  if (function == FUNCTION_BUTTON && ConfigManager->get(KEY_ANALOG_BUTTON)->getElement(nr).toInt()) {
    return A0;
  }
#endif

  return OFF_GPIO;
}

HardwareSerial &SuplaConfigESP::getHardwareSerial(int8_t rxPin, int8_t txPin) {
#ifdef ARDUINO_ARCH_ESP32

#ifndef SOC_RX0
#if CONFIG_IDF_TARGET_ESP32
#define SOC_RX0 3
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
#define SOC_RX0 44
#elif CONFIG_IDF_TARGET_ESP32C3
#define SOC_RX0 20
#endif
#endif

#ifndef SOC_TX0
#if CONFIG_IDF_TARGET_ESP32
#define SOC_TX0 1
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
#define SOC_TX0 43
#elif CONFIG_IDF_TARGET_ESP32C3
#define SOC_TX0 21
#endif
#endif

#if SOC_UART_NUM > 1
#ifndef RX1
#if CONFIG_IDF_TARGET_ESP32
#define RX1 9
#elif CONFIG_IDF_TARGET_ESP32S2
#define RX1 18
#elif CONFIG_IDF_TARGET_ESP32C3
#define RX1 18
#elif CONFIG_IDF_TARGET_ESP32S3
#define RX1 15
#endif
#endif

#ifndef TX1
#if CONFIG_IDF_TARGET_ESP32
#define TX1 10
#elif CONFIG_IDF_TARGET_ESP32S2
#define TX1 17
#elif CONFIG_IDF_TARGET_ESP32C3
#define TX1 19
#elif CONFIG_IDF_TARGET_ESP32S3
#define TX1 16
#endif
#endif
#endif /* SOC_UART_NUM > 1 */

#if SOC_UART_NUM > 2
#ifndef RX2
#if CONFIG_IDF_TARGET_ESP32
#define RX2 16
#elif CONFIG_IDF_TARGET_ESP32S3
#define RX2 19
#endif
#endif

#ifndef TX2
#if CONFIG_IDF_TARGET_ESP32
#define TX2 17
#elif CONFIG_IDF_TARGET_ESP32S3
#define TX2 20
#endif
#endif
#endif /* SOC_UART_NUM > 2 */

#if SOC_UART_NUM > 1
  if (rxPin == RX1 || txPin == RX1) {
    return Serial1;
  }
#endif

#if SOC_UART_NUM > 2
  if (rxPin == RX2 || txPin == TX2) {
    return Serial2;
  }
#endif

#else
  // toggle between use of GPIO13/GPIO15 or GPIO3/GPIO(1/2) as RX and TX
  if (rxPin == 13 || txPin == 15) {
    Serial.swap();
    return Serial;
  }

  if (rxPin == 2 && txPin == -1) {
    return Serial1;
  }
#endif

  // ESP8266 rxPin == 3 || txPin == 1 || txPin == 2
  // ESP32 rxPin == SOC_RX0 || txPin == SOC_TX0
  return Serial;
}

uint8_t SuplaConfigESP::getBaudRate(uint8_t gpio) {
  return ConfigManager->get(getKeyGpio(gpio))->getElement(ACTION_BUTTON).toInt();
}

void SuplaConfigESP::setBaudRate(uint8_t gpio, int baudRate) {
  ConfigManager->setElement(getKeyGpio(gpio), ACTION_BUTTON, baudRate);
}

int SuplaConfigESP::getBaudRateSpeed(uint8_t gpio) {
  switch (getBaudRate(gpio)) {
    case BAUDRATE_1200:
      return 1200;
    case BAUDRATE_2400:
      return 2400;
    case BAUDRATE_4800:
      return 4800;
    case BAUDRATE_9600:
      return 9600;
    case BAUDRATE_19200:
      return 19200;
    case BAUDRATE_38400:
      return 38400;
  }
  return 9600;
}

uint8_t SuplaConfigESP::getNumberButton(uint8_t nr) {
#ifdef GUI_SENSOR_I2C_EXPENDER
  if (strcmp(ConfigManager->get(KEY_EXPANDER_NUMBER_BUTTON)->getElement(nr).c_str(), "") != 0) {
    return ConfigManager->get(KEY_EXPANDER_NUMBER_BUTTON)->getElement(nr).toInt();
  }
#else
  if (strcmp(ConfigManager->get(KEY_NUMBER_BUTTON)->getElement(nr).c_str(), "") != 0) {
    return ConfigManager->get(KEY_NUMBER_BUTTON)->getElement(nr).toInt();
  }
#endif

  return nr;
}

uint8_t SuplaConfigESP::getNumberButtonAdditional(uint8_t functionButton, uint8_t nr) {
  if (strcmp(ConfigManager->get(KEY_NUMBER_BUTTON_ADDITIONAL)->getElement(functionButton + nr).c_str(), "") != 0) {
    return ConfigManager->get(KEY_NUMBER_BUTTON_ADDITIONAL)->getElement(functionButton + nr).toInt();
  }

  return nr;
}

uint8_t SuplaConfigESP::getKeyGpio(uint8_t gpio) {
  return KEY_GPIO + gpio;
}

bool SuplaConfigESP::getLevel(uint8_t gpio) {
  if (gpio == GPIO_VIRTUAL_RELAY) {
    return false;
  }
  return ConfigManager->get(getKeyGpio(gpio))->getElement(LEVEL_RELAY).toInt();
}

bool SuplaConfigESP::getPullUp(uint8_t gpio) {
  return ConfigManager->get(getKeyGpio(gpio))->getElement(PULL_UP_BUTTON).toInt();
}

bool SuplaConfigESP::getInversed(uint8_t gpio) {
  return ConfigManager->get(getKeyGpio(gpio))->getElement(INVERSED_BUTTON).toInt();
}

uint8_t SuplaConfigESP::getMemory(uint8_t gpio, uint8_t nr) {
  if (gpio == GPIO_VIRTUAL_RELAY) {
    return ConfigManager->get(KEY_VIRTUAL_RELAY_MEMORY)->getElement(nr).toInt();
  }
  return ConfigManager->get(getKeyGpio(gpio))->getElement(MEMORY).toInt();
}

uint8_t SuplaConfigESP::getAction(uint8_t gpio) {
  uint8_t action = ConfigManager->get(getKeyGpio(gpio))->getElement(ACTION_BUTTON).toInt();

  switch (action) {
    case Supla::GUI::Action::TURN_ON:
      return Supla::Action::TURN_ON;
    case Supla::GUI::Action::TURN_OFF:
      return Supla::Action::TURN_OFF;
    case Supla::GUI::Action::TOGGLE:
      return Supla::Action::TOGGLE;
    default:
      return action;
  }
}

uint8_t SuplaConfigESP::getEvent(uint8_t gpio) {
  uint8_t event = ConfigManager->get(getKeyGpio(gpio))->getElement(EVENT_BUTTON).toInt();

  switch (event) {
    case Supla::GUI::Event::ON_PRESS:
      return Supla::Event::ON_PRESS;
    case Supla::GUI::Event::ON_RELEASE:
      return Supla::Event::ON_RELEASE;
    case Supla::GUI::Event::ON_CHANGE:
      return Supla::Event::ON_CHANGE;
    case Supla::GUI::Event::ON_HOLD:
      return Supla::Event::ON_HOLD;
    case Supla::GUI::Event::CONDITIONAL_ON_CHANGE:
      return Supla::Event::CONDITIONAL_ON_CHANGE;
    default:
      return event;
  }
}

bool SuplaConfigESP::checkBusyCfg(int gpio, int function) {
  uint8_t key = KEY_GPIO + gpio;

  if (function == FUNCTION_CFG_BUTTON) {
    if (ConfigManager->get(key)->getElement(CFG).toInt() == 1) {
      return true;
    }
  }

  if (function == FUNCTION_CFG_LED) {
    // Aby nie robić konwersji danych dla nowego typu dla LED CFG, wykorzystałem PULL_UP_BUTTON
    if (ConfigManager->get(key)->getElement(FUNCTION).toInt() == FUNCTION_CFG_LED || ConfigManager->get(key)->getElement(CFG_LED).toInt() == 1) {
      return true;
    }
  }

  return false;
}

int SuplaConfigESP::checkBusyGpio(int gpio, int function) {
  if (!checkGpio(gpio)) {
    return false;
  }
  else {
    uint8_t key = KEY_GPIO + gpio;

    if ((function == FUNCTION_CFG_BUTTON && ConfigManager->get(key)->getElement(FUNCTION).toInt() == FUNCTION_BUTTON) ||
        (function == FUNCTION_BUTTON && checkBusyCfg(gpio, FUNCTION_CFG_BUTTON) &&
         ConfigManager->get(key)->getElement(FUNCTION).toInt() != FUNCTION_BUTTON))
      return true;

    if ((function == FUNCTION_CFG_LED && ConfigManager->get(key)->getElement(FUNCTION).toInt() == FUNCTION_LED) ||
        (function == FUNCTION_LED && checkBusyCfg(gpio, FUNCTION_CFG_LED) && ConfigManager->get(key)->getElement(FUNCTION).toInt() != FUNCTION_LED))
      return true;

    if (ConfigManager->get(key)->getElement(FUNCTION).toInt() != FUNCTION_OFF || checkBusyCfg(gpio, FUNCTION_CFG_BUTTON) ||
        checkBusyCfg(gpio, FUNCTION_CFG_LED))
      return false;

    return true;
  }
}

bool SuplaConfigESP::checkBusyGpio(int gpio) {
  uint8_t key = KEY_GPIO + gpio;
  if (ConfigManager->get(key)->getElement(FUNCTION).toInt() != FUNCTION_OFF) {
    return true;
  }

  return false;
}

void SuplaConfigESP::setLevel(uint8_t gpio, int level) {
  ConfigManager->setElement(getKeyGpio(gpio), LEVEL_RELAY, level);
}
void SuplaConfigESP::setMemory(uint8_t gpio, int memory, uint8_t nr) {
  if (gpio == GPIO_VIRTUAL_RELAY) {
    ConfigManager->setElement(KEY_VIRTUAL_RELAY_MEMORY, nr, memory);
  }
  else {
    ConfigManager->setElement(getKeyGpio(gpio), MEMORY, memory);
  }
}

void SuplaConfigESP::setPullUp(uint8_t gpio, int pullup) {
  ConfigManager->setElement(getKeyGpio(gpio), PULL_UP_BUTTON, pullup);
}

void SuplaConfigESP::setInversed(uint8_t gpio, int inversed) {
  ConfigManager->setElement(getKeyGpio(gpio), INVERSED_BUTTON, inversed);
}

void SuplaConfigESP::setAction(uint8_t gpio, int action) {
  ConfigManager->setElement(getKeyGpio(gpio), ACTION_BUTTON, action);
}

void SuplaConfigESP::setEvent(uint8_t gpio, int event) {
  ConfigManager->setElement(getKeyGpio(gpio), EVENT_BUTTON, event);
}

void SuplaConfigESP::setNumberButton(uint8_t nr, uint8_t nrButton) {
#ifdef GUI_SENSOR_I2C_EXPENDER
  ConfigManager->setElement(KEY_EXPANDER_NUMBER_BUTTON, nr, nrButton);
#else
  ConfigManager->setElement(KEY_NUMBER_BUTTON, nr, nrButton);
#endif
}

void SuplaConfigESP::setGpio(uint8_t gpio, uint8_t nr, uint8_t function) {
  uint8_t key;
  nr++;
  key = KEY_GPIO + gpio;

  if (function == FUNCTION_CFG_BUTTON) {
    ConfigManager->setElement(key, CFG, 1);
    return;
  }

  if (function == FUNCTION_CFG_LED) {
    ConfigManager->setElement(key, CFG_LED, 1);
    return;
  }

  ConfigManager->setElement(key, NR, nr);
  ConfigManager->setElement(key, FUNCTION, function);

  /*setLevel(gpio, ConfigESP->getLevel(gpio));
   setMemory(gpio, ConfigESP->getMemory(gpio));
   setPullUp(gpio, ConfigESP->getPullUp(gpio));
   setInversed(gpio, ConfigESP->getInversed(gpio));
   setAction(gpio, ConfigESP->getAction(gpio));
   setEvent(gpio, ConfigESP->getEvent(gpio));
   */
}

void SuplaConfigESP::clearGpio(uint8_t gpio, uint8_t function) {
  uint8_t key = KEY_GPIO + gpio;

  if (function == FUNCTION_CFG_BUTTON) {
    ConfigManager->setElement(key, CFG, 0);
    return;
  }

  if (function == FUNCTION_CFG_LED) {
    ConfigManager->setElement(key, CFG_LED, 0);
    return;
  }

  ConfigManager->setElement(key, NR, 0);
  ConfigManager->setElement(key, FUNCTION, FUNCTION_OFF);

  if (function == FUNCTION_BUTTON || function == FUNCTION_BUTTON_STOP) {
    setPullUp(gpio, true);
    setInversed(gpio, true);

    setAction(gpio, Supla::Action::TOGGLE);
    setEvent(gpio, Supla::Event::ON_PRESS);
  }
  if (function == FUNCTION_RELAY) {
    setLevel(gpio, LOW);
    setMemory(gpio, MEMORY_RESTORE);
  }
  if (function == FUNCTION_LIMIT_SWITCH) {
    setPullUp(gpio, true);
  }
}

uint8_t SuplaConfigESP::countFreeGpio(uint8_t exception) {
  uint8_t count = 1;

  for (uint8_t gpio = 0; gpio < OFF_GPIO; gpio++) {
    if (checkGpio(gpio)) {
      uint8_t key = KEY_GPIO + gpio;
      if (ConfigManager->get(key)->getElement(FUNCTION).toInt() == FUNCTION_OFF ||
          ConfigManager->get(key)->getElement(FUNCTION).toInt() == exception) {
        count++;
      }
    }
    delay(0);
  }

  if (exception == FUNCTION_RELAY)
    count += MAX_VIRTUAL_RELAY;

#ifdef GUI_SENSOR_I2C_EXPENDER
  if (Expander->checkActiveExpander(exception)) {
    count += MAX_EXPANDER_FOR_FUNCTION;
  }
#endif

  return count;
}

bool SuplaConfigESP::checkGpio(int gpio) {
  if (
#ifdef ARDUINO_ARCH_ESP8266
      gpio == 6 || gpio == 7 || gpio == 8 || gpio == 11
#ifdef ARDUINO_ESP8266_GENERIC
      || gpio == 9 || gpio == 10
#endif
#elif ARDUINO_ARCH_ESP32
      gpio == 28 || gpio == 29 || gpio == 30 || gpio == 31
#endif
  ) {
    return false;
  }

  return true;
}

void SuplaConfigESP::factoryReset(bool forceReset) {
  delay(2000);
  pinMode(0, INPUT_PULLUP);
  if (digitalRead(0) != HIGH || forceReset) {
    Serial.println(F("FACTORY RESET!!!"));

    clearEEPROM();

    ConfigManager->deleteAllValues();

    ConfigManager->set(KEY_SUPLA_SERVER, DEFAULT_SERVER);
    ConfigManager->set(KEY_SUPLA_EMAIL, DEFAULT_EMAIL);
    ConfigManager->set(KEY_HOST_NAME, DEFAULT_HOSTNAME);
    ConfigManager->set(KEY_LOGIN, DEFAULT_LOGIN);
    ConfigManager->set(KEY_LOGIN_PASS, DEFAULT_LOGIN_PASS);
    ConfigManager->set(KEY_ENABLE_GUI, getDefaultEnableGUI());
    ConfigManager->set(KEY_ENABLE_SSL, getDefaultEnableSSL());
    Supla::TanplateBoard::addTemplateBoard();
    if (ConfigESP->getGpio(FUNCTION_CFG_BUTTON) == OFF_GPIO)
      ConfigESP->setGpio(0, FUNCTION_CFG_BUTTON);

    ConfigManager->save();

    if (!forceReset) {
      rebootESP();
    }
  }
}
void SuplaConfigESP::reset(bool forceReset) {
  delay(2000);
  pinMode(0, INPUT_PULLUP);
  if (digitalRead(0) != HIGH || forceReset) {
    Serial.println(F("DEVICES CONFIGURATION RESET!"));

    clearEEPROM();
    ConfigManager->deleteDeviceValues();

    ConfigManager->set(KEY_ENABLE_GUI, getDefaultEnableGUI());
    ConfigManager->set(KEY_ENABLE_SSL, getDefaultEnableSSL());
    Supla::TanplateBoard::addTemplateBoard();
    if (ConfigESP->getGpio(FUNCTION_CFG_BUTTON) == OFF_GPIO)
      ConfigESP->setGpio(0, FUNCTION_CFG_BUTTON);

    ConfigManager->save();

    if (!forceReset) {
      rebootESP();
    }
  }
}

void SuplaConfigESP::clearEEPROM() {
  EEPROM.begin(1024);
  delay(15);
  for (int i = 1; i < 1024; ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
}
