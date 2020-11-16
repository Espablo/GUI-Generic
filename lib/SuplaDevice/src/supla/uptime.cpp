/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

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

#include "uptime.h"

namespace Supla {

Uptime::Uptime()
    : deviceUptime(0),
      connectionUptime(0),
      lastConnectionResetCause(SUPLA_LASTCONNECTIONRESETCAUSE_UNKNOWN),
      lastMillis(0),
      acceptConnectionLostCause(false) {
}

void Uptime::iterate(unsigned long millis) {
  int seconds = (millis - lastMillis) / 1000;
  if (seconds > 0) {
    lastMillis = millis - ((millis - lastMillis) % 1000);
    deviceUptime += seconds;
    connectionUptime += seconds;
  }
}

void Uptime::resetConnectionUptime() {
  connectionUptime = 0;
  acceptConnectionLostCause = true;
}

void Uptime::setConnectionLostCause(unsigned char cause) {
  if (acceptConnectionLostCause) {
    lastConnectionResetCause = cause;
    acceptConnectionLostCause = false;
  }
}

unsigned _supla_int_t Uptime::getUptime() {
  return deviceUptime;
}

unsigned _supla_int_t Uptime::getConnectionUptime() {
  return connectionUptime;
}

unsigned char Uptime::getLastResetCause() {
  return lastConnectionResetCause;
}


};  // namespace Supla
