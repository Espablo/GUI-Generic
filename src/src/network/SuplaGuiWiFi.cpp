#include "SuplaGuiWiFi.h"
#include "../../SuplaDeviceGUI.h"
#include <supla/tools.h>

namespace Supla {

GUIESPWifi::GUIESPWifi(const char *wifiSsid, const char *wifiPassword) : ESPWifi(wifiSsid, wifiPassword) {
}

void GUIESPWifi::setup() {
  if (!wifiConfigured) {
    // ESP32 requires setHostname to be called before begin...
    WiFi.setHostname(hostname);
    wifiConfigured = true;
#ifdef ARDUINO_ARCH_ESP8266
    gotIpEventHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &event) {
      (void)(event);
      Serial.print(F("local IP: "));
      Serial.println(WiFi.localIP());
      Serial.print(F("subnetMask: "));
      Serial.println(WiFi.subnetMask());
      Serial.print(F("gatewayIP: "));
      Serial.println(WiFi.gatewayIP());
      int rssi = WiFi.RSSI();
      Serial.print(F("Signal strength (RSSI): "));
      Serial.print(rssi);
      Serial.println(F(" dBm"));
    });
    disconnectedEventHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected &event) {
      (void)(event);
      Serial.println(F("WiFi station disconnected"));
    });
#else
    WiFiEventId_t event_gotIP = WiFi.onEvent(
        [](WiFiEvent_t event, WiFiEventInfo_t info) {
          Serial.print(F("local IP: "));
          Serial.println(WiFi.localIP());
          Serial.print(F("subnetMask: "));
          Serial.println(WiFi.subnetMask());
          Serial.print(F("gatewayIP: "));
          Serial.println(WiFi.gatewayIP());
          int rssi = WiFi.RSSI();
          Serial.print(F("Signal Strength (RSSI): "));
          Serial.print(rssi);
          Serial.println(F(" dBm"));
        },
        WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);

    (void)(event_gotIP);

    WiFiEventId_t event_disconnected = WiFi.onEvent(
        [](WiFiEvent_t event, WiFiEventInfo_t info) {
          Serial.println(F("WiFi Station disconnected"));
          // ESP32 doesn't reconnect automatically after lost connection
          WiFi.reconnect();
        },
        WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    (void)(event_disconnected);
#endif
  }
  else {
    if (mode == Supla::DEVICE_MODE_CONFIG) {
      if (getCountChannels() == 0)
        WiFi.mode(WIFI_AP_STA);
      else
        WiFi.mode(WIFI_MODE_AP);
    }
    else {
      WiFi.mode(WIFI_MODE_STA);
    }

    Serial.println(F("WiFi: resetting WiFi connection"));
    DisconnectProtocols();
    WiFi.disconnect();

    forceRestartESP();
  }

  if (mode == Supla::DEVICE_MODE_CONFIG) {
    SUPLA_LOG_INFO("WiFi: enter config mode with SSID: \"%s\"", hostname);
    if (getCountChannels() == 0) {
      WiFi.mode(WIFI_AP_STA);
      WiFi.begin(ssid, password);
    }
    else {
      WiFi.mode(WIFI_MODE_AP);
    }

    uint8_t mac[6] = {};
    char macStr[12 + 6] = {};
    if (Supla::Network::GetMacAddr(mac)) {
      generateHexString(mac, macStr, 6);
    }

    WiFi.softAP(String("SUPLA-GUI-Generic-") + macStr, "", 6);

    Supla::GUI::crateWebServer();
  }
  else {
    SUPLA_LOG_INFO("WiFi: establishing connection with SSID: \"%s\"", ssid);
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, password);
    // ESP8266 requires setHostname to be called after begin...
    WiFi.setHostname(hostname);

    if (ConfigManager->get(KEY_ENABLE_GUI)->getValueInt())
      Supla::GUI::crateWebServer();
  }

  delay(0);
}

void GUIESPWifi::forceRestartESP() {
  if (retryCount > 3) {
    retryCount = 0;

    if (ConfigManager->get(KEY_FORCE_RESTART_ESP)->getValueBool())
      ConfigESP->rebootESP();
  }
}
void GUIESPWifi::setHostName(const char *wifiHostname) {
  if (wifiHostname) {
    strncpy(hostname, wifiHostname, MAX_HOSTNAME);
  }
}

void GUIESPWifi::enableSSL(bool value) {
  setSSLEnabled(value);
}

void GUIESPWifi::setSsid(const char *wifiSsid) {
  if (wifiSsid) {
    strncpy(ssid, wifiSsid, MAX_SSID_SIZE);
  }
}

void GUIESPWifi::setPassword(const char *wifiPassword) {
  if (wifiPassword) {
    wifiConfigured = false;
    strncpy(password, wifiPassword, MAX_WIFI_PASSWORD_SIZE);
  }
}

};  // namespace Supla