#include "SuplaTemplateBoard.h"
#include "SuplaWebPageRelay.h"
#include <supla/events.h>


void chooseTemplateBoard(uint8_t board) {
  switch (board) {
    case BOARD_SONOFF_BASIC:
      ConfigESP->setGpio(0, FUNCTION_CFG_BUTTON);
      ConfigESP->setGpio(13, FUNCTION_CFG_LED, HIGH);

      ConfigManager->set(KEY_MAX_BUTTON, "1");
      ConfigESP->setGpio(14, 1, FUNCTION_BUTTON, Supla::ON_CHANGE);

      ConfigManager->set(KEY_MAX_RELAY, "1");
      ConfigESP->setGpio(12, 1, FUNCTION_RELAY, HIGH, MEMORY_RELAY_RESTORE);
      break;
    case BOARD_SONOFF_TH:
      ConfigESP->setGpio(0, FUNCTION_CFG_BUTTON);
      ConfigESP->setGpio(13, FUNCTION_CFG_LED, HIGH);

      ConfigManager->set(KEY_MAX_BUTTON, "1");
      ConfigESP->setGpio(0, 1, FUNCTION_BUTTON, Supla::ON_CHANGE);

      ConfigManager->set(KEY_MAX_RELAY, "1");
      ConfigESP->setGpio(12, 1, FUNCTION_RELAY, HIGH, MEMORY_RELAY_RESTORE);
      break;
    case BOARD_SONOFF_TOUCH:
      ConfigESP->setGpio(0, FUNCTION_CFG_BUTTON);
      ConfigESP->setGpio(13, FUNCTION_CFG_LED, HIGH);

      ConfigManager->set(KEY_MAX_BUTTON, "1");
      ConfigESP->setGpio(0, 1, FUNCTION_BUTTON, Supla::ON_CHANGE);

      ConfigManager->set(KEY_MAX_RELAY, "1");
      ConfigESP->setGpio(12, 1, FUNCTION_RELAY, HIGH, MEMORY_RELAY_RESTORE);
      break;
    case BOARD_SONOFF_TOUCH_2CH:
      ConfigESP->setGpio(0, FUNCTION_CFG_BUTTON);
      ConfigESP->setGpio(13, FUNCTION_CFG_LED, HIGH);

      ConfigManager->set(KEY_MAX_BUTTON, "2");
      ConfigESP->setGpio(0, 1, FUNCTION_BUTTON, Supla::ON_CHANGE);
      ConfigESP->setGpio(9, 2, FUNCTION_BUTTON, Supla::ON_CHANGE);

      ConfigManager->set(KEY_MAX_RELAY, "2");
      ConfigESP->setGpio(12, 1, FUNCTION_RELAY, HIGH, MEMORY_RELAY_RESTORE);
      ConfigESP->setGpio(5, 2, FUNCTION_RELAY, HIGH, MEMORY_RELAY_RESTORE);
      break;
    case BOARD_SONOFF_TOUCH_3CH:
      ConfigESP->setGpio(0, FUNCTION_CFG_BUTTON);
      ConfigESP->setGpio(13, FUNCTION_CFG_LED, HIGH);

      ConfigManager->set(KEY_MAX_BUTTON, "3");
      ConfigESP->setGpio(0, 1, FUNCTION_BUTTON, Supla::ON_CHANGE);
      ConfigESP->setGpio(9, 2, FUNCTION_BUTTON, Supla::ON_CHANGE);
      ConfigESP->setGpio(10, 3, FUNCTION_BUTTON, Supla::ON_CHANGE);

      ConfigManager->set(KEY_MAX_RELAY, "3");
      ConfigESP->setGpio(12, 1, FUNCTION_RELAY, HIGH, MEMORY_RELAY_RESTORE);
      ConfigESP->setGpio(5, 2, FUNCTION_RELAY, HIGH, MEMORY_RELAY_RESTORE);
      ConfigESP->setGpio(4, 3, FUNCTION_RELAY, HIGH, MEMORY_RELAY_RESTORE);
      break;
    case BOARD_SONOFF_4CH:
      ConfigESP->setGpio(0, FUNCTION_CFG_BUTTON);
      ConfigESP->setGpio(13, FUNCTION_CFG_LED, HIGH);

      ConfigManager->set(KEY_MAX_BUTTON, "4");
      ConfigESP->setGpio(0, 1, FUNCTION_BUTTON, Supla::ON_CHANGE);
      ConfigESP->setGpio(9, 2, FUNCTION_BUTTON, Supla::ON_CHANGE);
      ConfigESP->setGpio(10, 3, FUNCTION_BUTTON, Supla::ON_CHANGE);
      ConfigESP->setGpio(14, 4, FUNCTION_BUTTON, Supla::ON_CHANGE);

      ConfigManager->set(KEY_MAX_RELAY, "4");
      ConfigESP->setGpio(12, 1, FUNCTION_RELAY, HIGH, MEMORY_RELAY_RESTORE);
      ConfigESP->setGpio(5, 2, FUNCTION_RELAY, HIGH, MEMORY_RELAY_RESTORE);
      ConfigESP->setGpio(4, 3, FUNCTION_RELAY, HIGH, MEMORY_RELAY_RESTORE);
      ConfigESP->setGpio(15, 4, FUNCTION_RELAY, HIGH, MEMORY_RELAY_RESTORE);

      break;
    case BOARD_YUNSHA:
      ConfigESP->setGpio(0, FUNCTION_CFG_BUTTON);
      ConfigESP->setGpio(2, FUNCTION_CFG_LED, HIGH);

      ConfigManager->set(KEY_MAX_BUTTON, "1");
      ConfigESP->setGpio(3, 1, FUNCTION_BUTTON, Supla::ON_CHANGE);

      ConfigManager->set(KEY_MAX_RELAY, "1");
      ConfigESP->setGpio(4, 1, FUNCTION_RELAY, HIGH, MEMORY_RELAY_RESTORE);
      break;
    default:
      ConfigManager->set(KEY_MAX_BUTTON, "1");
      ConfigManager->set(KEY_MAX_RELAY, "1");
      break;
  }
}
