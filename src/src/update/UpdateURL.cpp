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
#include "UpdateURL.h"
#include "../../SuplaDeviceGUI.h"

UpdateURL::UpdateURL(const String& url) {
  parseURL = new ParseURL(url);
}

int UpdateURL::update() {
  if (!WebServer->isLoggedIn()) {
    return HTTP_UPDATE_FAILED;
  }

  WiFiClient client;

  Serial.print("connecting to ");
  Serial.print(parseURL->getHost());
  Serial.print(":");
  Serial.println(parseURL->getPort());

  if (!client.connect(parseURL->getHost().c_str(), parseURL->getPort())) {
    Serial.println("connection failed");
    return HTTP_UPDATE_FAILED;
  }

  Serial.print("Starting OTA from: ");
  Serial.println(parseURL->getPath());

#ifdef ARDUINO_ARCH_ESP8266
  ESPhttpUpdate.rebootOnUpdate(false);
  ESPhttpUpdate.setLedPin(ConfigESP->getGpio(FUNCTION_CFG_LED), ConfigESP->getLevel(ConfigESP->getGpio(FUNCTION_CFG_LED)));
  ESPhttpUpdate.closeConnectionsOnUpdate(false);

  auto ret = ESPhttpUpdate.update(client, parseURL->getHost().c_str(), parseURL->getPort(), parseURL->getPath().c_str());
#elif ARDUINO_ARCH_ESP32
  httpUpdate.rebootOnUpdate(false);
  httpUpdate.setLedPin(ConfigESP->getGpio(FUNCTION_CFG_LED), ConfigESP->getLevel(ConfigESP->getGpio(FUNCTION_CFG_LED)));
  auto ret = httpUpdate.update(client, parseURL->getHost().c_str(), parseURL->getPort(), parseURL->getPath().c_str());
#endif

  client.stop();

  return ret;
}
