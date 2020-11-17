#include "SuplaTemplateBoard.h"
#include "SuplaWebPageRelay.h"
#include <supla/events.h>

void setButton(uint8_t gpio, uint8_t event) {
  uint8_t nr = ConfigManager->get(KEY_MAX_BUTTON)->getValueInt();
  String test;
  nr = nr + 1;
  test = nr;
  ConfigManager->set(KEY_MAX_BUTTON, test.c_str());
  ConfigESP->setGpio(gpio, nr, FUNCTION_BUTTON, event);
}

void setRelay(uint8_t gpio, uint8_t level) {
  uint8_t nr = ConfigManager->get(KEY_MAX_RELAY)->getValueInt();
  String test;
  nr = nr + 1;
  test = nr;
  ConfigManager->set(KEY_MAX_RELAY, test.c_str());
  ConfigESP->setGpio(gpio, nr, FUNCTION_RELAY, level, MEMORY_RELAY_RESTORE);
}

void setLimitSwitch(uint8_t gpio) {
  uint8_t nr = ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt();
  String test;
  nr = nr + 1;
  test = nr;
  ConfigManager->set(KEY_MAX_LIMIT_SWITCH, test.c_str());
  ConfigESP->setGpio(gpio, nr, FUNCTION_LIMIT_SWITCH, 0);
  ConfigESP->setGpio(4, 1, FUNCTION_LIMIT_SWITCH, 0);
}

void setLedCFG(uint8_t gpio, uint8_t level) {
  ConfigESP->setGpio(gpio, FUNCTION_CFG_LED, level);
}

void setButtonCFG(uint8_t gpio) {
  ConfigESP->setGpio(gpio, FUNCTION_CFG_BUTTON);
}

void chooseTemplateBoard(uint8_t board) {
  ConfigManager->set(KEY_MAX_BUTTON, "0");
  ConfigManager->set(KEY_MAX_RELAY, "0");
  ConfigManager->set(KEY_MAX_LIMIT_SWITCH, "0");
  
  switch (board) {
    case BOARD_ELECTRODRAGON:
      setLedCFG(16);
      setButtonCFG(0);
      setButton(0);
      setButton(2);
      setRelay(12);
      setRelay(13);
      break;
    case BOARD_INCAN3:
      setLedCFG(2, LOW);
      setButtonCFG(0);
      setButton(14, Supla::ON_CHANGE);
      setButton(12, Supla::ON_CHANGE);
      setRelay(5);
      setRelay(13);
      setLimitSwitch(4);
      setLimitSwitch(16);
      break;
    case BOARD_INCAN4:
      setLedCFG(12);
      setButtonCFG(0);
      setButton(2, Supla::ON_CHANGE);
      setButton(10, Supla::ON_CHANGE);
      setRelay(4);
      setRelay(14);
      setLimitSwitch(4);
      setLimitSwitch(16);
      break;
    case BOARD_MELINK:
      setLedCFG(12);
      setButtonCFG(5);
      setButton(5);
      setRelay(4);
      break;
    case BOARD_NEO_COOLCAM:
      setLedCFG(4);
      setButtonCFG(13);
      setButton(13);
      setRelay(12);
      break;
    case BOARD_SHELLY1:
      setButtonCFG(5);
      setButton(5);
      setRelay(4);
      break;
    case BOARD_SHELLY2:
      setLedCFG(16);
      setButtonCFG(12);
      setButton(12);
      setButton(14);
      setRelay(4);
      setRelay(5);
      break;
    case BOARD_SONOFF_BASIC:
      setLedCFG(13);
      setButtonCFG(0);
      setButton(0);
      setRelay(12);
      break;
    case BOARD_SONOFF_DUAL_R2:
      setLedCFG(13);
      setButtonCFG(0);
      setButton(0);
      setButton(9);
      setRelay(12);
      setRelay(5);
      break;
    case BOARD_SONOFF_S2X:
      setLedCFG(13);
      setButtonCFG(0);
      setButton(0);
      setRelay(12);
      ConfigESP->setGpio(14, FUNCTION_SI7021_SONOFF);
      break;
    case BOARD_SONOFF_SV:
      setLedCFG(13);
      setButtonCFG(0);
      setButton(0);
      setRelay(12);
      break;
    case BOARD_SONOFF_TH:
      setLedCFG(13);
      setButtonCFG(0);
      setButton(0);
      setRelay(12);
      ConfigESP->setGpio(14, FUNCTION_SI7021_SONOFF);
      break;
    case BOARD_SONOFF_TOUCH:
      setLedCFG(13);
      setButtonCFG(0);
      setButton(0);
      setRelay(12);
      break;
    case BOARD_SONOFF_TOUCH_2CH:
      setLedCFG(13);
      setButtonCFG(0);
      setButton(0);
      setButton(9);
      setRelay(12);
      setRelay(5);
      break;
    case BOARD_SONOFF_TOUCH_3CH:
      setLedCFG(13);
      setButtonCFG(0);
      setButton(0);
      setButton(9);
      setButton(10);
      setRelay(12);
      setRelay(5);
      setRelay(4);
      break;
    case BOARD_SONOFF_4CH:
      setLedCFG(13);
      setButtonCFG(0);
      setButton(0);
      setButton(9);
      setButton(10);
      setButton(14);
      setRelay(12);
      setRelay(5);
      setRelay(4);
      setRelay(15);
      break;
    case BOARD_YUNSHAN:
      setLedCFG(2, LOW);
      setButtonCFG(0);
      setButton(3, Supla::ON_CHANGE);
      setRelay(4);
      break;

    case BOARD_YUNTONG_SMART:
      setLedCFG(15);
      setButtonCFG(12);
      setButton(12, Supla::ON_CHANGE);
      setRelay(4);
      break;
  }
}
