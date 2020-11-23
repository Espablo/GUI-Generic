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

#include <supla/triggerable.h>
#include <supla/element.h>
#include "GUI-Generic_Config.h"

enum _configModeESP { NORMAL_MODE, CONFIG_MODE };
enum _ConfigMode { CONFIG_MODE_10_ON_PRESSES, CONFIG_MODE_5SEK_HOLD };


typedef struct {
  int status;
  const char *msg;
  const char *old_msg;
} _supla_status;

class SuplaConfigESP : public Supla::Triggerable, public Supla::Element {
 public:
  SuplaConfigESP();

  void addConfigESP(int _pinNumberConfig, int _pinLedConfig, int _modeConfigButton, bool _highIsOn = true);
  void runAction(int event, int action);
  void rebootESP();

  const char *getLastStatusSupla();

  void ledBlinking(int time);
  void ledBlinkingStop(void);
  int getPinLedConfig();
  virtual uint8_t pinOnValue() {
    return ledHighIsOn ? HIGH : LOW;
  }

  virtual uint8_t pinOffValue() {
    return ledHighIsOn ? LOW : HIGH;
  }

  String getMacAddress(bool formating);

  _configModeESP configModeESP;
  _supla_status supla_status;

  int getGpio(int nr, int function);
  int getGpio(int function) {
    return getGpio(1, function);
  }
  int getLevel(int nr, int function);
  int getLevel(int function) {
    return getLevel(1, function);
  }
  bool checkBusyCfg(int gpio);
  int checkBusyGpio(int gpio, int function);
  uint8_t countFreeGpio(uint8_t exception = 0);
  void setGpio(uint8_t gpio, uint8_t nr, uint8_t function, uint8_t level, uint8_t memory = 0);
  void setGpio(uint8_t gpio, uint8_t function, uint8_t level = 0) {
    setGpio(gpio, 1, function, level);
  }
  void clearGpio(uint8_t gpio, uint8_t function = 0);

  int getMemory(int nr, int function);
  int getMemory(int function) {
    return getMemory(1, function);
  }
  void factoryReset();

 private:
  void configModeInit();
  void iterateAlways();
  int pinNumberConfig;
  int pinLedConfig;
  int modeConfigButton;
  bool ledHighIsOn;

  ETSTimer led_timer;
};

void ledBlinking_func(void *timer_arg);
void status_func(int status, const char *msg);

#endif  // SuplaConfigESP_h
