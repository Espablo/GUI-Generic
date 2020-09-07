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
#include "SuplaDeviceGUI.h"
#include "SuplaConfigManager.h"

SuplaConfigESP::SuplaConfigESP() {
  configModeESP = NORMAL_MODE;

  if (ConfigManager->isDeviceConfigured()) {
    ConfigManager->setGUIDandAUTHKEY();
    if(String(ConfigManager->get(KEY_LOGIN)->getValue()) == 0){
      ConfigManager->set(KEY_LOGIN, DEFAULT_LOGIN);
    }    
    if(String(ConfigManager->get(KEY_LOGIN_PASS)->getValue()) == 0){
      ConfigManager->set(KEY_LOGIN_PASS, DEFAULT_LOGIN_PASS);
    }
    if(String(ConfigManager->get(KEY_HOST_NAME)->getValue()) == 0){
      ConfigManager->set(KEY_HOST_NAME, DEFAULT_HOSTNAME);
    }    
    if(String(ConfigManager->get(KEY_SUPLA_SERVER)->getValue()) == 0){
      ConfigManager->set(KEY_SUPLA_SERVER, DEFAULT_SERVER);
    }    
    if(String(ConfigManager->get(KEY_SUPLA_EMAIL)->getValue()) == 0){
      ConfigManager->set(KEY_SUPLA_EMAIL, DEFAULT_EMAIL);
    }    

//    char BUTTON[MAX_TYPE_BUTTON];
//    memset(BUTTON, Supla::ON_PRESS + '0', MAX_TYPE_BUTTON);
//    ConfigManager->set(KEY_TYPE_BUTTON, BUTTON);

    ConfigManager->save();
    configModeInit();
  }

  SuplaDevice.setStatusFuncImpl(&status_func);
}

 void SuplaConfigESP::addConfigESP(int _pinNumberConfig, int _pinLedConfig, int _modeConfigButton, bool _highIsOn) {
  pinNumberConfig = _pinNumberConfig;
  pinLedConfig = _pinLedConfig;
  modeConfigButton = _modeConfigButton;
  highIsOn = _highIsOn;
  
  if (pinLedConfig <= 0) {
    Serial.println(F("ESP  - status LED disabled"));
  } else {
    pinMode(pinLedConfig, OUTPUT);
    digitalWrite(pinLedConfig, pinOffValue());
  }

  Supla::Control::Button *buttonConfig = new Supla::Control::Button(pinNumberConfig, true, true);

  if (modeConfigButton == CONFIG_MODE_10_ON_PRESSES) {
    buttonConfig->willTrigger(*ConfigESP, Supla::ON_PRESS, CONFIG_MODE_10_ON_PRESSES);
  }
  if (modeConfigButton == CONFIG_MODE_5SEK_HOLD) {
    buttonConfig->willTrigger(*ConfigESP, Supla::ON_PRESS, CONFIG_MODE_5SEK_HOLD);
    buttonConfig->willTrigger(*ConfigESP, Supla::ON_RELEASE, CONFIG_MODE_5SEK_HOLD);
  }
}

void SuplaConfigESP::trigger(int event, int action) {
  if (action == CONFIG_MODE_10_ON_PRESSES) {
    if (millis() - cnfigChangeTimeMs > 10000UL) {
      cnfigChangeTimeMs = millis();
      countPresses = 0;
    }
    countPresses++;

    if (countPresses == 3) {
      Serial.println(F("CONFIG_MODE_3_PRESSES"));
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
        if(!digitalRead(pinNumberConfig)){
        Serial.println(F("CONFIG_MODE_5SEK_HOLD"));
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
  while (1)wdt_reset();
}

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case WIFI_EVENT_STAMODE_CONNECTED:
      Serial.print(millis());
      Serial.print(" => ");

      Serial.println(F("WIFI_EVENT_STAMODE_CONNECTED"));
      break;
    case WIFI_EVENT_STAMODE_DISCONNECTED:
      Serial.print(millis());
      Serial.print(" => ");

      Serial.println(F("WiFi lost connection"));
      break;
    case WIFI_EVENT_STAMODE_AUTHMODE_CHANGE:
      Serial.print(millis());
      Serial.print(" => ");

      Serial.println(F("WIFI_EVENT_STAMODE_AUTHMODE_CHANGE"));
      break;
    case WIFI_EVENT_STAMODE_GOT_IP:
      Serial.print(millis());
      Serial.print(" => ");
      Serial.println(F("WIFI_EVENT_STAMODE_GOT_IP"));
      Serial.println(WiFi.localIP());
      break;
    case WIFI_EVENT_STAMODE_DHCP_TIMEOUT:
      Serial.print(millis());
      Serial.print(" => ");

      Serial.println(F("WIFI_EVENT_STAMODE_DHCP_TIMEOUT"));
      break;
    case WIFI_EVENT_SOFTAPMODE_STACONNECTED:
      Serial.print(millis());
      Serial.print(" => ");

      Serial.println(F("WIFI_EVENT_SOFTAPMODE_STACONNECTED"));
      break;
    case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED:
      Serial.print(millis());
      Serial.print(" => ");

      Serial.println(F("WIFI_EVENT_SOFTAPMODE_STADISCONNECTED"));
      break;
    case WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED:
      // Serial.print(" => ");
      //Serial.println("WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED"));
      break;
    case WIFI_EVENT_MAX:
      Serial.print(millis());
      Serial.print(" => ");

      Serial.println(F("WIFI_EVENT_MAX"));
      break;
  }
}

void SuplaConfigESP::configModeInit() {
  configModeESP = CONFIG_MODE;
  ledBlinking(100);
  WiFi.softAPdisconnect(true);

  WiFi.onEvent(WiFiEvent);
  Serial.print(F("Creating Access Point"));
  Serial.print(F("Setting mode ... "));
  Serial.println(WiFi.mode(WIFI_AP_STA) ? "Ready" : "Failed!");
  WiFi.disconnect(true);

  String CONFIG_WIFI_NAME = "SUPLA-ESP8266-" + getMacAddress(false);
  while (!WiFi.softAP(CONFIG_WIFI_NAME, ""))
  {
    Serial.println(F("."));
    delay(100);
  }
  Serial.println(F("Network Created!"));
  Serial.print(F("Soft-AP IP address = "));
  Serial.println(WiFi.softAPIP());
}

const char *SuplaConfigESP::getLastStatusSupla() {
  return supla_status.msg;
}

void SuplaConfigESP::ledBlinking(int time) {
  os_timer_disarm(&led_timer);
  os_timer_setfn(&led_timer, ledBlinking_func, NULL);
  os_timer_arm (&led_timer, time, true);
}

void SuplaConfigESP::ledBlinkingStop(void) {
  os_timer_disarm(&led_timer);
  digitalWrite(pinLedConfig, pinOffValue());
}

int SuplaConfigESP::getPinLedConfig() {
  return pinLedConfig;
}

String SuplaConfigESP::getMacAddress(bool formating) {
  uint8_t mac_addr[6];
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
      ConfigESP->supla_status.msg = "Nieprawidłowy identyfikator GUID lub rejestracja urządzeń NIEAKTYWNA";
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
    Serial.println(ConfigESP->supla_status.msg);
  }
}
