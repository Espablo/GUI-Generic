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

#include <esp_spiffs.h>
#include <string.h>
#include <supla-common/log.h>

#include <cstdlib>
#include <stdio.h>

#include "spiffs_storage.h"

using namespace Supla;

// By default, update storage every 3 min
// Storage is not saved if there is no change
#define SUPLA_WRITING_PERIOD 3 * 60 * 1000

SpiffsStorage::SpiffsStorage(uint32_t storageSize)
  : Storage(0), bufferSize(storageSize) {
    setStateSavePeriod((unsigned long)SUPLA_WRITING_PERIOD);

    buffer = (char *)malloc(bufferSize);
    memset(buffer, 0, bufferSize);
  }

SpiffsStorage::~SpiffsStorage() {
  if (buffer) {
    free(buffer);
  }
  esp_vfs_spiffs_unregister(NULL);
}

bool SpiffsStorage::init() {
  if (!buffer) {
    supla_log(LOG_ERR, "Storage: failed to allocate storage memory");
    return false;
  }
  // init spiffs
  esp_vfs_spiffs_conf_t conf = {.base_path = "/spiffs",
    .partition_label = NULL,
    .max_files = 5,
    .format_if_mount_failed = true};

  esp_err_t ret = esp_vfs_spiffs_register(&conf);

  if (ret != ESP_OK) {
    if (ret == ESP_FAIL) {
      supla_log(LOG_WARNING, "Failed to mount or format filesystem");
    } else if (ret == ESP_ERR_NOT_FOUND) {
      supla_log(LOG_ERR, "Failed to find SPIFFS partition");
    } else {
      supla_log(
          LOG_ERR, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
    }
    return false;
  }

  size_t total = 0;
  size_t used = 0;
  ret = esp_spiffs_info(conf.partition_label, &total, &used);
  if (ret != ESP_OK) {
    supla_log(LOG_ERR,
        "Failed to get SPIFFS partition information (%s)",
        esp_err_to_name(ret));
  } else {
    supla_log(LOG_INFO, "Partition size: total: %d, used: %d", total, used);
  }

  FILE *file = fopen("/spiffs/storage.bin", "rb");

  if (file) {
    fseek(file, 0, SEEK_END);
    uint32_t fileSize = ftell(file);
    rewind(file);

    if (fileSize != bufferSize) {
      supla_log(
          LOG_WARNING,
          "Storage: storage file size differs from configured buffer size. "
          "Rewritting...");
    } else {
      supla_log(LOG_DEBUG, "Storage: reading file to internal buffer");
      fread(buffer, 1, bufferSize, file);
    }

    fclose(file);
  }

  return Storage::init();
}

int SpiffsStorage::readStorage(unsigned int offset,
    unsigned char *buf,
    int size,
    bool logs) {
  if (!buffer) {
    return 0;
  }

  if (offset + size > bufferSize) {
    supla_log(LOG_ERR, "Storage: attempt to read out of storage size");
    return 0;
  }

  for (int i = 0; i < size; i++) {
    buf[i] = buffer[offset + i];
  }
  return size;
}

int SpiffsStorage::writeStorage(unsigned int offset,
    const unsigned char *buf,
    int size) {
  dataChanged = true;

  if (offset + size > bufferSize) {
    supla_log(LOG_ERR, "Storage: attempt to write out of storage size");
    return 0;
  }

  for (int i = 0; i < size; i++) {
    buffer[offset + i] = buf[i];
  }

  return size;
}

void SpiffsStorage::commit() {
  if (dataChanged) {
    // write to file
    supla_log(LOG_DEBUG, "Storage: commit");
    FILE *file = fopen("/spiffs/storage.bin", "wb");
    if (file) {
      uint32_t bytesWritten = fwrite(buffer, 1, bufferSize, file);
      if (bufferSize != bytesWritten) {
        supla_log(LOG_ERR, "Storage: file write error");
      } else {
        supla_log(LOG_DEBUG, "Storage: file saved");
      }

      fclose(file);
    } else {
      supla_log(LOG_ERR, "Storage: failed to open storage file");
    }
  }
  dataChanged = false;
}
