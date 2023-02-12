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

#ifndef CONFIG_EXPANDER_PCF8575_H_
#define CONFIG_EXPANDER_PCF8575_H_

#include <supla/io.h>

namespace Supla {
namespace Control {
class ConfigExpander {
 public:
  ConfigExpander();
  bool checkBusyGpioExpander(uint8_t gpio, uint8_t nr, uint8_t function);
  uint8_t getGpioExpander(uint8_t nr, uint8_t function);
  uint8_t getAdressExpander(uint8_t nr, uint8_t function);
  void setGpioExpander(uint8_t gpio, uint8_t adress, uint8_t nr, uint8_t function);
  void clearGpioExpander(uint8_t gpio, uint8_t nr, uint8_t function);
  void clearFunctionGpioExpander(uint8_t function);

  bool checkActiveExpander(uint8_t function);
  uint8_t getFunctionExpander(uint8_t adress);
  uint8_t getNrExpander(uint8_t adress);
  Supla::Io *getIoExpender(uint8_t nr, uint8_t function);

 protected:
  struct IoExpender {
    Supla::Io *io = {nullptr};
#ifdef ARDUINO_ARCH_ESP32
    Supla::Io *io_Wire1 = {nullptr};
#endif
  };
  
  IoExpender ioExpender[3];
};
};  // namespace Control
};  // namespace Supla

#endif
