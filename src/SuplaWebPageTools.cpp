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
#include "SuplaDeviceGUI.h"

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

  WebServer->httpServer->on(getURL(PATH_TOOLS), HTTP_POST, [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    handleToolsSave();
  });
}

void handleTools(int save) {
  WebServer->sendHeaderStart();
  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_TOOLS);

#ifdef TEMPLATE_BOARD_JSON
  addForm(webContentBuffer, F("post"), PATH_TOOLS);
  addFormHeader(webContentBuffer, S_TEMPLATE_BOARD);
  addTextBox(webContentBuffer, INPUT_BOARD, F("JSON"), F(""), 0, 600, false);
  addButtonSubmit(webContentBuffer, S_LOAD_CONFIGURATION);
  addTextBox(webContentBuffer, Supla::TanplateBoard::templateBoardWarning);
#ifdef TEMPLATE_JSON
  addTextBox(webContentBuffer, TEMPLATE_JSON);
#endif
  addFormHeaderEnd(webContentBuffer);
  addFormEnd(webContentBuffer);
#elif defined(TEMPLATE_BOARD_OLD)
#if (DEFAULT_TEMPLATE_BOARD == BOARD_OFF)
  addForm(webContentBuffer, F("post"), PATH_TOOLS);
  addFormHeader(webContentBuffer, S_TEMPLATE_BOARD);
  uint8_t selected = ConfigManager->get(KEY_BOARD)->getValueInt();
  addListBox(webContentBuffer, INPUT_BOARD, S_TYPE, BOARD_P, MAX_MODULE, selected);
  addFormHeaderEnd(webContentBuffer);
  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
#else
  addFormHeader(webContentBuffer, S_DEFAULT_TEMPLATE_BOARD);
  addLabel(webContentBuffer, FPSTR(BOARD_P[DEFAULT_TEMPLATE_BOARD]));
  addFormHeaderEnd(webContentBuffer);
#endif
#endif

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
  addButton(webContentBuffer, S_DEVICE_SETTINGS, PATH_DEVICE_SETTINGS);

  WebServer->sendHeaderEnd();
}

void handleToolsSave() {
#ifdef TEMPLATE_BOARD_JSON
  Supla::TanplateBoard::chooseTemplateBoard(WebServer->httpServer->arg(INPUT_BOARD).c_str());
#elif defined(TEMPLATE_BOARD_OLD)
  if (strcmp(WebServer->httpServer->arg(INPUT_BOARD).c_str(), "") != 0) {
    chooseTemplateBoard(WebServer->httpServer->arg(INPUT_BOARD).toInt());
    Supla::Storage::ScheduleSave(2000);
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleTools(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleTools(2);
      break;
  }
}
