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

#ifndef _pushover_h
#define _pushover_h

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <supla/action_handler.h>
#include <supla/actions.h>
#include <supla/channel_element.h>

#define MAX_TOKEN_SIZE   32
#define MAX_USER_SIZE    32
#define MAX_TITLE_SIZE   32
#define MAX_MESSAGE_SIZE 16
#define MAX_SOUND_SIZE   16

namespace Supla {}  // namespace Supla

namespace Supla {
namespace PushoverSound {
enum PushoverSound {
  SOUND_PUSHOVER,
  SOUND_BIKE,
  SOUND_BUGLE,
  SOUND_CASHREGISTER,
  SOUND_CLASSICAL,
  SOUND_COSMIC,
  SOUND_FALLING,
  SOUND_GAMELAN,
  SOUND_INCOMING,
  SOUND_INTERMISSION,
  SOUND_MAGIC,
  SOUND_MECHANICAL,
  SOUND_PIANOBAR,
  SOUND_SIREN,
  SOUND_SPACEALARM,
  SOUND_TUGBOAT,
  SOUND_ALIEN,
  SOUND_CLIMB,
  SOUND_PERSISTENT,
  SOUND_ECHO,
  SOUND_UPDOWN,
  SOUND_VIBRATE,
  SOUND_NONE,
  SOUND_COUNT
};
};
namespace Control {
class Pushover : public Element, public ActionHandler {
 public:
  Pushover(const char *token = nullptr,
           const char *user = nullptr,
           bool isSecured = true);
  ~Pushover();

  void setToken(const char *token);
  void setUser(const char *user);
  void setMessage(const char *message);
  void setTitle(const char *title);
  void setSound(uint8_t sound);

  void iterateAlways();
  void handleAction(int event, int action);

  bool checkConnection();
  void toggleConnection();
  bool openConnection();
  bool closeConnection();
  void sendRequest();
  void send();

 protected:
  WiFiClient *client = NULL;
  const char *host = "api.pushover.net";
  const char *path = "/1/messages.json";
  unsigned long lastMsgReceivedMs;

  bool _isSecured;
  char _token[MAX_TOKEN_SIZE];
  char _user[MAX_USER_SIZE];
  char _title[MAX_TITLE_SIZE * 2];
  char _message[MAX_MESSAGE_SIZE * 2];
  char _sound[MAX_SOUND_SIZE];
};

};  // namespace Control
};  // namespace Supla

#endif
