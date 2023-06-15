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

#include "SSD1306.h"

#include <ETH.h>
#include <supla/tools.h>
#include "GUIGenericCommon.h"

void msOverlay(OLEDDisplay* display, OLEDDisplayUiState* state) {
  display->setColor(WHITE);
  display->drawRect(0, 0, display->getWidth(), 20);

  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_16);
  display->drawString(display->getWidth() / 2, 0, "boneIO");
}

void drawFrameStart(OLEDDisplay* display) {
  display->clear();
  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_24);

  display->drawXbm(0, 7, supla_logo_width, supla_logo_height, supla_logo_bits);
  display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display->drawString(display->getWidth() / 2 + 22, display->getHeight() / 2, "SUPLA");

  display->display();
}

void drawFrameConfigMode(OLEDDisplay* display) {
  display->clear();
  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

  display->drawString(0, 2, ConfigESP->getLastStatusMessageSupla());
  display->drawString(0, 22, "Connect to WiFi");
  display->drawString(0, 32, getAPName());
  display->drawString(0, 42, String("IP WiFi: 192.168.4.1"));
#ifdef SUPLA_WT32_ETH01_LAN8720
  display->drawString(0, 52, String("IP ETH: ") + ETH.localIP().toString());
#else
  display->drawString(0, 52, String("IP local: ") + WiFi.localIP().toString());
#endif
  display->display();
}

void drawFrame1(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->clear();
  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

  display->drawString(0, 22, "LAST STATE");
  display->drawString(0, 32, ConfigESP->getLastStatusMessageSupla());
#ifdef SUPLA_WT32_ETH01_LAN8720
  display->drawString(0, 52, String("IP: ") + ETH.localIP().toString());
#else
  display->drawString(0, 52, String("IP: ") + WiFi.localIP().toString());
#endif
}

void drawFrame2(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->clear();
  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

  display->drawString(0, 22, String("FW: ") + Supla::Channel::reg_dev.SoftVer);
  display->drawString(0, 32, String("VER: ") + ConfigManager->get(KEY_HOST_NAME)->getValue());
  display->drawString(0, 42, String("MAC: ") + WiFi.macAddress());

  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();
      if (channel->getChannelNumber() == 0) {
        double lastTemperature = channel->getValueDouble();
        display->drawString(0, 52, String("Temperature: ") + lastTemperature + "°C");
      }
    }
  }
}

namespace Supla {
namespace Display {
SSD1306::SSD1306(uint8_t _address, int _sda, int _scl, OLEDDISPLAY_GEOMETRY g, HW_I2C _i2cBus, int _frequency) {
  display = new SH1106Wire(_address, _sda, _scl, g, _i2cBus, _frequency);
  ui = new OLEDDisplayUi(display);

  overlays[0] = {msOverlay};
  int overlaysCount = 1;

  frames[0] = {drawFrame1};
  frames[1] = {drawFrame2};
  int frameCount = 2;

  ui->disableAllIndicators();
  ui->disableAutoTransition();
  ui->disableIndicator();
  ui->setFrameAnimation(SLIDE_LEFT);

  ui->setOverlays(overlays, overlaysCount);
  ui->setFrames(frames, frameCount);
  ui->init();

  display->setBrightness(255);
  timeLastChangeOled = millis();

  display->flipScreenVertically();

  drawFrameStart(display);
}

void SSD1306::onInit() {
  // SuplaDevice.addClock(new Supla::Clock);
}

void SSD1306::iterateAlways() {
  if (ConfigESP->getLastStatusSupla() == STATUS_CONFIG_MODE) {
    drawFrameConfigMode(display);
    return;
  }

  if (millis() - timeLastChangeOled < 10000 && firstStart) {
    drawFrameStart(display);
  }
  else {
    firstStart = false;

    int remainingTimeBudget = ui->update();

    if (remainingTimeBudget > 0) {
      delay(remainingTimeBudget);
    }
    if (millis() - timeLastChangeOled > 60000 && oledON) {
      display->displayOff();
      oledON = false;
    }
  }
}

void SSD1306::addButton(Supla::Control::Button* button) {
  if (button) {
    button->addAction(OLED_NEXT_FRAME, this, Supla::ON_PRESS);
    button->addAction(OLED_TURN_ON, this, Supla::ON_PRESS);
  }
}

void SSD1306::handleAction(int event, int action) {
  if (action == OLED_NEXT_FRAME && oledON) {
    ui->nextFrame();
    timeLastChangeOled = millis();
  }

  if (action == OLED_TURN_ON && oledON == false) {
    display->setBrightness(255);
    display->displayOn();

    timeLastChangeOled = millis();
    oledON = true;
  }
}
}  // namespace Display
}  // namespace Supla