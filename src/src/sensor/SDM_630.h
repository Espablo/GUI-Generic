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

#ifndef _Three_Phase_SDM630_h
#define _Three_Phase_SDM630_h

#include <Arduino.h>
#include <SDM.h>
#include <supla/sensor/electricity_meter.h>

#include "SDM_ReadValues.h"

namespace Supla {
namespace Sensor {

class SDM630 : public ElectricityMeter {
 public:
  SDM630(int8_t pinRX = SDM_RX_PIN, int8_t pinTX = SDM_TX_PIN);
  void onInit();
  virtual void readValuesFromDevice();

 protected:
  ReadValuesSDM sdm;
};

};  // namespace Sensor
};  // namespace Supla

#endif
