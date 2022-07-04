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

#ifndef EXTRAS_PORTING_ESP_IDF_SPIFFS_STORAGE_H_
#define EXTRAS_PORTING_ESP_IDF_SPIFFS_STORAGE_H_

#include <supla/storage/storage.h>

namespace Supla {

class SpiffsStorage : public Storage {
 public:
  explicit SpiffsStorage(uint32_t size = 512);
  virtual ~SpiffsStorage();
  bool init();
  void commit();

 protected:
  int readStorage(unsigned int, unsigned char *, int, bool);
  int writeStorage(unsigned int, const unsigned char *, int);

  bool dataChanged = false;
  char *buffer = nullptr;
  uint32_t bufferSize = 0;
};

};  // namespace Supla

#endif  // EXTRAS_PORTING_ESP_IDF_SPIFFS_STORAGE_H_
