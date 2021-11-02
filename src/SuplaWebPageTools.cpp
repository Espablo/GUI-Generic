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

#include "SuplaWebPageTools.h"

void createWebTools() {
  WebServer->httpServer->on(getURL(PATH_TOOLS), HTTP_GET, [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    String sCommand = WebServer->httpServer->arg(ARG_PARM_URL);

    if (strcasecmp_P(sCommand.c_str(), PATH_DOWNLOAD) == 0) {
      handleDownload();
    }
    else if (strcasecmp_P(sCommand.c_str(), PATH_UPLOAD) == 0) {
      handleUpload();
    }
    else if (strcasecmp_P(sCommand.c_str(), PATH_RESET) == 0) {
      WebServer->httpServer->sendHeader(F("Location"), PATH_START);
      // WebServer->httpServer->send(303);
      handlePageHome(2);
      ConfigESP->reset(true);
    }
    else if (strcasecmp_P(sCommand.c_str(), PATH_FACTORY_RESET) == 0) {
      WebServer->httpServer->sendHeader(F("Location"), PATH_START);
      // WebServer->httpServer->send(303);
      handlePageHome(1);
      ConfigESP->factoryReset(true);
    }
    else {
      handleTools();
    }
  });
}

void handleTools() {
  addFormHeader(webContentBuffer, S_TOOLS);
  addButton(webContentBuffer, S_SAVE_CONFIGURATION, getParameterRequest(PATH_TOOLS, ARG_PARM_URL, PATH_DOWNLOAD));
  addButton(webContentBuffer, S_LOAD_CONFIGURATION, getParameterRequest(PATH_TOOLS, ARG_PARM_URL, PATH_UPLOAD));
#ifdef SUPLA_OTA
  addButton(webContentBuffer, S_UPDATE, PATH_UPDATE_HENDLE);
#endif
  addButton(webContentBuffer, S_RESET_CONFIGURATION, getParameterRequest(PATH_TOOLS, ARG_PARM_URL, PATH_RESET));
  addButton(webContentBuffer, S_RESTORE_FACTORY_SETTING, getParameterRequest(PATH_TOOLS, ARG_PARM_URL, PATH_FACTORY_RESET));
  addFormHeaderEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, "");

  WebServer->sendContent();
}
