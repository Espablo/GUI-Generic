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

#include <cJSON.h>
#include <ctype.h>
#include <esp_http_client.h>
#include <esp_idf_ota.h>
#include <esp_ota_ops.h>
#include <esp_task_wdt.h>
#include <stdio.h>
#include <supla-common/log.h>
#include <supla/rsa_verificator.h>
#include <supla/sha256.h>
#include <supla/time.h>
#include <supla/tools.h>

#include <cerrno>

#include "supla/device/sw_update.h"

#define BUFFER_SIZE 4096

Supla::Device::SwUpdate *Supla::Device::SwUpdate::Create(SuplaDeviceClass *sdc,
                                                         const char *newUrl) {
  return new Supla::EspIdfOta(sdc, newUrl);
}

Supla::EspIdfOta::EspIdfOta(SuplaDeviceClass *sdc, const char *newUrl)
    : Supla::Device::SwUpdate(sdc, newUrl) {
}

void Supla::EspIdfOta::start() {
  Supla::Device::SwUpdate::start();
  finished = true;

#define URL_SIZE 512
#define BUF_SIZE 512

  char urlWithParams[URL_SIZE] = {};
  char buf[BUF_SIZE] = {};
  strncpy(urlWithParams, url, URL_SIZE - 1);
  int curPos = strlen(urlWithParams);

  int v = 0;

  while (1) {
    v = stringAppend(
        urlWithParams + curPos, "/check_update?name=", URL_SIZE - curPos - 1);
    if (v == 0) break;
    curPos += v;

    urlEncode(Supla::Channel::reg_dev.Name, buf, BUF_SIZE);
    v = stringAppend(urlWithParams + curPos, buf, URL_SIZE - curPos - 1);
    if (v == 0) break;
    curPos += v;

    v = stringAppend(urlWithParams + curPos, "&mfr=", URL_SIZE - curPos - 1);
    if (v == 0) break;
    curPos += v;

    snprintf(buf, sizeof(buf), "%d", Supla::Channel::reg_dev.ManufacturerID);
    v = stringAppend(urlWithParams + curPos, buf, URL_SIZE - curPos - 1);
    if (v == 0) break;
    curPos += v;

    v = stringAppend(urlWithParams + curPos, "&prod=", URL_SIZE - curPos - 1);
    if (v == 0) break;
    curPos += v;

    snprintf(buf, sizeof(buf), "%d", Supla::Channel::reg_dev.ProductID);
    v = stringAppend(urlWithParams + curPos, buf, URL_SIZE - curPos - 1);
    if (v == 0) break;
    curPos += v;

    v = stringAppend(
        urlWithParams + curPos, "&version=", URL_SIZE - curPos - 1);
    if (v == 0) break;
    curPos += v;

    urlEncode(Supla::Channel::reg_dev.SoftVer, buf, BUF_SIZE);
    v = stringAppend(urlWithParams + curPos, buf, URL_SIZE - curPos - 1);
    if (v == 0) break;
    curPos += v;

    v = stringAppend(urlWithParams + curPos, "&sguid=", URL_SIZE - curPos - 1);
    if (v == 0) break;
    curPos += v;

    {
      Supla::Sha256 hash;
      hash.update(reinterpret_cast<uint8_t *>(Supla::Channel::reg_dev.GUID),
                  SUPLA_GUID_SIZE);

      uint8_t sha[32] = {};
      hash.digest(sha);

      if (curPos < URL_SIZE - 1 - 32 * 2) {
        curPos += generateHexString(sha, urlWithParams + curPos, 32);
      } else {
        v = 0;
        break;
      }
    }

    if (strlen(Supla::Channel::reg_dev.Email) > 0) {
      v = stringAppend(
          urlWithParams + curPos, "&smail=", URL_SIZE - curPos - 1);
      if (v == 0) break;
      curPos += v;

      Supla::Sha256 hash;
      strncpy(buf, Supla::Channel::reg_dev.Email, BUF_SIZE);
      auto bufPtr = buf;
      int size = 0;
      while (*bufPtr) {
        *bufPtr = tolower(*bufPtr);
        bufPtr++;
        size++;
      }

      hash.update(reinterpret_cast<uint8_t *>(buf), size);

      uint8_t sha[32] = {};
      hash.digest(sha);

      if (curPos < URL_SIZE - 1 - 32 * 2) {
        curPos += generateHexString(sha, urlWithParams + curPos, 32);
      } else {
        v = 0;
        break;
      }
    }

    if (beta) {
      v = stringAppend(
          urlWithParams + curPos, "&beta=1", URL_SIZE - curPos - 1);
      if (v == 0) break;
      curPos += v;
    }
    break;
  }

  if (v == 0) {
    fail("SW update: fail - too long request url");
    return;
  }

  esp_http_client_config_t configCheckUpdate = {};
  configCheckUpdate.url = urlWithParams;
  configCheckUpdate.timeout_ms = 5000;

  client = esp_http_client_init(&configCheckUpdate);
  if (client == NULL) {
    fail("SW update: failed initialize connection with update server");
    return;
  }
  esp_err_t err;
  err = esp_http_client_open(client, 0);
  if (err != ESP_OK) {
    fail("SW update: failed to open connection with update server");
    return;
  }
  esp_http_client_fetch_headers(client);

  // Start fetching bin file and perform update
  const esp_partition_t *updatePartition = NULL;

  supla_log(LOG_DEBUG, "Starting OTA");

  otaBuffer = new uint8_t[BUFFER_SIZE + 1];
  if (otaBuffer == nullptr) {
    fail("SW udpate: failed to allocate memory");
    return;
  }

  while (true) {
    int dataRead = esp_http_client_read(
        client, reinterpret_cast<char *>(otaBuffer), BUFFER_SIZE);
    if (dataRead < 0) {
      fail("SW update: data read error");
      return;
    } else if (dataRead > 0) {
      otaBuffer[dataRead] = '\0';
      supla_log(LOG_DEBUG, "Read: %s", otaBuffer);
    } else if (dataRead == 0) {
      if (errno == ECONNRESET || errno == ENOTCONN) {
        supla_log(LOG_DEBUG, "Connection closed, errno = %d", errno);
        break;
      }
      if (esp_http_client_is_complete_data_received(client) == true) {
        break;
      }
    }
  }

  if (esp_http_client_is_complete_data_received(client) != true) {
    fail("SW update: error in receiving check update response");
    return;
  }
  cJSON *json = cJSON_Parse(reinterpret_cast<const char *>(otaBuffer));

  if (json == nullptr) {
    fail("SW update: failed to parse check update response");
    return;
  }
  cJSON *status = cJSON_GetObjectItemCaseSensitive(json, "status");
  cJSON *version = cJSON_GetObjectItemCaseSensitive(json, "version");
  cJSON *url = cJSON_GetObjectItemCaseSensitive(json, "url");
  if (cJSON_IsString(status) && (status->valuestring != NULL)) {
    snprintf(buf, BUF_SIZE, "SW update status: %s", status->valuestring);
    supla_log(LOG_DEBUG, "%s", buf);
    log(buf);
  }
  if (cJSON_IsString(version) && (version->valuestring != NULL) &&
      cJSON_IsString(url) && (url->valuestring != NULL)) {
    snprintf(buf, BUF_SIZE, "SW update new version: %s", version->valuestring);
    supla_log(LOG_DEBUG, "%s", buf);
    log(buf);
    snprintf(buf, BUF_SIZE, "SW update url: \"%s\"", url->valuestring);
    supla_log(LOG_DEBUG, "%s", buf);
    log(buf);
    esp_http_client_set_url(client, url->valuestring);
  } else {
    fail("SW update: no url and version received - finishing");
    return;
  }

  err = esp_http_client_open(client, 0);
  cJSON_Delete(json);
  if (err != ESP_OK) {
    fail("SW update: failed to open HTTP connection");
    return;
  }
  esp_http_client_fetch_headers(client);

  updatePartition = esp_ota_get_next_update_partition(NULL);
  if (updatePartition == NULL) {
    fail("SW update: failed to get next update partition");
    return;
  }
  supla_log(LOG_DEBUG,
            "Used ota partition subtype %d, offset 0x%x",
            updatePartition->subtype,
            updatePartition->address);

  int binSize = 0;

  err = esp_ota_begin(updatePartition, OTA_SIZE_UNKNOWN, &updateHandle);
  if (err != ESP_OK) {
    fail("SW update: OTA begin failed");
    return;
  }

  supla_log(LOG_DEBUG, "Getting file from server...");
  while (true) {
    int dataRead = esp_http_client_read(
        client, reinterpret_cast<char *>(otaBuffer), BUFFER_SIZE);
    if (dataRead < 0) {
      fail("SW update: data read error");
      return;
    } else if (dataRead > 0) {
      err = esp_ota_write(updateHandle, (const void *)otaBuffer, dataRead);
      if (err == ESP_ERR_OTA_VALIDATE_FAILED) {
        fail("SW update: image corrupted - invalid magic byte");
        return;
      }
      if (err != ESP_OK) {
        fail("SW update: flash write fail");
        return;
      }
      binSize += dataRead;
    } else if (dataRead == 0) {
      if (errno == ECONNRESET || errno == ENOTCONN) {
        supla_log(LOG_DEBUG, "Connection closed, errno = %d", errno);
        break;
      }
      if (esp_http_client_is_complete_data_received(client) == true) {
        break;
      }
    }
  }
  supla_log(LOG_DEBUG, "Download complete. Wrote %d bytes", binSize);
  if (esp_http_client_is_complete_data_received(client) != true) {
    fail("SW update: error in receiving complete file");
    return;
  }

  esp_http_client_cleanup(client);
  client = 0;

  err = esp_ota_end(updateHandle);
  updateHandle = 0;

  if (err != ESP_OK) {
    if (err == ESP_ERR_OTA_VALIDATE_FAILED) {
      fail("SW update: image validation failed - image is corrupted");
    } else {
      fail("SW update: OTA end failed");
    }
    return;
  }

  // If we are here, then file was correctly downloaded and stored on flash.
  // We apply here additional RSA signature check added to Supla firmware

  if (!verifyRsaSignature(updatePartition, binSize)) {
    fail("SW update: RSA signature verification failed");
    return;
  }

  // RSA signature check done
  err = esp_ota_set_boot_partition(updatePartition);
  if (err != ESP_OK) {
    fail("SW update: failed to set boot partition");
    return;
  }
  delete[] otaBuffer;
  otaBuffer = nullptr;
  return;
}

