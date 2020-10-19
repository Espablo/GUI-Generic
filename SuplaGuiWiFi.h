#ifndef SuplaGuiWiFi_h
#define SuplaGuiWiFi_h

#include <supla/network/esp_wifi.h>

#define MAX_HOSTNAME           32

namespace Supla {
class GUIESPWifi : public Supla::ESPWifi {

    public:
        GUIESPWifi(const char *wifiSsid = nullptr, const char *wifiPassword = nullptr, IPAddress *ip = nullptr) : ESPWifi(wifiSsid, wifiPassword, ip){
        }

    int connect(const char *server, int port = -1) {
        String message;
        if (client == NULL) {
            if (isSecured) {
            message = "Secured connection";
            client = new WiFiClientSecure();
            if (fingerprint.length() > 0) {
                message += " with certificate matching";
                ((WiFiClientSecure *)client)->setFingerprint(fingerprint.c_str());
            } else {
                message += " without certificate matching";
                ((WiFiClientSecure *)client)->setInsecure();
            }
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

        if (isBuffer) {
            static_cast<WiFiClientSecure*>(client)->setBufferSizes(256, 256); // EXPERIMENTAL
        }
        static_cast<WiFiClientSecure*>(client)->setTimeout(500);

        bool result = client->connect(server, connectionPort);

        if (result && isSecured) {
            if (!((WiFiClientSecure *)client)->verify(fingerprint.c_str(), server)) {
            supla_log(LOG_DEBUG, "Provided certificates doesn't match!");
            client->stop();
            return false;
            }
        };

        return result;
        }
        void enableBuffer(bool value) {
            isBuffer = value;
        }

        void setHostName(const char* wifiHostname) {
            if (wifiHostname) {
                strncpy(hostname, wifiHostname, MAX_HOSTNAME);
            }
        }

protected:
    bool isBuffer;
    char hostname[MAX_HOSTNAME];
};
};  // namespace Supla
#endif //SuplaGuiWiFi_h
