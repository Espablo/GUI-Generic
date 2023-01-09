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

UpdateURL::UpdateURL(const String& url) : url(url) {
}

int UpdateURL::update() {
  if (!WebServer->isLoggedIn()) {
    return HTTP_UPDATE_FAILED;
  }

  parseURL(&_url);

  WiFiClient client;

  Serial.print("connecting to ");
  Serial.println(_url.host.c_str());
  Serial.println(_url.port.toInt());
  Serial.println(_url.version.c_str());

  if (!client.connect(_url.host.c_str(), _url.port.toInt())) {
    Serial.println("connection failed");
    return HTTP_UPDATE_FAILED;
  }

  Serial.print("Starting OTA from: ");
  Serial.println(_url.path.c_str());

  ESPhttpUpdate.rebootOnUpdate(false);
  ESPhttpUpdate.closeConnectionsOnUpdate(false);
  ESPhttpUpdate.setLedPin(ConfigESP->getGpio(FUNCTION_CFG_LED), ConfigESP->getLevel(ConfigESP->getGpio(FUNCTION_CFG_LED)));
  auto ret = ESPhttpUpdate.update(client, _url.host.c_str(), _url.port.toInt(), _url.path.c_str());

  return ret;
}

void UpdateURL::parseURL(URL* url) {
  // Assume a valid URL
  enum URLParseState
  {
    PROTOCOL,
    SEPERATOR,
    HOST,
    PORT,
    PATH,
    VERSION,
  } state = PROTOCOL;

  url->protocol = "";
  url->host = "";
  url->port = "";
  url->path = "/";
  url->version = "";

  for (unsigned int i = 0; i < this->url.length(); i++) {
    switch (state) {
      case PROTOCOL:
        if (this->url[i] == ':')
          state = SEPERATOR;
        else
          url->protocol += this->url[i];
        break;
      case SEPERATOR:
        if (this->url[i] != '/') {
          state = HOST;
          url->host += this->url[i];
        }
        break;
      case HOST:
        if (this->url[i] == ':')
          state = PORT;
        else {
          if (this->url[i] == '/')
            state = PATH;
          else
            url->host += this->url[i];
        }
        break;
      case PORT:
        if (this->url[i] == '/')
          state = PATH;
        else
          url->port += this->url[i];
        break;

      case VERSION:
        url->path += this->url[i];

        if (this->url[i] == '/')
          state = PATH;
        else
          url->version += this->url[i];
        break;

      case PATH:
        url->path += this->url[i];

        if (this->url[i - 1] == 'g' && this->url[i] == 'v') {
          state = VERSION;
        }
    }
  }

  if (url->port.isEmpty()) {
    if (url->protocol == "http" || url->protocol == "https") {
      url->port = "80";
    }
    // if (url->protocol == "https") {
    //   url->port = "443";
    // }
  }
}
