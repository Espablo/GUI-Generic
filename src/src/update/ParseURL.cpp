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

#include "ParseURL.h"

ParseURL::ParseURL(const String& url) : url(url) {
  parse(&_url);
}

void ParseURL::parse(URL* url) {
  enum URLParseState
  {
    PROTOCOL,
    SEPERATOR,
    HOST,
    PORT,
    PATH,
  } state = PROTOCOL;

  url->protocol = "";
  url->host = "";
  url->port = "";
  url->path = "/";

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

      case PATH:
        url->path += this->url[i];
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

  url->protocol.c_str();
  url->host.c_str();
  url->port.c_str();
  url->path.c_str();
}