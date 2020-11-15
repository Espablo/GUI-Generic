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

#include <ESP8266WiFi.h>

#include "SuplaConfigESP.h"
#include "SuplaConfigManager.h"
#include "SuplaDeviceGUI.h"

SuplaConfigESP::SuplaConfigESP() {
  configModeESP = NORMAL_MODE;

  if (ConfigManager->isDeviceConfigured()) {
    ConfigManager->setGUIDandAUTHKEY();
    if (String(ConfigManager->get(KEY_LOGIN)->getValue()) == 0) {
      ConfigManager->set(KEY_LOGIN, DEFAULT_LOGIN);
    }
    if (String(ConfigManager->get(KEY_LOGIN_PASS)->getValue()) == 0) {
      ConfigManager->set(KEY_LOGIN_PASS, DEFAULT_LOGIN_PASS);
    }
    if (String(ConfigManager->get(KEY_HOST_NAME)->getValue()) == 0) {
      ConfigManager->set(KEY_HOST_NAME, DEFAULT_HOSTNAME);
    }
    if (String(ConfigManager->get(KEY_SUPLA_SERVER)->getValue()) == 0) {
      ConfigManager->set(KEY_SUPLA_SERVER, DEFAULT_SERVER);
    }
    if (String(ConfigManager->get(KEY_SUPLA_EMAIL)->getValue()) == 0) {
      ConfigManager->set(KEY_SUPLA_EMAIL, DEFAULT_EMAIL);
    }
    ConfigManager->save();

    configModeInit();
  }

  // if(String(ConfigManager->get(KEY_WIFI_SSID)->getValue()) == 0 ||
  //         String(ConfigManager->get(KEY_WIFI_PASS)->getValue()) == 0 ||
  //         String(ConfigManager->get(KEY_SUPLA_SERVER)->getValue()) ==
  //         DEFAULT_SERVER ||
  //         String(ConfigManager->get(KEY_SUPLA_EMAIL)->getValue()) ==
  //         DEFAULT_EMAIL){
  //   configModeInit();
  //   return;
  // }
  SuplaDevice.setStatusFuncImpl(&status_func);
}

void SuplaConfigESP::addConfigESP(int _pinNumberConfig, int _pinLedConfig, int _modeConfigButton, bool _highIsOn) {
  pinNumberConfig = _pinNumberConfig;
  pinLedConfig = _pinLedConfig;
  modeConfigButton = _modeConfigButton;
  highIsOn = _highIsOn;

  if (pinLedConfig <= 0) {
    Serial.println(F("ESP  - status LED disabled"));
  }
  else {
    pinMode(pinLedConfig, OUTPUT);
    digitalWrite(pinLedConfig, pinOffValue());
  }

  Supla::Control::Button *buttonConfig = new Supla::Control::Button(pinNumberConfig, true, true);

  if (modeConfigButton == CONFIG_MODE_10_ON_PRESSES) {
    buttonConfig->addAction(CONFIG_MODE_10_ON_PRESSES, *ConfigESP, Supla::ON_PRESS);
  }
  if (modeConfigButton == CONFIG_MODE_5SEK_HOLD) {
    buttonConfig->addAction(CONFIG_MODE_5SEK_HOLD, *ConfigESP, Supla::ON_PRESS);
    buttonConfig->addAction(CONFIG_MODE_5SEK_HOLD, *ConfigESP, Supla::ON_RELEASE);
  }
}

void SuplaConfigESP::runAction(int event, int action) {
  if (action == CONFIG_MODE_10_ON_PRESSES) {
    if (millis() - cnfigChangeTimeMs > 10000UL) {
      cnfigChangeTimeMs = millis();
      countPresses = 0;
    }
    countPresses++;

    if (countPresses == 10) {
      //      Serial.println(F("CONFIG_MODE_3_PRESSES"));
      configModeInit();
      countPresses = 0;
      return;
    }
  }

  if (action == CONFIG_MODE_5SEK_HOLD) {
    if (event == Supla::ON_PRESS) {
      cnfigChangeTimeMs = millis();
    }
    if (event == Supla::ON_RELEASE) {
      if (millis() - cnfigChangeTimeMs > 5000UL) {
        if (!digitalRead(pinNumberConfig)) {
          //        Serial.println(F("CONFIG_MODE_5SEK_HOLD"));
          configModeInit();
        }
      }
      cnfigChangeTimeMs = 0;
    }
  }

  if (configModeESP == CONFIG_MODE) {
    if (event == Supla::ON_PRESS) {
      rebootESP();
    }
  }
}

