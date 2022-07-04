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

#ifndef EXTRAS_PORTING_LINUX_LINUX_NETWORK_H_
#define EXTRAS_PORTING_LINUX_LINUX_NETWORK_H_

#include <openssl/ssl.h>
#include <supla/network/network.h>

namespace Supla {

class LinuxNetwork : public Network {
 public:
  LinuxNetwork();
  ~LinuxNetwork() override;

  int read(void *buf, int count) override;
  int write(void *buf, int count) override;
  int connect(const char *server, int port = -1) override;
  bool connected() override;
  void disconnect() override;
  bool isReady() override;
  void setup() override;
  bool iterate() override;
  void fillStateData(TDSC_ChannelState *channelState) override;

 protected:
  bool isDeviceReady = false;
  SSL_CTX *ctx = nullptr;
  SSL *ssl = nullptr;
  int connectionFd = -1;
};

};  // namespace Supla

#endif  // EXTRAS_PORTING_LINUX_LINUX_NETWORK_H_
