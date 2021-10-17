#ifndef SuplaGuiWiFi_h
#define SuplaGuiWiFi_h

#include <supla/network/esp_wifi.h>

#define MAX_HOSTNAME 32

namespace Supla {
class GUIESPWifi : public Supla::ESPWifi {
 public:
  GUIESPWifi(const char *wifiSsid = nullptr, const char *wifiPassword = nullptr)
      : ESPWifi(wifiSsid, wifiPassword) {
  }

  int connect(const char *server, int port = -1) {
    String message;
    if (client == NULL) {
      if (isSecured) {
        message = "Secured connection";
        auto clientSec = new WiFiClientSecure();
        client = clientSec;

#ifdef ARDUINO_ARCH_ESP8266
        clientSec->setBufferSizes(2048, 512);  // EXPERIMENTAL
        if (fingerprint.length() > 0) {
          message += " with certificate matching";
          clientSec->setFingerprint(fingerprint.c_str());
        } else {
          message += " without certificate matching";
          clientSec->setInsecure();
        }
#elif ARDUINO_ARCH_ESP32
        clientSec->setInsecure();
#endif
      } else {
        message = "unsecured connection";
        client = new WiFiClient();
      }
    }

    int connectionPort = (isSecured ? 2016 : 2015);
    if (port != -1) {
      connectionPort = port;
    }

    supla_log(LOG_DEBUG,
              "Establishing %s with: %s (port: %d)",
              message.c_str(),
              server,
              connectionPort);

    bool result = client->connect(server, connectionPort);

    return result;
  }

  void setup() {
    if (!wifiConfigured) {
      wifiConfigured = true;
#ifdef ARDUINO_ARCH_ESP8266
      gotIpEventHandler =
          WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &event) {
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
      disconnectedEventHandler = WiFi.onStationModeDisconnected(
          [](const WiFiEventStationModeDisconnected &event) {
            (void)(event);
            Serial.println(F("WiFi station disconnected"));
          });
#else
      WiFi.onEvent(
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
          WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);

      WiFi.onEvent(
          [](WiFiEvent_t event, WiFiEventInfo_t info) {
            Serial.println(F("wifi Station disconnected"));
          },
          WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);
#endif
      Serial.print(F("WiFi: establishing connection with SSID: \""));
      Serial.print(ssid);
      Serial.println(F("\""));

      if (ConfigESP->configModeESP == NORMAL_MODE) {
        WiFi.softAPdisconnect(true);
        WiFi.setAutoConnect(false);
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
      }

      if (hostname) {
        WiFi.setHostname(hostname);
      }

    } else if (strcmp(ssid, "") != 0) {
      Serial.println(F("WiFi: resetting WiFi connection"));
      if (client) {
        delete client;
        client = nullptr;
      }
      WiFi.reconnect();
    }

    delay(0);
  }

  void setHostName(const char *wifiHostname) {
    if (wifiHostname) {
      strncpy(hostname, wifiHostname, MAX_HOSTNAME);
    }
  }

  void enableSSL(bool value) {
    isSecured = value;
    wifiConfigured = false;
    if (client) {
      delete client;
      client = nullptr;
    }
  }

 protected:
  char hostname[MAX_HOSTNAME];
};
};      // namespace Supla
#endif  // SuplaGuiWiFi_h