void SuplaConfigESP::rebootESP() {
  delay(1000);
  WiFi.forceSleepBegin();
  wdt_reset();
  ESP.restart();
  while (1) wdt_reset();
}

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case WIFI_EVENT_STAMODE_CONNECTED:
      //      Serial.print(millis());
      //      Serial.print(" => ");
      //
      //      Serial.println(F("WIFI_EVENT_STAMODE_CONNECTED"));
      break;
    case WIFI_EVENT_STAMODE_DISCONNECTED:
      //      Serial.print(millis());
      //      Serial.print(" => ");
      //
      //      Serial.println(F("WiFi lost connection"));
      break;
    case WIFI_EVENT_STAMODE_AUTHMODE_CHANGE:
      //      Serial.print(millis());
      //      Serial.print(" => ");
      //
      //      Serial.println(F("WIFI_EVENT_STAMODE_AUTHMODE_CHANGE"));
      break;
    case WIFI_EVENT_STAMODE_GOT_IP:
      //      Serial.print(millis());
      //      Serial.print(" => ");
      //      Serial.println(F("WIFI_EVENT_STAMODE_GOT_IP"));
      //      Serial.println(WiFi.localIP());
      break;
    case WIFI_EVENT_STAMODE_DHCP_TIMEOUT:
      //      Serial.print(millis());
      //      Serial.print(" => ");
      //
      //      Serial.println(F("WIFI_EVENT_STAMODE_DHCP_TIMEOUT"));
      break;
    case WIFI_EVENT_SOFTAPMODE_STACONNECTED:
      //      Serial.print(millis());
      //      Serial.print(" => ");
      //
      //      Serial.println(F("WIFI_EVENT_SOFTAPMODE_STACONNECTED"));
      break;
    case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED:
      //      Serial.print(millis());
      //      Serial.print(" => ");
      //
      //      Serial.println(F("WIFI_EVENT_SOFTAPMODE_STADISCONNECTED"));
      break;
    case WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED:
      // Serial.print(" => ");
      // Serial.println("WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED"));
      break;
    case WIFI_EVENT_MAX:
      //      Serial.print(millis());
      //      Serial.print(" => ");
      //
      //      Serial.println(F("WIFI_EVENT_MAX"));
      break;
  }
}

void SuplaConfigESP::configModeInit() {
  configModeESP = CONFIG_MODE;
  ledBlinking(100);

  WiFi.onEvent(WiFiEvent);
  // Serial.print(F("Creating Access Point"));
  // Serial.print(F("Setting mode ... "));
  // Serial.println(WiFi.mode(WIFI_AP_STA) ? "Ready" : "Failed!");
  WiFi.softAPdisconnect(true);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP_STA);

  String CONFIG_WIFI_NAME = "SUPLA-ESP8266-" + getMacAddress(false);
  while (!WiFi.softAP(CONFIG_WIFI_NAME, "")) {
    // Serial.println(F("."));
    delay(100);
  }

  // Serial.println(F("Network Created!"));
  // Serial.print(F("Soft-AP IP address = "));
  // Serial.println(WiFi.softAPIP());
}

const char *SuplaConfigESP::getLastStatusSupla() {
  return supla_status.msg;
}

void SuplaConfigESP::ledBlinking(int time) {
  os_timer_disarm(&led_timer);
  os_timer_setfn(&led_timer, ledBlinking_func, NULL);
  os_timer_arm(&led_timer, time, true);
}

void SuplaConfigESP::ledBlinkingStop(void) {
  os_timer_disarm(&led_timer);
  digitalWrite(pinLedConfig, pinOffValue());
}

