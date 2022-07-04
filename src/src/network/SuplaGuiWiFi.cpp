#include "SuplaGuiWiFi.h"
#include "../../SuplaDeviceGUI.h"

namespace Supla {

GUIESPWifi::GUIESPWifi(const char *wifiSsid, const char *wifiPassword) : ESPWifi(wifiSsid, wifiPassword) {
  retryCount = 0;
#ifdef ARDUINO_ARCH_ESP8266
  gotIpEventHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &event) {
    (void)(event);
    Serial.print(F("local IP: "));
    Serial.println(WiFi.localIP());
    Serial.print(F("subnetMask: "));
    Serial.println(WiFi.subnetMask());
    Serial.print(F("gatewayIP: "));
    Serial.println(WiFi.gatewayIP());
    long rssi = WiFi.RSSI();
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
        long rssi = WiFi.RSSI();
        Serial.print(F("Signal Strength (RSSI): "));
        Serial.print(rssi);
        Serial.println(F(" dBm"));
      },
#if defined(ESP_ARDUINO_VERSION)
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2, 0, 0)
      WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
#endif
#else
      WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
#endif

  (void)(event_gotIP);

  WiFiEventId_t event_disconnected = WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) { Serial.println(F("wifi Station disconnected")); },

#if defined(ESP_ARDUINO_VERSION)
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2, 0, 0)
                                                  WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);
#endif
#else
                                                  WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);
#endif
  (void)(event_disconnected);
#endif
}

GUIESPWifi::~GUIESPWifi() {
  delete[] client;
};

int GUIESPWifi::connect(const char *server, int port) {
  if (strcmp(server, DEFAULT_SERVER) == 0) {
    return 0;
  }

  String message;
  if (client == NULL) {
    if (sslEnabled) {
      message = "Secured connection";
      auto clientSec = new WiFiClientSecure();
      client = clientSec;

#ifdef ARDUINO_ARCH_ESP8266
      clientSec->setBufferSizes(1024, 512);  // EXPERIMENTAL
      if (fingerprint.length() > 0) {
        message += " with certificate matching";
        clientSec->setFingerprint(fingerprint.c_str());
      }
      else {
        message += " without certificate matching";
        clientSec->setInsecure();
      }
#elif ARDUINO_ARCH_ESP32
      clientSec->setInsecure();
#endif
    }
    else {
      message = "unsecured connection";
      client = new WiFiClient();
    }
  }

  int connectionPort = (sslEnabled ? 2016 : 2015);
  if (port != -1) {
    connectionPort = port;
  }

  supla_log(LOG_DEBUG, "Establishing %s with: %s (port: %d)", message.c_str(), server, connectionPort);

  bool result = client->connect(server, connectionPort);

  return result;
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
      WiFi.mode(WIFI_MODE_AP);
    }
    else {
      WiFi.mode(WIFI_MODE_STA);
    }
    Serial.println(F("WiFi: resetting WiFi connection"));
    if (client) {
      delete client;
      client = nullptr;
    }
    WiFi.disconnect();
  }

  if (mode == Supla::DEVICE_MODE_CONFIG) {
    Serial.print("WiFi: enter config mode with SSID: ");
    Serial.println(hostname);
    WiFi.mode(WIFI_MODE_AP);
    WiFi.softAP(hostname, nullptr, 6);
  }
  else {
    Serial.print(F("WiFi: establishing connection with SSID: \""));
    Serial.print(ssid);
    Serial.println(F("\""));
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, password);
    // ESP8266 requires setHostname to be called after begin...
    WiFi.setHostname(hostname);
  }

  retryCount++;
  if (retryCount > 4) {
    retryCount = 0;
    wifiConfigured = false;

    if (ConfigManager->get(KEY_FORCE_RESTART_ESP)->getValueBool())
      ConfigESP->rebootESP();
  }

  delay(0);
}

void GUIESPWifi::setHostName(const char *wifiHostname) {
  if (wifiHostname) {
    strncpy(hostname, wifiHostname, MAX_HOSTNAME);
  }
}

void GUIESPWifi::enableSSL(bool value) {
  sslEnabled = value;
  wifiConfigured = false;
  if (client) {
    delete client;
    client = nullptr;
  }
}

void GUIESPWifi::setSsid(const char *wifiSsid) {
  if (wifiSsid) {
    wifiConfigured = false;
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
