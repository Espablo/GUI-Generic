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

#ifndef _local_action_h
#define _local_action_h

#include <stdint.h>
#include "action_handler.h"

namespace Supla {

class ActionHandlerClient;
class Condition;

class LocalAction {
 public:
  virtual ~LocalAction();
  virtual void addAction(int action, ActionHandler &client, int event);
  virtual void addAction(int action, ActionHandler *client, int event);

  virtual void runAction(int event);

  virtual bool isEventAlreadyUsed(int event);

  virtual void disableOtherClients(ActionHandler &client, int event);
  virtual void enableOtherClients(ActionHandler &client, int event);
  virtual void disableOtherClients(ActionHandler *client, int event);
  virtual void enableOtherClients(ActionHandler *client, int event);

  static ActionHandlerClient *getClientListPtr();
};

};  // namespace Supla

#endif
