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

#include "pushover.h"

#include <Arduino.h>

namespace Supla {
namespace Control {

Pushover::Pushover(const char *token, const char *user, bool isSecured)
    : lastMsgReceivedMs(0) {
  setToken(token);
  setUser(user);
  _isSecured = isSecured;
}

Pushover::~Pushover() {
  delete[] client;
}

void Pushover::setToken(const char *token) {
  if (token) {
    strncpy(_token, token, MAX_TOKEN_SIZE);
  }
}

void Pushover::setUser(const char *user) {
  if (user) {
    strncpy(_user, user, MAX_USER_SIZE);
  }
}

void Pushover::setTitle(const char *title) {
  if (title) {
    strncpy(_title, title, MAX_TITLE_SIZE * 2);
  }
}

void Pushover::setMessage(const char *message) {
  if (message) {
    strncpy(_message, message, MAX_MESSAGE_SIZE * 2);
  }
}

void Pushover::setSound(uint8_t sound) {
  switch (sound) {
    case Supla::PushoverSound::SOUND_PUSHOVER:
      strncpy(_sound, "pushover", MAX_SOUND_SIZE);
      break;
    case Supla::PushoverSound::SOUND_BIKE:
      strncpy(_sound, "bike", MAX_SOUND_SIZE);
      break;
    case Supla::PushoverSound::SOUND_BUGLE:
      strncpy(_sound, "bugle", MAX_SOUND_SIZE);
      break;
    case Supla::PushoverSound::SOUND_CASHREGISTER:
      strncpy(_sound, "cashregister", MAX_SOUND_SIZE);
      break;
    case Supla::PushoverSound::SOUND_CLASSICAL:
      strncpy(_sound, "classical", MAX_SOUND_SIZE);
      break;
    case Supla::PushoverSound::SOUND_COSMIC:
      strncpy(_sound, "cosmic", MAX_SOUND_SIZE);
      break;
    case Supla::PushoverSound::SOUND_FALLING:
      strncpy(_sound, "falling", MAX_SOUND_SIZE);
      break;
    case Supla::PushoverSound::SOUND_GAMELAN:
      strncpy(_sound, "gamelan", MAX_SOUND_SIZE);
      break;
    case Supla::PushoverSound::SOUND_INCOMING:
      strncpy(_sound, "incoming", MAX_SOUND_SIZE);
      break;
    case Supla::PushoverSound::SOUND_INTERMISSION:
      strncpy(_sound, "intermission", MAX_SOUND_SIZE);
      break;
    case Supla::PushoverSound::SOUND_MAGIC:
      strncpy(_sound, "magic", MAX_SOUND_SIZE);
      break;
    case Supla::PushoverSound::SOUND_MECHANICAL:
      strncpy(_sound, "mechanical", MAX_SOUND_SIZE);
      break;
    case Supla::PushoverSound::SOUND_PIANOBAR:
      strncpy(_sound, "pianobar", MAX_SOUND_SIZE);
      break;
    case Supla::PushoverSound::SOUND_SIREN:
      strncpy(_sound, "siren", MAX_SOUND_SIZE);
      break;
    case Supla::PushoverSound::SOUND_SPACEALARM:
      strncpy(_sound, "spacealarm", MAX_SOUND_SIZE);
      break;
    case Supla::PushoverSound::SOUND_TUGBOAT:
      strncpy(_sound, "tugboat", MAX_SOUND_SIZE);
      break;
    case Supla::PushoverSound::SOUND_ALIEN:
      strncpy(_sound, "alien", MAX_SOUND_SIZE);
      break;
    case Supla::PushoverSound::SOUND_CLIMB:
      strncpy(_sound, "climb", MAX_SOUND_SIZE);
      break;
    case Supla::PushoverSound::SOUND_PERSISTENT:
      strncpy(_sound, "persistent", MAX_SOUND_SIZE);
      break;
    case Supla::PushoverSound::SOUND_ECHO:
      strncpy(_sound, "echo", MAX_SOUND_SIZE);
      break;
    case Supla::PushoverSound::SOUND_UPDOWN:
      strncpy(_sound, "updown", MAX_SOUND_SIZE);
      break;
    case Supla::PushoverSound::SOUND_VIBRATE:
      strncpy(_sound, "vibrate", MAX_SOUND_SIZE);
      break;

    default:
      strncpy(_sound, "bugle", MAX_SOUND_SIZE);
      break;
  }
}

bool Pushover::openConnection() {
  if (!client->connect(host, _isSecured ? 443 : 80)) {
    return false;
  }
  return true;
}

bool Pushover::closeConnection() {
  client->stop();
  return checkConnection();
}

bool Pushover::checkConnection() {
  if (client->connected() == 1) {
    return true;
  } else {
    return false;
  }
}

void Pushover::toggleConnection() {
  if (client == NULL) {
    if (_isSecured) {
      client = new WiFiClientSecure();
      ((WiFiClientSecure *)client)->setInsecure();
#ifdef ARDUINO_ARCH_ESP8266
      ((WiFiClientSecure *)client)->setBufferSizes(1024, 256);
#endif
    } else {
      client = new WiFiClient();
    }
  }

  if (checkConnection()) {
    closeConnection();
  } else {
    openConnection();
  }
}

void Pushover::sendRequest() {
  if (client) {
    String post = String("token=") + _token + "&user=" + _user +
                  "&title=" + _title + "&message=" + _message +
                  "&sound=" + _sound;

    /* String("token=") + _token + "&user=" + _user + "&title=" + _title +
     "&message=" + _message + "&device=" + _device + "&url=" + _url +
     "&url_title=" + _url_title + "&priority=" + _priority +
     "&retry=" + _retry + "&expire=" + _expire + "&sound=" + _sound;*/

    client->print(String("POST ") + path + " HTTP/1.1\r\n" + "host: " + host +
                  "\r\n" + "Content-length: " + String(post.length()) + "\r\n" +
                  "Content-Type: application/x-www-form-urlencoded\r\n" +
                  "Connection: close\r\n\r\n" + post);

    int timeout_at = millis() + 5000;
    while (!client->available() && timeout_at - millis() < 0) {
      Serial.println(F("Pushover - timeout"));
      client->stop();
      return;
    }

    while (client->available()) {
      if (client->readStringUntil('\n') == "\r") {
        Serial.println(F("Pushover - Headers received"));
        break;
      }
    }

    String line = client->readString();
    if (line.indexOf("\"status\":1") != -1 || line.indexOf("200 OK") != -1) {
      Serial.println(F("Alert sent successfully!"));
    } else {
      Serial.println(F("Alert failure"));
      Serial.println(line);
    }
  }
}

void Pushover::send() {
  toggleConnection();
  sendRequest();
  toggleConnection();

  if (client) {
    delete client;
    client = nullptr;
  }
}

void Pushover::iterateAlways() {
  if (lastMsgReceivedMs != 0 && millis() - lastMsgReceivedMs > 1000) {
    lastMsgReceivedMs = 0;
    send();
  }
}

void Pushover::handleAction(int event, int action) {
  (void)(event);
  if (action == Supla::TURN_ON) {
    lastMsgReceivedMs = millis();
  }
}

};  // namespace Control
};  // namespace Supla