int SuplaConfigESP::getPinLedConfig() {
  return pinLedConfig;
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

void ledBlinking_func(void *timer_arg) {
  int val = digitalRead(ConfigESP->getPinLedConfig());
  digitalWrite(ConfigESP->getPinLedConfig(), val == HIGH ? 0 : 1);
}

void status_func(int status, const char *msg) {
  switch (status) {
    case 2:
      ConfigESP->supla_status.msg = "Już zainicjalizowane";
      break;
    case 3:
      ConfigESP->supla_status.msg = "Nie przypisane CB";
      break;
    case 4:
      ConfigESP->supla_status.msg =
        "Nieprawidłowy identyfikator GUID lub rejestracja urządzeń "
        "NIEAKTYWNA";
      break;
    case 5:
      ConfigESP->supla_status.msg = "Nieznany adres serwera";
      break;
    case 6:
      ConfigESP->supla_status.msg = "Nieznany identyfikator ID";
      break;
    case 7:
      ConfigESP->supla_status.msg = "Zainicjowany";
      break;
    case 8:
      ConfigESP->supla_status.msg = "Przekroczono limit kanału";
      break;
    case 9:
      ConfigESP->supla_status.msg = "Rozłączony";
      break;
    case 10:
      ConfigESP->supla_status.msg = "Rejestracja w toku";
      break;
    case 11:
      ConfigESP->supla_status.msg = "Błąd zmiennej";
      break;
    case 12:
      ConfigESP->supla_status.msg = "Błąd wersji protokołu";
      break;
    case 13:
      ConfigESP->supla_status.msg = "Złe poświadczenia";
      break;
    case 14:
      ConfigESP->supla_status.msg = "Tymczasowo niedostępne";
      break;
    case 15:
      ConfigESP->supla_status.msg = "Konflikt lokalizacji";
      break;
    case 16:
      ConfigESP->supla_status.msg = "Konflikt kanałów";
      break;
    case 17:
      ConfigESP->supla_status.msg = "Zarejestrowany i gotowy";
      break;
    case 18:
      ConfigESP->supla_status.msg = "Urządzenie jest rozłączone";
      break;
    case 19:
      ConfigESP->supla_status.msg = "Lokalizacja jest wyłączona";
      break;
    case 20:
      ConfigESP->supla_status.msg = "Przekroczono limit urządzeń";
  }

  static int lock;
  if (status == 17 && ConfigESP->configModeESP == NORMAL_MODE) {
    ConfigESP->ledBlinkingStop();
    lock = 0;
  }
  else if (status != 17 && lock == 0 && ConfigESP->configModeESP == NORMAL_MODE) {
    ConfigESP->ledBlinking(500);
    lock = 1;
  }

  if (ConfigESP->supla_status.old_msg != ConfigESP->supla_status.msg) {
    ConfigESP->supla_status.old_msg = ConfigESP->supla_status.msg;
    ConfigESP->supla_status.status = status;
    //    Serial.println(ConfigESP->supla_status.msg);
  }
}
#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
int SuplaConfigESP::getMemoryRelay(int nr) {
  uint8_t gpio;
  for (gpio = 0; gpio <= OFF_GPIO; gpio++) {
    String key = GPIO;
    key += gpio;
    if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_RELAY) {
      if (ConfigManager->get(key.c_str())->getElement(NR).toInt() == nr) {
        uint8_t memory = ConfigManager->get(key.c_str())->getElement(MEMORY).toInt();
        return memory;
      }
    }
  }
  return OFF_GPIO;
}
#endif

int SuplaConfigESP::getGpio(int nr, int function) {
  uint8_t gpio;
  for (gpio = 0; gpio <= OFF_GPIO; gpio++) {
    String key = GPIO;
    key += gpio;
    if (function == FUNCTION_CFG_BUTTON) {
      if (checkBusyCfg(gpio)) {
        return gpio;
      }
    }
    if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == function) {
      if (ConfigManager->get(key.c_str())->getElement(NR).toInt() == nr) {
        return gpio;
      }
    }
  }
  return OFF_GPIO;
}

int SuplaConfigESP::getLevel(int nr, int function) {
  uint8_t gpio;
  for (gpio = 0; gpio <= OFF_GPIO; gpio++) {
    String key = GPIO;
    key += gpio;
    if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == function) {
      if (ConfigManager->get(key.c_str())->getElement(NR).toInt() == nr) {
        uint8_t level = ConfigManager->get(key.c_str())->getElement(LEVEL).toInt();
        return level;
      }
    }
  }
  return OFF_GPIO;
}

bool SuplaConfigESP::checkBusyCfg(int gpio) {
  String key = GPIO;
  key += gpio;
  if (ConfigManager->get(key.c_str())->getElement(FUNCTION_CFG_LED).toInt() == 1) {
    return true;
  }
  return false;
}

