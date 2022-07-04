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

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <openssl/err.h>
#include <poll.h>
#include <resolv.h>
#include <string.h>
#include <supla/supla_lib_config.h>
#include <unistd.h>

#include <iostream>

#include "linux_network.h"

int32_t print_ssl_error(SSL *ssl, int ret_code) {
  int32_t ssl_error;

  ssl_error = SSL_get_error(ssl, ret_code);

  switch (ssl_error) {
    case SSL_ERROR_NONE:
      supla_log(LOG_ERR, "SSL_ERROR_NONE");
      break;
    case SSL_ERROR_SSL:
      supla_log(LOG_ERR, "SSL_ERROR_SSL");
      break;
    case SSL_ERROR_WANT_READ:
      supla_log(LOG_ERR, "SSL_ERROR_WANT_READ");
      break;
    case SSL_ERROR_WANT_WRITE:
      supla_log(LOG_ERR, "SSL_ERROR_WANT_WRITE");
      break;
    case SSL_ERROR_WANT_X509_LOOKUP:
      supla_log(LOG_ERR, "SSL_ERROR_WANT_X509_LOOKUP");
      break;
    case SSL_ERROR_SYSCALL:
      supla_log(LOG_ERR, "SSL_ERROR_SYSCALL");
      break;
    case SSL_ERROR_ZERO_RETURN:
      supla_log(LOG_ERR, "SSL_ERROR_ZERO_RETURN");
      break;
    case SSL_ERROR_WANT_CONNECT:
      supla_log(LOG_ERR, "SSL_ERROR_WANT_CONNECT");
      break;
    case SSL_ERROR_WANT_ACCEPT:
      supla_log(LOG_ERR, "SSL_ERROR_WANT_ACCEPT");
      break;
  }

  return ssl_error;
}

void print_ssl_certs(SSL *ssl) {
  X509 *cert = nullptr;
  char *line;

  cert = SSL_get_peer_certificate(ssl);
  if (cert != NULL) {
    supla_log(LOG_DEBUG, "Server certificates:");
    line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
    supla_log(LOG_DEBUG, "Subject: %s", line);
    free(line);
    line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
    supla_log(LOG_DEBUG, "Issuer: %s", line);
    free(line);
    X509_free(cert);
  } else {
    supla_log(LOG_DEBUG, "No certificates.");
  }
}

Supla::LinuxNetwork::LinuxNetwork() : Network(nullptr) {
  signal(SIGPIPE, SIG_IGN);
}

Supla::LinuxNetwork::~LinuxNetwork() {
  disconnect();
  SSL_CTX_free(ctx);
  ctx = nullptr;
}

int Supla::LinuxNetwork::read(void *buf, int count) {
  _supla_int_t result = SSL_read(ssl, buf, count);
  if (result > 0) {
#ifdef SUPLA_COMM_DEBUG
    printData("Recv", buf, result);
#endif
    return result;
  } else {
    int sslError = SSL_get_error(ssl, result);

    switch (sslError) {
      case SSL_ERROR_WANT_READ: {
        break;
      }
      case SSL_ERROR_ZERO_RETURN: {
        supla_log(LOG_INFO, "Connection closed by peer");
        Disconnect();
        break;
      }
      default: {
        print_ssl_error(ssl, result);
      }
    }
  }

  return -1;
}

int Supla::LinuxNetwork::write(void *buf, int count) {
#ifdef SUPLA_COMM_DEBUG
  printData("Send", buf, count);
#endif
  int sendSize = SSL_write(ssl, buf, count);
  if (sendSize <= 0) {
    print_ssl_error(ssl, sendSize);
  }
  return sendSize;
}

