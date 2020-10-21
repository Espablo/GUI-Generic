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
#include "GUI-Generic_Config.h"

enum _configModeESP { NORMAL_MODE, CONFIG_MODE };

typedef struct {
  int status;
  const char *msg;
  const char *old_msg;
} _supla_status;

class SuplaConfigESP : public Supla::Triggerable {
public:
  SuplaConfigESP();

  void addConfigESP(int _pinNumberConfig, int _pinLedConfig,
                    int _modeConfigButton, bool _highIsOn = true);
  void runAction(int event, int action);
  void rebootESP();

  const char *getLastStatusSupla();

  void ledBlinking(int time);
  void ledBlinkingStop(void);
  int getPinLedConfig();
  virtual uint8_t pinOnValue() { return highIsOn ? HIGH : LOW; }

  virtual uint8_t pinOffValue() { return highIsOn ? LOW : HIGH; }

  String getMacAddress(bool formating);

  _configModeESP configModeESP;
  _supla_status supla_status;

  int sort(int function);
  int getGpio(int nr, int function);
  int getLevel(int nr, int function);
  int checkBusy(int gpio, int function);

  int getMemoryRelay(int nr);
  int getCfgFlag();

  void factoryReset();

 private:
  void configModeInit();

  int pinNumberConfig;
  int pinLedConfig;
  int modeConfigButton;
  int countPresses = 0;
  unsigned long cnfigChangeTimeMs = 0;
  bool highIsOn;
  // bool factoryReset = 0;

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
  int _relayGpio[17];
  int _relayLevel[17];
  int _relayMemory[17];
#endif

#ifdef SUPLA_BUTTON
  int _buttonGpio[17];
  int _buttonLevel[17];
#endif

#ifdef SUPLA_LIMIT_SWITCH
  int _limitSwiitchGpio[17];
#endif

#ifdef SUPLA_DHT11
  int _dht11Gpio[17];
#endif

#ifdef SUPLA_DHT22
  int _dht22Gpio[17];
#endif

#ifdef SUPLA_DS18B20
  int _ds18b20Gpio[17];
#endif

#ifdef SUPLA_BME280
  int _sdaGpio[17];
  int _sclGpio[17];
#endif

#ifdef SUPLA_HC_SR04
  int _trigGpio[17];
  int _echoGpio[17];
#endif

#ifdef SUPLA_CONFIG
  int _cfgLedGpio[17];
  int _cfgLedLevel[17];
  int _cfgButtonGpio[17];
  int _cfgButtonFlag[17];
#endif

  ETSTimer led_timer;
};

void ledBlinking_func(void *timer_arg);
void status_func(int status, const char *msg);

#endif // SuplaConfigESP_h
