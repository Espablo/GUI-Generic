
#ifndef SuplaWebPageHome_h
#define SuplaWebPageHome_h

#define PATH_REBOT "rbt"

#define INPUT_WIFI_SSID     "sid"
#define INPUT_WIFI_PASS     "wpw"
#define INPUT_EMAIL         "eml"
#define INPUT_SERVER        "svr"
#define INPUT_HOSTNAME      "shn"
#define INPUT_MODUL_LOGIN   "mlg"
#define INPUT_MODUL_PASS    "mps"
#define INPUT_ROLLERSHUTTER "irsr"

void createWebPageHome();
void handlePageHome(int save = 0);
void handlePageHomeSave();

#endif  // ifndef SuplaWebPageHome_h