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
#ifndef _bone_IO_h
#define _bone_IO_h

#if defined(SUPLA_SUPLA_BONEIO_32x10A) || defined(SUPLA_SUPLA_BONEIO_24x16A)
#define SUPLA_BONEIO
#endif

#if defined(SUPLA_MCP23017)
#define USE_MCP_OUTPUT
#endif

#if defined(SUPLA_ROLLERSHUTTER)
#define BONEIO_ROLLER_SHUTTER_CHANNEL
#else
#define BONEIO_RELAY_CHANNEL
#endif

#include <Arduino.h>
#include "src/boneIO/Common.h"
#include "src/sensor/LM75.h"
#include "../../SuplaDeviceGUI.h"
#include "../expander/ExpanderMCP23017.h"
#include "../expander/ExpanderPCF8574.h"
#include "../expander/ExpanderPCF8575.h"
#include "display/SSD1306.h"

namespace Supla {

struct DevicePin {
  Supla::Io *io;
  int pin;
};

struct RelayButtonPair {
  Supla::Control::Relay *relay;
  Supla::Control::Button *button;
};

struct RollerShutterButtonPair {
  Supla::Control::RollerShutter *shutter;
  Supla::Control::Button *openButton;
  Supla::Control::Button *closeButton;
};

class boneIO {
 public:
  boneIO();

 private:
  RollerShutterButtonPair
  createRollerShutterChannel(DevicePin shutterRelayUpPin, DevicePin shutterRelayDownPin, DevicePin openButtonPin, DevicePin closeButtonPin);
  RelayButtonPair createRelayChannel(DevicePin relayPin, DevicePin buttonPin, bool highIsOn);
};

};  // namespace Supla

#endif
