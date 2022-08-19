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
#include <string.h>
#include <stdio.h>
#include <supla/time.h>

#include <SuplaDevice.h>
#include <supla/log_wrapper.h>
#include <supla/protocol/protocol_layer.h>

#include "supla/element.h"
#include "supla/network/network.h"
#include "supla/storage/config.h"

namespace Supla {

Network *Network::netIntf = nullptr;

Network *Network::Instance() {
  return netIntf;
}

bool Network::Connected() {
  if (Instance() != nullptr) {
    return Instance()->connected();
  }
  return false;
}

int Network::Read(void *buf, int count) {
  if (Instance() != nullptr) {
    return Instance()->read(buf, count);
  }
  return -1;
}

int Network::Write(void *buf, int count) {
  if (Instance() != nullptr) {
    return Instance()->write(buf, count);
  }
  return -1;
}

int Network::Connect(const char *server, int port) {
  if (Instance() != nullptr) {
    Instance()->clearTimeCounters();
    return Instance()->connect(server, port);
  }
  return 0;
}

void Network::Disconnect() {
  for (auto proto = Supla::Protocol::ProtocolLayer::first(); proto != nullptr;
      proto = proto->next()) {
    proto->disconnect();
  }
  if (Instance() != nullptr) {
    return Instance()->disconnect();
  }
  return;
}

void Network::Setup() {
  if (Instance() != nullptr) {
    return Instance()->setup();
  }
  return;
}

void Network::Uninit() {
  if (Instance() != nullptr) {
    return Instance()->uninit();
  }
  return;
}

bool Network::IsReady() {
  if (Instance() != nullptr) {
    return Instance()->isReady();
  }
  return false;
}

bool Network::Iterate() {
  if (Instance() != nullptr) {
    return Instance()->iterate();
  }
  return false;
}

void Network::SetConfigMode() {
  if (Instance() != nullptr) {
    Instance()->setConfigMode();
  }
  return;
}

void Network::SetNormalMode() {
  if (Instance() != nullptr) {
    Instance()->setNormalMode();
  }
  return;
}

bool Network::GetMacAddr(uint8_t *buf) {
  if (Instance() != nullptr) {
    return Instance()->getMacAddr(buf);
  }
  return false;
}

void Network::SetHostname(const char *buf) {
  if (Instance() != nullptr) {
    Instance()->setHostname(buf);
  }
  return;
}


Network::Network(unsigned char *ip) {
  netIntf = this;

  if (ip == nullptr) {
    useLocalIp = false;
  } else {
    useLocalIp = true;
    memcpy(localIp, ip, 4);
  }
}

Network::~Network() {
  netIntf = nullptr;
}

bool Network::iterate() {
  return false;
}

void Network::clearTimeCounters() {
}

void Network::setTimeout(int timeoutMs) {
  (void)(timeoutMs);
  SUPLA_LOG_DEBUG("setTimeout is not implemented for this interface");
}

void Network::fillStateData(TDSC_ChannelState *channelState) {
  (void)(channelState);
  SUPLA_LOG_DEBUG("fillStateData is not implemented for this interface");
}

#ifdef ARDUINO
#define TMP_STRING_SIZE 1024
#else
#define TMP_STRING_SIZE 2048
#endif

void Network::printData(const char *prefix, const void *buf, const int count) {
  char tmp[TMP_STRING_SIZE] = {};
  for (int i = 0; i < count && ((i + 1) * 3 < TMP_STRING_SIZE - 1); i++) {
    snprintf(
        tmp + i * 3, 4,
        "%02X ",
        static_cast<unsigned int>(static_cast<const unsigned char *>(buf)[i]));
  }
  SUPLA_LOG_DEBUG("%s: [%s]", prefix, tmp);
}

void Network::setSsid(const char *wifiSsid) {
  (void)(wifiSsid);
}

void Network::setPassword(const char *wifiPassword) {
  (void)(wifiPassword);
}

void Network::setConfigMode() {
  mode = Supla::DEVICE_MODE_CONFIG;
  modeChanged = true;
}

void Network::setNormalMode() {
  mode = Supla::DEVICE_MODE_NORMAL;
  modeChanged = true;
}

void Network::uninit() {
}

bool Network::getMacAddr(uint8_t *buf) {
  (void)(buf);
  return false;
}

void Network::setHostname(const char *buf) {
  strncpy(hostname, buf, 32);
  SUPLA_LOG_DEBUG("Network AP/hostname: %s", hostname);
}

void Network::setSuplaDeviceClass(SuplaDeviceClass *ptr) {
  sdc = ptr;
}

bool Network::isWifiConfigRequired() {
  return false;
}

void Network::setSSLEnabled(bool enabled) {
  sslEnabled = enabled;
}

void Network::setCACert(const char *rootCA) {
  (void)(rootCA);
  rootCACert = rootCA;
}

};  // namespace Supla
