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

#ifndef SuplaDeviceGUI_h
#define SuplaDeviceGUI_h

#include "GUI-Generic_Config.h"

#include <DoubleResetDetector.h>
#include <SPI.h>

#include <SuplaDevice.h>

#include "SuplaConfigESP.h"
#include "SuplaConfigManager.h"
#include "SuplaWebServer.h"
#include "SuplaWebPageRelay.h"

#include "SuplaWebPageDownload.h"
#include "SuplaWebPageUpload.h"
#include "SuplaWebPageTools.h"
#include "GUIGenericCommon.h"
#include "SuplaCommonPROGMEM.h"
#include "Markup.h"
#include "SuplaOled.h"

#include <vector>

#include <supla/control/button.h>
#include <supla/control/relay.h>
#include <supla/control/roller_shutter.h>

#include "SuplaSensorDS18B20.h"
#include <supla/sensor/DHT.h>
#include <supla/sensor/HC_SR04.h>
#include <supla/sensor/binary.h>
#ifdef SUPLA_BME280
#include <supla/sensor/BME280.h>
#include "SuplaWebPageSensor.h"
#endif
#ifdef SUPLA_SI7021_SONOFF
#include <supla/sensor/Si7021_sonoff.h>
#endif
#ifdef SUPLA_BME280
#include <supla/sensor/BME280.h>
#include "SuplaWebPageSensor.h"
#endif
#ifdef SUPLA_SHT3x
#include <supla/sensor/SHT3x.h>
#endif
#ifdef SUPLA_SI7021
#include <supla/sensor/Si7021.h>
#endif
#ifdef SUPLA_MAX6675
#include <supla/sensor/MAX6675_K.h>
#endif
#ifdef SUPLA_IMPULSE_COUNTER
#include <supla/sensor/impulse_counter.h>
#endif
#ifdef DEBUG_MODE
#include <supla/sensor/esp_free_heap.h>
#endif

namespace Supla {
namespace GUI {

void begin();

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
void addRelayButton(int pinRelay, int pinButton, bool highIsOn = true);

extern std::vector<Supla::Control::Relay *> relay;
extern std::vector<Supla::Control::Button *> button;
#endif

#ifdef SUPLA_DS18B20
void addDS18B20MultiThermometer(int pinNumber);

extern std::vector<DS18B20 *> sensorDS;
#endif

#ifdef SUPLA_CONFIG
void addConfigESP(int pinNumberConfig, int pinLedConfig, int modeConfigButton, bool highIsOn);
#endif

#ifdef SUPLA_ROLLERSHUTTER
void addRolleShutter(int pinRelayUp, int pinRelayDown, int pinButtonUp, int pinButtonDown, bool highIsOn = true);
void addRolleShutterMomentary(int pinRelayUp, int pinRelayDown, int pinButtonUp, int pinButtonDown, bool highIsOn = true);

extern std::vector<Supla::Control::RollerShutter *> RollerShutterRelay;
extern std::vector<Supla::Control::Button *> RollerShutterButtonOpen;
extern std::vector<Supla::Control::Button *> RollerShutterButtonClose;
#endif

#ifdef SUPLA_IMPULSE_COUNTER
extern std::vector<Supla::Sensor::ImpulseCounter *> impulseCounter;
void addImpulseCounter(int pin, bool lowToHigh, bool inputPullup, unsigned int debounceDelay);
#endif

#ifdef SUPLA_BME280
extern std::vector<Supla::Sensor::BME280 *> sensorBme280;
#endif

#ifdef SUPLA_SI7021_SONOFF
extern std::vector<Supla::Sensor::Si7021Sonoff *> sensorSi7021Sonoff;
#endif

#ifdef SUPLA_DHT22
extern std::vector<Supla::Sensor::DHT *> sensorDHT22;
#endif

#ifdef SUPLA_MAX6675
extern std::vector<Supla::Sensor::MAX6675_K *> sensorMAX6675_K;
#endif

};  // namespace GUI
};  // namespace Supla

extern SuplaConfigManager *ConfigManager;
extern SuplaConfigESP *ConfigESP;
extern SuplaWebServer *WebServer;

#endif  // SuplaDeviceGUI_h