int SuplaConfigESP::checkBusyGpio(int gpio, int function) {
  if (gpio == 6 || gpio == 7 || gpio == 8 || gpio == 11) {
    return true;
  }
  else {
    String key = GPIO;
    key += gpio;
    if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_BUTTON) {
      if (function == FUNCTION_CFG_BUTTON) {
        return false;
      }
    }
    if (checkBusyCfg(gpio)) {
      if (function != FUNCTION_BUTTON) {
        return true;
      }
    }
    if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() != FUNCTION_OFF) {
      if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() != function) {
        return true;
      }
    }
    return false;
  }
}

void SuplaConfigESP::setGpio(uint8_t gpio, uint8_t nr, uint8_t function, uint8_t level, uint8_t memory) {
  String key = GPIO;
  key += gpio;
  if (function == FUNCTION_CFG_BUTTON) {
    ConfigManager->setElement(key.c_str(), CFG, 1);
    return;
  }

  ConfigManager->setElement(key.c_str(), NR, nr);
  ConfigManager->setElement(key.c_str(), FUNCTION, function);
  ConfigManager->setElement(key.c_str(), LEVEL, level);
  ConfigManager->setElement(key.c_str(), MEMORY, memory);

  // ConfigManager->setElement(key.c_str(), MEMORY, memory);
  // ConfigManager->setElement(key.c_str(), CFG, cfg);
}

void SuplaConfigESP::clearGpio(uint8_t gpio, uint8_t function) {
  String key = GPIO;
  key += gpio;
  if (function == FUNCTION_CFG_BUTTON) {
    ConfigManager->setElement(key.c_str(), CFG, 0);
    return;
  }
  ConfigManager->setElement(key.c_str(), NR, 0);
  ConfigManager->setElement(key.c_str(), FUNCTION, FUNCTION_OFF);
  ConfigManager->setElement(key.c_str(), LEVEL, 0);
  ConfigManager->setElement(key.c_str(), MEMORY, 0);
}

uint8_t SuplaConfigESP::countFreeGpio(uint8_t exception) {
  uint8_t count = 0;
  for (uint8_t gpio = 0; gpio < OFF_GPIO; gpio++) {
    if (gpio != 6 && gpio != 7 && gpio != 8 && gpio != 11) {
      String key = GPIO;
      key += gpio;
      if (ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == FUNCTION_OFF ||
          ConfigManager->get(key.c_str())->getElement(FUNCTION).toInt() == exception) {
        count++;
      }
    }
  }
  return count;
}

void SuplaConfigESP::factoryReset() {
  delay(1000);
  pinMode(0, INPUT);
  if (!digitalRead(0)) {
    Serial.println("FACTORY RESET!!!");

    ConfigManager->set(KEY_WIFI_SSID, "");
    ConfigManager->set(KEY_WIFI_PASS, "");
    ConfigManager->set(KEY_SUPLA_SERVER, DEFAULT_SERVER);
    ConfigManager->set(KEY_SUPLA_EMAIL, DEFAULT_EMAIL);
    ConfigManager->set(KEY_HOST_NAME, DEFAULT_HOSTNAME);
    ConfigManager->set(KEY_LOGIN, DEFAULT_LOGIN);
    ConfigManager->set(KEY_LOGIN_PASS, DEFAULT_LOGIN_PASS);
    ConfigManager->set(KEY_MAX_ROLLERSHUTTER, "0");
    ConfigManager->set(KEY_MAX_RELAY, "1");
    ConfigManager->set(KEY_MAX_BUTTON, "1");
    ConfigManager->set(KEY_MAX_LIMIT_SWITCH, "0");
    ConfigManager->set(KEY_MAX_DHT22, "1");
    ConfigManager->set(KEY_MAX_DHT11, "1");
    ConfigManager->set(KEY_MULTI_MAX_DS18B20, "1");
    ConfigManager->set(KEY_ALTITUDE_BME280, "0");

    int nr;
    String key;

    for (nr = 0; nr <= 17; nr++) {
      key = GPIO;
      key += nr;
      ConfigManager->set(key.c_str(), "0,0,0,0,0");
    }

    ConfigManager->set(KEY_ACTIVE_SENSOR, "0,0,0,0,0");

    for (nr = 0; nr <= MAX_DS18B20; nr++) {
      key = KEY_DS;
      key += nr;
      ConfigManager->set(key.c_str(), "");
      key = KEY_DS_NAME;
      key += nr;
      ConfigManager->set(key.c_str(), "");
    }

    ConfigManager->save();

    delay(3000);
    WiFi.forceSleepBegin();
    wdt_reset();
    ESP.restart();
    while (1) wdt_reset();
  }
}
