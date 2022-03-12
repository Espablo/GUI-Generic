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

#ifndef _definitions_h_
#define _definitions_h_

#ifdef ARDUINO
#include <Arduino.h>
#endif

#ifndef LSBFIRST
#define LSBFIRST 0
#endif /*LSBFIRST*/

#ifndef LOW
#define LOW 0
#endif /*LOW*/

#ifndef HIGH
#define HIGH 1
#endif /*HIGH*/

#ifndef INPUT
#define INPUT 0
#endif /*INPUT*/

#ifndef INPUT_PULLUP
#define INPUT_PULLUP 2
#endif /*INPUT_PULLUP*/

#ifndef OUTPUT
#define OUTPUT 1
#endif /*OUTPUT*/

#endif /*_definitions_h_*/