int Supla::LinuxNetwork::connect(const char *server, int port) {
  ssl = SSL_new(ctx);
  if (ssl == nullptr) {
    supla_log(LOG_ERR, "SSL_new() failed");
    return 0;
  }

  int connectionPort = (port == -1 ? 2015 : port);
  if (connectionPort != 2016) {
    supla_log(LOG_WARNING,
              "Server port is not 2016. Trying to establish secured connection "
              "anyway");
  }

  supla_log(LOG_INFO,
            "Establishing connection with: %s (port: %d)",
            server,
            connectionPort);

  struct hostent *host = gethostbyname(server);
  if (host == nullptr) {
    supla_log(LOG_ERR, "gethostbyname failed (%s)", server);
    return 0;
  }

  struct addrinfo hints = {0};
  struct addrinfo *addresses;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  char portStr[10] = "2016";
  snprintf(portStr, sizeof(portStr), "%d", port);

  const int status = getaddrinfo(server, portStr, &hints, &addresses);
  if (status != 0) {
    supla_log(LOG_ERR, "%s: %s", server, gai_strerror(status));
    return 0;
  }

  int err = 0;
  int flagsCopy = 0;
  for (struct addrinfo *addr = addresses; addr != nullptr;
       addr = addr->ai_next) {
    connectionFd = socket(
        addresses->ai_family, addresses->ai_socktype, addresses->ai_protocol);
    if (connectionFd == -1) {
      err = errno;
      continue;
    }

    flagsCopy = fcntl(connectionFd, F_GETFL, 0);
    fcntl(connectionFd, F_SETFL, O_NONBLOCK);
    if (::connect(connectionFd, addr->ai_addr, addr->ai_addrlen) == 0) {
      break;
    }

    err = errno;
    bool isConnected = false;

    if (errno == EWOULDBLOCK || errno == EINPROGRESS) {
      struct pollfd pfd = {};
      pfd.fd = connectionFd;
      pfd.events = POLLOUT;

      int result = poll(&pfd, 1, 3000);
      if (result > 0) {
        socklen_t len = sizeof(err);
        int retval = getsockopt(connectionFd, SOL_SOCKET, SO_ERROR, &err, &len);

        if (retval == 0 && err == 0) {
          isConnected = true;
        }
      }
    }

    if (isConnected) {
      break;
    }
    connectionFd = -1;
    close(connectionFd);
  }

  freeaddrinfo(addresses);

  if (connectionFd == -1) {
    supla_log(LOG_ERR, "%s: %s", server, strerror(err));
    return 0;
  }

  fcntl(connectionFd, F_SETFL, flagsCopy);

  SSL_set_fd(ssl, connectionFd);
  SSL_connect(ssl);

  fcntl(connectionFd, F_SETFL, O_NONBLOCK);
  supla_log(LOG_DEBUG, "Connected with %s encryption", SSL_get_cipher(ssl));
  SSL_get_cipher(ssl);
  print_ssl_certs(ssl);

  return 1;
}

bool Supla::LinuxNetwork::connected() {
  return connectionFd >= 0;
}

void Supla::LinuxNetwork::disconnect() {
  if (ssl) {
    SSL_free(ssl);
  }
  if (connectionFd >= 0) {
    close(connectionFd);
  }
  connectionFd = -1;
  ssl = nullptr;
}

bool Supla::LinuxNetwork::isReady() {
  return isDeviceReady;
}

void Supla::LinuxNetwork::setup() {
  if (ctx == nullptr) {
    const SSL_METHOD *method = TLS_client_method();
    ctx = SSL_CTX_new(method);

    if (ctx == nullptr) {
      supla_log(LOG_ERR, "SSL_CTX_new failed");
      return;
    }
  }
  isDeviceReady = true;
}

bool Supla::LinuxNetwork::iterate() {
  return true;
}

void Supla::LinuxNetwork::fillStateData(TDSC_ChannelState *channelState) {
  channelState->Fields |= SUPLA_CHANNELSTATE_FIELD_IPV4;

  struct sockaddr_in address = {};
  socklen_t len = sizeof(address);
  getsockname(connectionFd, (struct sockaddr *)&address, &len);
  channelState->IPv4 =
      static_cast<unsigned _supla_int_t>(address.sin_addr.s_addr);
}
