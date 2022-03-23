#ifndef SuplaGuiWiFi_h
#define SuplaGuiWiFi_h

#include <supla/network/esp_wifi.h>

#define MAX_HOSTNAME   32
#define DEFAULT_SERVER "svrX.supla.org"

namespace Supla {
class GUIESPWifi : public Supla::ESPWifi {
 public:
  GUIESPWifi(const char *wifiSsid = nullptr, const char *wifiPassword = nullptr);
  ~GUIESPWifi();

  int connect(const char *server, int port = -1);
  void setup();
  void setHostName(const char *wifiHostname);
  void enableSSL(bool value);
  void setSsid(const char *wifiSsid);
  void setPassword(const char *wifiPassword);

 protected:
  char hostname[MAX_HOSTNAME];
  int8_t retryCount;
};
};      // namespace Supla
#endif  // SuplaGuiWiFi_h
