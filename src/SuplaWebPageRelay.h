#ifndef SuplaWebPageRelay_h
#define SuplaWebPageRelay_h

#include "SuplaDeviceGUI.h"

#if defined(SUPLA_RELAY) || defined(SUPLA_MCP23017)
#define GUI_RELAY
#endif

#define INPUT_ADRESS_MCP23017 "iam"

#ifdef GUI_RELAY
#define PATH_RELAY "relay"

#define PATH_RELAY_SET          "setrelay"
#define PATH_RELAY_SET_MCP23017 "setrelaymcp"

#define INPUT_MAX_RELAY      "mrl"
#define INPUT_RELAY_GPIO     "rlg"
#define INPUT_RELAY_LEVEL    "irl"
#define INPUT_RELAY_MEMORY   "irm"
#define INPUT_RELAY_DURATION "ird"
#define INPUT_ROLLERSHUTTER  "irsr"

#define INPUT_CONDITIONS_SENSOR_TYPE "cst"
#define INPUT_CONDITIONS_TYPE        "ct"
#define INPUT_CONDITIONS_MIN         "cmi"
#define INPUT_CONDITIONS_MAX         "cma"

#endif

enum _memory_relay
{
  MEMORY_RELAY_OFF,
  MEMORY_RELAY_ON,
  MEMORY_RELAY_RESTORE
};

enum _conditions
{
  CONDITIONS_DS18B20 = 1,
  CONDITIONS_DHT_TEMP,
  CONDITIONS_DHT_HUMI,
};

#if defined(SUPLA_PUSHOVER)
#define INPUT_PUSHOVER         "po"
#define INPUT_PUSHOVER_MESSAGE "pm"
#endif

#if defined(SUPLA_DIRECT_LINKS)
#define INPUT_DIRECT_LINK_ON  "dlo"
#define INPUT_DIRECT_LINK_OFF "dlof"
#endif

class SuplaWebPageRelay {
 public:
  SuplaWebPageRelay();

#ifdef GUI_RELAY
  void createWebPageRelay();
  void handleRelay();
  void handleRelaySave();
  void supla_webpage_relay(int save);
#endif

#if defined(SUPLA_RELAY)
  void handleRelaySet();
  void handleRelaySaveSet();

  void supla_webpage_relay_set(int save, int nr = 0);
#endif

#ifdef SUPLA_MCP23017
  void supla_webpage_relay_set_MCP23017(int save);
  void handleRelaySaveSetMCP23017();
  void handleRelaySetMCP23017();
#endif

#if defined(SUPLA_LED)
#define PATH_LED        "led"
#define PATH_SAVE_LED   "saveled"
#define INPUT_LED       "led"
#define INPUT_LEVEL_LED "ill"
#endif
};

extern SuplaWebPageRelay* WebPageRelay;
#endif  // SuplaWebPageRelay_h
