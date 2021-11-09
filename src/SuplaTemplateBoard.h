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

#ifndef SuplaTemplateBoard_h
#define SuplaTemplateBoard_h

#include "Arduino.h"
#include <supla/events.h>
#include <supla/actions.h>
#include <pgmspace.h>
#include "GUIGenericCommon.h"

#define BOARD_OFF              0
#define BOARD_ELECTRODRAGON    1
#define BOARD_INCAN3           2
#define BOARD_INCAN4           3
#define BOARD_MELINK           4
#define BOARD_NEO_COOLCAM      5
#define BOARD_SHELLY1          6
#define BOARD_SHELLY2          7
#define BOARD_SONOFF_BASIC     8
#define BOARD_SONOFF_DUAL_R2   9
#define BOARD_SONOFF_S2X       10
#define BOARD_SONOFF_SV        11
#define BOARD_SONOFF_TH        12
#define BOARD_SONOFF_TOUCH     13
#define BOARD_SONOFF_TOUCH_2CH 14
#define BOARD_SONOFF_TOUCH_3CH 15
#define BOARD_SONOFF_4CH       16
#define BOARD_YUNSHAN          17
#define BOARD_YUNTONG_SMART    18
#define BOARD_GOSUND_SP111     19
#define BOARD_DIMMER_LUKASZH   20
#define BOARD_H801             21
#define BOARD_SHELLY_PLUG_S    22
#define BOARD_SONOFF_MINI      23
#define BOARD_MINITIGER_1CH    24
#define BOARD_MINITIGER_2CH    25
#define BOARD_MINITIGER_3CH    26
#define MAX_MODULE             27

#if (DEFAULT_TEMPLATE_BOARD != BOARD_OFF)
#define SUPLA_RELAY
#define SUPLA_BUTTON
#define SUPLA_LIMIT_SWITCH
#define SUPLA_ROLLERSHUTTER
#define SUPLA_CONFIG
#define SUPLA_ENABLE_GUI
//#define SUPLA_ENABLE_SSL

#if (DEFAULT_TEMPLATE_BOARD == BOARD_GOSUND_SP111) || (DEFAULT_TEMPLATE_BOARD == BOARD_SHELLY_PLUG_S)
#define SUPLA_HLW8012
#define SUPLA_LED
#undef SUPLA_LIMIT_SWITCH
#undef SUPLA_ROLLERSHUTTER

#elif (DEFAULT_TEMPLATE_BOARD == BOARD_DIMMER_LUKASZH)
#define SUPLA_RGBW

#elif (DEFAULT_TEMPLATE_BOARD == BOARD_H801)
#define SUPLA_RGBW
#undef SUPLA_LIMIT_SWITCH
#undef SUPLA_RELAY

#elif (DEFAULT_TEMPLATE_BOARD == BOARD_SONOFF_TH)
#define SUPLA_SI7021_SONOFF

#endif
#endif

#if defined(SUPLA_ROLLERSHUTTER)
#if !defined(SUPLA_BUTTON)
#define SUPLA_BUTTON
#endif
#endif

#if defined(SUPLA_ROLLERSHUTTER) || defined(SUPLA_PUSHOVER) || defined(SUPLA_DIRECT_LINKS) || defined(SUPLA_LED)
#if !defined(SUPLA_RELAY)
#define SUPLA_RELAY
#endif
#endif

const char BOARD_NULL[] PROGMEM = "";
const char ELECTRODRAGON[] PROGMEM = "ElectroDragon";
const char INCAN3[] PROGMEM = "inCan3";
const char INCAN4[] PROGMEM = "inCan4";
const char MELINK[] PROGMEM = "Melink";
const char NEO_COOLCAM[] PROGMEM = "Neo Coolcam";
const char SHELLY1[] PROGMEM = "Shelly 1";
const char SHELLY2[] PROGMEM = "Shelly 2";
const char SONOFF_BASIC[] PROGMEM = "SONOFF BASIC";
const char SONOFF_DUAL_R2[] PROGMEM = "SONOFF DUAL R2";
const char SONOFF_S2X[] PROGMEM = "SONOFF S2X";
const char SONOFF_SV[] PROGMEM = "SONOFF SV";
const char SONOFF_TH[] PROGMEM = "SONOFF TH";
const char SONOFF_TOUCH[] PROGMEM = "SONOFF TOUCH";
const char SONOFF_TOUCH_2CH[] PROGMEM = "SONOFF TOUCH DUAL";
const char SONOFF_TOUCH_3CH[] PROGMEM = "SONOFF TOUCH TRIPLE";
const char SONOFF_4CH[] PROGMEM = "SONOFF 4CH";
const char YUNSHAN[] PROGMEM = "Yunshan";
const char YUNTONG_SMART[] PROGMEM = "YUNTONG Smart";
const char GOSUNG_SP111[] PROGMEM = "Gosund SP111";
const char DIMMER_LUKASZH[] PROGMEM = "Dimmer by @LukaszH";
const char H801[] PROGMEM = "H801 RGBW";
const char SHELLY_PLUG_S[] PROGMEM = "Shelly Plug S";
const char SONOFF_MINI[] PROGMEM = "SONOFF Mini";
const char MINITIGER_1CH[] PROGMEM = "Minitiger 1 Gang v2 Switch";
const char MINITIGER_2CH[] PROGMEM = "Minitiger 2 Gang v2 Switch";
const char MINITIGER_3CH[] PROGMEM = "Minitiger 3 Gang Switch";

const char* const BOARD_P[MAX_MODULE] PROGMEM = {
    S_ABSENT,       ELECTRODRAGON, INCAN3,         INCAN4,    MELINK,        NEO_COOLCAM,      SHELLY1,          SHELLY2,       SONOFF_BASIC,
    SONOFF_DUAL_R2, SONOFF_S2X,    SONOFF_SV,      SONOFF_TH, SONOFF_TOUCH,  SONOFF_TOUCH_2CH, SONOFF_TOUCH_3CH, SONOFF_4CH,    YUNSHAN,
    YUNTONG_SMART,  GOSUNG_SP111,  DIMMER_LUKASZH, H801,      SHELLY_PLUG_S, SONOFF_MINI,      MINITIGER_1CH,    MINITIGER_2CH, MINITIGER_3CH};

void addButton(
    uint8_t gpio, uint8_t event = Supla::Event::ON_PRESS, uint8_t action = Supla::Action::TOGGLE, bool pullUp = true, bool invertLogic = true);
void addRelay(uint8_t gpio, uint8_t level = HIGH);
void addLimitSwitch(uint8_t gpio);
void addLedCFG(uint8_t gpio, uint8_t level = HIGH);
void addLed(uint8_t gpio);
void addButtonCFG(uint8_t gpio);
void addHLW8012(int8_t pinCF, int8_t pinCF1, int8_t pinSEL);
void addRGBW(int8_t redPin, int8_t greenPin, int8_t bluePin, int8_t brightnessPin);
void addDimmer(int8_t brightnessPin);
void saveChooseTemplateBoard(int8_t board);
void chooseTemplateBoard(uint8_t board);

#endif  // SuplaTemplateBoard_h
