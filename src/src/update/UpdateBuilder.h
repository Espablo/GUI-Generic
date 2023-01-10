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
#ifndef _UPDATE_BUILDER_H
#define _UPDATE_BUILDER_H

#include <Arduino.h>

#include "ParseURL.h"

// NONE - jeśli wersja modułu jest zgodna z wersją GG buildera (brak nowszej wersji,)
// WAIT - jeśli wersja modułu jest inna niż wersja GG buildera, ale builder nie ma jeszcze skompilowanego nowego GG; to inicjuje skompilowanie nowej
// wersji GG, która powinna być dostępna maksymalnie po 5 minutach
// READY - jeśli wersja modułu jest inna niż wersja GG buildera, ale builder ma gotowy skompilowany nowy GG i można go pobrać przez GET
// UNKNOWN - jeśli builder nie zna jeszcze takiego hasha.
enum UpdateBuilderResult
{
  BUILDER_UPDATE_FAILED,
  BUILDER_UPDATE_NO_UPDATES,
  BUILDER_UPDATE_WAIT,
  BUILDER_UPDATE_READY,
};

class UpdateBuilder {
 public:
  UpdateBuilder(const String& url);
  int check();

 private:
  ParseURL* parseURL;
};
#endif