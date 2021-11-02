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

#include "SuplaWebCorrection.h"

void createWebCorrection() {
  WebServer->httpServer->on(getURL(PATH_CORRECTION), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handleCorrection();
    else
      handleCorrectionSave();
  });
}

void handleCorrection(int save) {
  String correction;

  WebServer->sendHeaderStart();

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_CORRECTION);

  addForm(webContentBuffer, F("post"), PATH_CORRECTION);

  addFormHeader(webContentBuffer, S_CORRECTION_FOR_CH);
  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_THERMOMETER) {
        correction = ConfigManager->get(KEY_CORRECTION_TEMP)->getElement(channel->getChannelNumber());

        addNumberBox(webContentBuffer, getInput(INPUT_CORRECTION_TEMP, channel->getChannelNumber()),
                     String(S_CH_CORRECTION) + S_SPACE + channel->getChannelNumber(), emptyString, false, correction);
      }

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {
        correction = ConfigManager->get(KEY_CORRECTION_TEMP)->getElement(channel->getChannelNumber());
        addNumberBox(webContentBuffer, getInput(INPUT_CORRECTION_TEMP, channel->getChannelNumber()),
                     String(S_CH_CORRECTION) + S_SPACE + channel->getChannelNumber() + S_SPACE + ("[1]"), emptyString, false, correction);

        correction = ConfigManager->get(KEY_CORRECTION_HUMIDITY)->getElement(channel->getChannelNumber());
        addNumberBox(webContentBuffer, getInput(INPUT_CORRECTION_HUMIDITY, channel->getChannelNumber()),
                     String(S_CH_CORRECTION) + S_SPACE + channel->getChannelNumber() + S_SPACE + F("[2]"), emptyString, false, correction);
      }
    }
  }
  addFormHeaderEnd(webContentBuffer);

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);

  WebServer->sendHeaderEnd();
}

void handleCorrectionSave() {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  String correction;

  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_THERMOMETER) {
        correction = WebServer->httpServer->arg(getInput(INPUT_CORRECTION_TEMP, channel->getChannelNumber()));
        ConfigManager->setElement(KEY_CORRECTION_TEMP, channel->getChannelNumber(), correction.c_str());
      }

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {
        correction = WebServer->httpServer->arg(getInput(INPUT_CORRECTION_TEMP, channel->getChannelNumber()));
        ConfigManager->setElement(KEY_CORRECTION_TEMP, channel->getChannelNumber(), correction.c_str());

        correction = WebServer->httpServer->arg(getInput(INPUT_CORRECTION_HUMIDITY, channel->getChannelNumber()));
        ConfigManager->setElement(KEY_CORRECTION_HUMIDITY, channel->getChannelNumber(), correction.c_str());
      }
    }
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleCorrection(5);
      ConfigESP->rebootESP();
      break;
    case E_CONFIG_FILE_OPEN:
      handleCorrection(2);
      break;
  }
}
