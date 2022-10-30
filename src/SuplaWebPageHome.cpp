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

#include "SuplaWebPageHome.h"

void createWebPageHome() {
  WebServer->httpServer->on(PATH_START, [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (strcmp(WebServer->httpServer->arg(PATH_REBOT).c_str(), "1") == 0) {
      WebServer->httpServer->sendHeader("Location", PATH_START, true);
      handlePageHome(2);
      ConfigESP->rebootESP();
      return;
    }

    if (strcmp(WebServer->httpServer->arg(PATH_REBOT_CREATOR).c_str(), "1") == 0) {
      WebServer->httpServer->sendHeader("Location", PATH_START, true);
      handlePageHome(2);
      ConfigESP->rebootESP();
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handlePageHome();
    else
      handlePageHomeSave();
  });
}

void handlePageHome(int save) {
  WebServer->sendHeaderStart();

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript();

  addForm(webContentBuffer, F("post"));

  addFormHeader(webContentBuffer);
#ifdef SUPLA_SDM630
  addLabel(webContentBuffer, "ErrCode: " + String(Supla::GUI::smd->sdm.getErrCode()));
  addLabel(webContentBuffer, "ErrCount: " + String(Supla::GUI::smd->sdm.getErrCount()));
#endif

  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_THERMOMETER) {
        addLabel(webContentBuffer, String(channel->getValueDouble(), 2) + "°C");
      }

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {
        addLabel(webContentBuffer, String(channel->getValueDoubleFirst(), 2) + "°C" + S_SPACE + String(channel->getValueDoubleSecond(), 2) + "%");
      }

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYSENSOR) {
        addLabel(webContentBuffer, String(channel->getValueDoubleSecond(), 2) + "%");
      }

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_DISTANCESENSOR) {
        addLabel(webContentBuffer, String(channel->getValueDouble(), 2) + "m");
      }

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_ELECTRICITY_METER) {
        TSuplaChannelExtendedValue* extValue = channel->getExtValue();
        if (extValue == nullptr)
          break;

        TElectricityMeter_ExtendedValue_V2* emValue = reinterpret_cast<TElectricityMeter_ExtendedValue_V2*>(extValue->value);
        if (emValue->m_count < 1 || emValue == nullptr)
          break;

        for (size_t i = 0; i < 3; i++) {
          addLabel(webContentBuffer, String(emValue->m[0].voltage[i] / 100.0) + "V");
          addLabel(webContentBuffer, String(emValue->m[0].power_active[i] / 100000.0) + "W");
          addLabel(webContentBuffer, String(emValue->m[0].current[i] / 1000.0) + "A");
        }
      }
      if (channel->getChannelType() == SUPLA_CHANNELTYPE_PRESSURESENSOR) {
        addLabel(webContentBuffer, String(channel->getValueDouble()) + "hPa");
      }
    }

    if (element->getSecondaryChannel()) {
      auto channel = element->getSecondaryChannel();
      if (channel->getChannelType() == SUPLA_CHANNELTYPE_PRESSURESENSOR) {
        addLabel(webContentBuffer, String(channel->getValueDouble()) + "hPa");
      }
    }
  }
  addFormHeaderEnd(webContentBuffer);

#ifdef SUPLA_WT32_ETH01_LAN8720

#else
  addFormHeader(webContentBuffer, S_SETTING_WIFI_SSID);
  addTextBox(webContentBuffer, INPUT_WIFI_SSID, S_WIFI_SSID, KEY_WIFI_SSID, 0, MAX_SSID, true);
  addTextBoxPassword(webContentBuffer, INPUT_WIFI_PASS, S_WIFI_PASS, KEY_WIFI_PASS, 0, MAX_PASSWORD, false);
  addTextBox(webContentBuffer, INPUT_HOSTNAME, S_HOST_NAME, KEY_HOST_NAME, 0, MAX_HOSTNAME, true);
  addFormHeaderEnd(webContentBuffer);
