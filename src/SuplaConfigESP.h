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

#ifndef SuplaConfigESP_h
#define SuplaConfigESP_h

#include "Arduino.h"
#include "EEPROM.h"
#include <supla/action_handler.h>
#include <supla/element.h>
#include "SuplaConfigManager.h"
#include <ESP8266mDNS.h>

#include <cont.h>
#include <user_interface.h>

enum _configModeESP
{
  NORMAL_MODE,
  CONFIG_MODE
};
enum _ConfigMode
{
  CONFIG_MODE_10_ON_PRESSES,
  CONFIG_MODE_5SEK_HOLD,
  FACTORYRESET
};

#define OFF_GPIO     17
#define OFF_MCP23017 4

typedef struct {
  int status;
  const char *msg;
} _supla_status;

class SuplaConfigESP : public Supla::ActionHandler, public Supla::Element {
 public:
  SuplaConfigESP();

  void addConfigESP(int _pinNumberConfig, int _pinLedConfig);
  void handleAction(int event, int action);
  void rebootESP();

  bool getDefaultEnableSSL();
  bool getDefaultEnableGUI();
  uint8_t getDefaultTamplateBoard();

  bool checkSSL();
  bool checkAvailableGUI();

  const char *getLastStatusMessageSupla();
  int getLastStatusSupla();

  void ledBlinking(int time);
  void ledBlinkingStop(void);

  uint8_t pinOnValue();
  uint8_t pinOffValue();

  String getMacAddress(bool formating);

  _configModeESP configModeESP;
  _supla_status supla_status;

  int getGpio(int nr, int function);
  int getGpio(int function) {
    return getGpio(1, function);
  }

  uint8_t getKeyGpio(uint8_t gpio);

  bool getLevel(uint8_t gpio);
  bool getPullUp(uint8_t gpio);
  bool getInversed(uint8_t gpio);
  uint8_t getMemory(uint8_t gpio);
  uint8_t getAction(uint8_t gpio);
  uint8_t getEvent(uint8_t gpio);

  bool checkBusyCfg(int gpio);
  int checkBusyGpio(int gpio, int function);
  uint8_t countFreeGpio(uint8_t exception = 0);

  void setLevel(uint8_t gpio, int level);
  void setMemory(uint8_t gpio, int memory);

  void setPullUp(uint8_t gpio, int pullup);
  void setInversed(uint8_t gpio, int inversed);
  void setAction(uint8_t gpio, int action);
  void setEvent(uint8_t gpio, int event);

  void setGpio(uint8_t gpio, uint8_t nr, uint8_t function);
  void setGpio(uint8_t gpio, uint8_t function) {
    setGpio(gpio, 1, function);
  }

  void clearGpio(uint8_t gpio, uint8_t function = 0);
  void reset(bool forceReset = false);
  void factoryReset(bool forceReset = false);
  const String getConfigNameAP();

  bool checkBusyGpioMCP23017(uint8_t gpio, uint8_t nr, uint8_t function);
  uint8_t getGpioMCP23017(uint8_t nr, uint8_t function);
  uint8_t getAdressMCP23017(uint8_t nr, uint8_t function);
  void setGpioMCP23017(uint8_t gpio, uint8_t adress, uint8_t nr, uint8_t function);
  void clearGpioMCP23017(uint8_t gpio, uint8_t nr, uint8_t function);
  void clearFunctionGpio(uint8_t function);

  bool checkActiveMCP23017(uint8_t function);
  uint8_t getFunctionMCP23017(uint8_t adress);
  uint8_t getNrMCP23017(uint8_t adress);
  void configModeInit();

 private:
  bool MDNSConfigured = false;
  bool APConfigured = false;
  void iterateAlways();
  void clearEEPROM();

  ETSTimer led_timer;
};

void ledBlinking_func(void *timer_arg);
void status_func(int status, const char *msg);

uint32_t getFreeStackWatermark();
unsigned long FreeMem();
void checkRAM();

#endif  // SuplaConfigESP_h