void Supla::EspIdfOta::iterate() {
}

bool Supla::EspIdfOta::isFinished() {
  return finished;
}

#define RSA_FOOTER_SIZE 16
bool Supla::EspIdfOta::verifyRsaSignature(
    const esp_partition_t *updatePartition, int binSize) {
  uint8_t footer[RSA_FOOTER_SIZE] = {};
  // footer contain some information, however supla-esp-signtool is able to
  // produce only one footer, so here we hardcode it. Please check
  // supla-esp-signtool for details.
  const uint8_t expectedFooter[RSA_FOOTER_SIZE] = {0xBA,
                                                   0xBE,
                                                   0x2B,
                                                   0xED,
                                                   0x00,
                                                   0x01,
                                                   0x02,
                                                   0x00,
                                                   0x10,
                                                   0x00,
                                                   0x00,
                                                   0x00,
                                                   0x00,
                                                   0x00,
                                                   0x00,
                                                   0x00};

  Supla::Sha256 hash;

  if (binSize < RSA_FOOTER_SIZE + RSA_NUM_BYTES + 1) {
    supla_log(LOG_DEBUG, "Fail: bin size too small");
    return false;
  }

  int appSize = binSize - RSA_FOOTER_SIZE - RSA_NUM_BYTES;

  for (int i = 0; i < appSize; i += BUFFER_SIZE) {
    int sizeToRead = BUFFER_SIZE;
    if (i + sizeToRead > appSize) {
      sizeToRead = appSize - i;
    }
    esp_err_t err =
        esp_partition_read(updatePartition, i, otaBuffer, sizeToRead);

    if (err != ESP_OK) {
      supla_log(LOG_DEBUG, "Fail: error reading app");
      return false;
    }

    hash.update(otaBuffer, sizeToRead);
  }

  esp_err_t err = esp_partition_read(
      updatePartition, binSize - RSA_FOOTER_SIZE, footer, RSA_FOOTER_SIZE);

  if (err != ESP_OK) {
    supla_log(LOG_DEBUG, "Fail: error reading footer");
    return false;
  }

  if (memcmp(footer, expectedFooter, RSA_FOOTER_SIZE) != 0) {
    supla_log(LOG_DEBUG, "Fail: invalid footer");
    return false;
  }

  err = esp_partition_read(updatePartition,
                           binSize - RSA_FOOTER_SIZE - RSA_NUM_BYTES,
                           otaBuffer,
                           RSA_NUM_BYTES);

  if (err != ESP_OK) {
    supla_log(LOG_DEBUG, "Fail: error reading signature");
    return false;
  }

  if (sdc && sdc->getRsaPublicKey()) {
    Supla::RsaVerificator rsa(sdc->getRsaPublicKey());
    if (rsa.verify(&hash, otaBuffer)) {
      supla_log(LOG_DEBUG, "RSA signature verification successful");
      return true;
    } else {
      supla_log(LOG_DEBUG, "Fail: Incorrect RSA signature");
      return false;
    }
  } else {
    supla_log(LOG_DEBUG,
              "Fail: RSA public key not set in SuplaDevice instance");
    return false;
  }
  return false;
}

bool Supla::EspIdfOta::isAborted() {
  return abort;
}

void Supla::EspIdfOta::fail(const char *reason) {
  if (otaBuffer) {
    delete[] otaBuffer;
    otaBuffer = nullptr;
  }

  if (client) {
    esp_http_client_cleanup(client);
    client = 0;
  }
  if (updateHandle) {
    esp_ota_end(updateHandle);
    updateHandle = 0;
  }

  log(reason);
  abort = true;
}

void Supla::EspIdfOta::log(const char *value) {
  if (sdc) {
    sdc->addLastStateLog(value);
  }
}
