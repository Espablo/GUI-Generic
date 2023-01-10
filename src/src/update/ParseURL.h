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
#ifndef _PARSER_URL_H
#define _PARSER_URL_H
#include <Arduino.h>

class ParseURL {
  struct URL {
    String protocol = "";
    String host = "";
    String port = "";
    String path = "";
  } _url;

 public:
  ParseURL(const String& url);

  const String& getProtocol() {
    return _url.protocol;
  }

  const String& getHost() {
    return _url.host;
  }

  uint16_t getPort() {
    return _url.port.toInt();
  }

  const String& getPath() {
    return _url.path;
  }

 private:
  String url;
  void parse(URL* url);
};
#endif