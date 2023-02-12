/*
 * Copyright (C) AC SOFTWARE SP. Z O.O
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef SRC_SUPLA_NETWORK_HTML_TEXT_CMD_INPUT_PARAMETER_H_
#define SRC_SUPLA_NETWORK_HTML_TEXT_CMD_INPUT_PARAMETER_H_

#include <supla/network/html_element.h>
#include <supla/local_action.h>
#include <stdint.h>

namespace Supla {

namespace Html {

struct RegisteredCmdActionMap {
  char *cmd = nullptr;
  int eventId = 0;
  RegisteredCmdActionMap *next = nullptr;

  ~RegisteredCmdActionMap();
};

class TextCmdInputParameter : public HtmlElement, public LocalAction {
 public:
  TextCmdInputParameter(const char *paramTag, const char *paramLabel);
  virtual ~TextCmdInputParameter();
  void send(Supla::WebSender* sender) override;
  bool handleResponse(const char* key, const char* value) override;
  void registerCmd(const char *cmdStr, int eventId);

 protected:
  char *tag = nullptr;
  char *label = nullptr;
  RegisteredCmdActionMap *firstCmd = nullptr;
};

};  // namespace Html
};  // namespace Supla


#endif  // SRC_SUPLA_NETWORK_HTML_TEXT_CMD_INPUT_PARAMETER_H_
