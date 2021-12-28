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

#ifndef SuplaLCD_H
#define SuplaLCD_H

#include "GUI-Generic_Config.h"

#ifdef SUPLA_LCD_HD44780
#include "GUIGenericCommon.h"
#include "SuplaConfigManager.h"
#include "SuplaConfigESP.h"
#include "SuplaDevice.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <supla/action_handler.h>
#include <supla/element.h>
#include <supla/control/button.h>
#include <Arduino.h>

enum ActionLCD
{
  TURN_ON,
  NEXT_FRAME,
};

enum typeLCD
{
  LCD_2X16,
  LCD_2X20,
  LCD_4X16,
  LCD_4X20,
};

struct LCDElement {
  uint8_t chanelSensor;
  bool useAlternativeMeasurement = false;
  uint8_t screenNumbers;
};

class SuplaLCD : public Supla::Element, public Supla::ActionHandler {
 public:
  SuplaLCD(uint8_t lcdAddr, uint8_t lcdCols, uint8_t lcdRows);
  void setup(SuplaConfigManager* configManager, SuplaConfigESP* configESP);
  virtual void onInit();
  virtual void iterateAlways();
  virtual void handleAction(int event, int action);

 private:
  SuplaConfigManager* ConfigManager;
  SuplaConfigESP* ConfigESP;
  LiquidCrystal_I2C* lcd;
  LCDElement* lcdElement;

  uint8_t lcdCols = 0;
  uint8_t sensorCount = 0;
  uint8_t screenNumbers = 0;
  uint8_t screenMax = 0;

  bool oledON = true;

  void updateDisplay();
  void display();
  void nextDisplay();

  String getValueSensor(uint8_t numberSensor);
  unsigned long timeLastChange = 0;
  unsigned long backLightTimeLastChange = 0;
};
#endif
#endif
