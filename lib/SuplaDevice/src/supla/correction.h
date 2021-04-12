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

#ifndef __correction_h
#define __correction_h

#include <stdint.h>

namespace Supla {

  class Correction {
    public:
      static void add(uint8_t channelNumber, double correction, bool forSecondaryValue = false);
      static double get(uint8_t channelNumber, bool forSecondaryValue = false);
      static void clear();

    protected:
      Correction(uint8_t channelNumber, double correction, bool forSecondaryValue);
      ~Correction();

      static Correction *first;
      Correction *next;
      uint8_t channelNumber;
      double correction;
      bool forSecondaryValue;
  };

};
#endif