#endif

  addFormHeader(webContentBuffer, S_SETTING_SUPLA);
  addTextBox(webContentBuffer, INPUT_SERVER, S_SUPLA_SERVER, KEY_SUPLA_SERVER, DEFAULT_SERVER, 0, MAX_SUPLA_SERVER, true);
  addTextBox(webContentBuffer, INPUT_EMAIL, S_SUPLA_EMAIL, KEY_SUPLA_EMAIL, DEFAULT_EMAIL, 0, MAX_EMAIL, true);
  addFormHeaderEnd(webContentBuffer);

  addFormHeader(webContentBuffer, S_SETTING_ADMIN);
  addTextBox(webContentBuffer, INPUT_MODUL_LOGIN, S_LOGIN, KEY_LOGIN, 0, MAX_MLOGIN, true);
  addTextBoxPassword(webContentBuffer, INPUT_MODUL_PASS, S_LOGIN_PASS, KEY_LOGIN_PASS, MIN_PASSWORD, MAX_MPASSWORD, true);
  addFormHeaderEnd(webContentBuffer);

#ifdef SUPLA_ROLLERSHUTTER
  uint8_t maxrollershutter = ConfigManager->get(KEY_MAX_RELAY)->getValueInt();
  if (maxrollershutter >= 2) {
    addFormHeader(webContentBuffer, S_ROLLERSHUTTERS);
    addNumberBox(webContentBuffer, INPUT_ROLLERSHUTTER, S_QUANTITY, KEY_MAX_ROLLERSHUTTER, (maxrollershutter / 2));
    addFormHeaderEnd(webContentBuffer);
  }
#endif

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);

  addButton(webContentBuffer, S_DEVICE_SETTINGS, PATH_DEVICE_SETTINGS);
  WebServer->sendHeaderEnd();
}

void handlePageHomeSave() {
  if (strcmp(WebServer->httpServer->arg(INPUT_WIFI_SSID).c_str(), "") != 0)
    ConfigManager->set(KEY_WIFI_SSID, WebServer->httpServer->arg(INPUT_WIFI_SSID).c_str());
  // if (strcmp(WebServer->httpServer->arg(INPUT_WIFI_PASS).c_str(), "") != 0)
  ConfigManager->set(KEY_WIFI_PASS, WebServer->httpServer->arg(INPUT_WIFI_PASS).c_str());
  if (strcmp(WebServer->httpServer->arg(INPUT_SERVER).c_str(), "") != 0)
    ConfigManager->set(KEY_SUPLA_SERVER, WebServer->httpServer->arg(INPUT_SERVER).c_str());
  if (strcmp(WebServer->httpServer->arg(INPUT_EMAIL).c_str(), "") != 0)
    ConfigManager->set(KEY_SUPLA_EMAIL, WebServer->httpServer->arg(INPUT_EMAIL).c_str());
  if (strcmp(WebServer->httpServer->arg(INPUT_HOSTNAME).c_str(), "") != 0)
    ConfigManager->set(KEY_HOST_NAME, WebServer->httpServer->arg(INPUT_HOSTNAME).c_str());
  if (strcmp(WebServer->httpServer->arg(INPUT_MODUL_LOGIN).c_str(), "") != 0)
    ConfigManager->set(KEY_LOGIN, WebServer->httpServer->arg(INPUT_MODUL_LOGIN).c_str());
  if (strcmp(WebServer->httpServer->arg(INPUT_MODUL_PASS).c_str(), "") != 0)
    ConfigManager->set(KEY_LOGIN_PASS, WebServer->httpServer->arg(INPUT_MODUL_PASS).c_str());

#ifdef SUPLA_ROLLERSHUTTER
  if (strcmp(WebServer->httpServer->arg(INPUT_ROLLERSHUTTER).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_ROLLERSHUTTER, WebServer->httpServer->arg(INPUT_ROLLERSHUTTER).toInt());

    if (ConfigManager->get(KEY_MAX_ROLLERSHUTTER)->getValueInt() > 0) {
      ConfigManager->set(KEY_MAX_BUTTON, WebServer->httpServer->arg(INPUT_ROLLERSHUTTER).toInt() * 2);
    }
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      if (ConfigESP->configModeESP == Supla::DEVICE_MODE_NORMAL) {
        handlePageHome(1);
      }
      else {
        handlePageHome(7);
      }
      break;

    case E_CONFIG_FILE_OPEN:
      handlePageHome(2);
      break;
  }
}