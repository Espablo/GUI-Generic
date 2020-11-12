#ifndef SuplaTemplateBoard_h
#define SuplaTemplateBoard_h

#include <pgmspace.h>

#define MAX_MODULE 9

enum _board
{
  BOARD_SONOFF_BASIC = 1,
  BOARD_SONOFF_TH,
  BOARD_SONOFF_TOUCH,
  BOARD_SONOFF_TOUCH_2CH,
  BOARD_SONOFF_TOUCH_3CH,
  BOARD_SONOFF_4CH,
  BOARD_YUNSHA,
  BOARD_INCAN3
};

const char BOARD_NULL[] PROGMEM = "BRAK";
const char SONOFF_BASIC[] PROGMEM = "SONOFF BASIC";
const char SONOFF_TH[] PROGMEM = "SONOFF TH";
const char SONOFF_TOUCH[] PROGMEM = "SONOFF TOUCH";
const char SONOFF_TOUCH_2CH[] PROGMEM = "SONOFF TOUCH DUAL";
const char SONOFF_TOUCH_3CH[] PROGMEM = "SONOFF TOUCH TRIPLE";
const char SONOFF_4CH[] PROGMEM = "SONOFF 4CH";
const char YUNSHA[] PROGMEM = "Yunshan";
const char INCAN3[] PROGMEM = "inCan3";
const char* const BOARD_P[MAX_MODULE] PROGMEM = {BOARD_NULL, SONOFF_BASIC, SONOFF_TH, SONOFF_TOUCH, SONOFF_TOUCH_2CH, SONOFF_TOUCH_3CH, SONOFF_4CH, YUNSHA, INCAN3};

void chooseTemplateBoard(uint8_t board);

#endif  // SuplaTemplateBoard_h
