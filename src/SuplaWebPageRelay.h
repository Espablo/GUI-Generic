#ifndef SuplaWebPageRelay_h
#define SuplaWebPageRelay_h

#include "SuplaDeviceGUI.h"

#if defined(SUPLA_RELAY) || defined(SUPLA_MCP23017)
#define GUI_RELAY
#endif

#define INPUT_ADRESS_MCP23017 "iam"
#define PATH_RELAY_SET        "setrelay"

#ifdef GUI_RELAY
#define PATH_RELAY     "relay"
#define PATH_RELAY_SET "setrelay"

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

void createWebPageRelay();
void handleRelay(int save = 0);
void handleRelaySave();
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

#ifdef SUPLA_RELAY
void handleRelaySet(int save = 0);
void handleRelaySaveSet();
#endif

#ifdef SUPLA_MCP23017
void handleRelaySetMCP23017(int save = 0);
void handleRelaySaveSetMCP23017();
#endif

#ifdef SUPLA_PUSHOVER
#define INPUT_PUSHOVER_MESSAGE "pm"
#endif

#ifdef SUPLA_DIRECT_LINKS
#define INPUT_DIRECT_LINK_ON  "dlo"
#define INPUT_DIRECT_LINK_OFF "dlof"
#endif

#ifdef SUPLA_LED
#define PATH_LED        "led"
#define PATH_SAVE_LED   "saveled"
#define INPUT_LED       "led"
#define INPUT_LEVEL_LED "ill"
#endif

#endif  // SuplaWebPageRelay_h
