#include "SuplaCommonPROGMEM.h"
#include "SuplaTemplateBoard.h"

String StateString(uint8_t adr) {
  return PGMT(STATE_P[adr]);
}

String LevelString(uint8_t nr) {
  return PGMT(LEVEL_P[nr]);
}

String MemoryString(uint8_t nr) {
  return PGMT(MEMORY_P[nr]);
}

String TriggerString(uint8_t nr) {
  return PGMT(TRIGGER_P[nr]);
}

String BoardString(uint8_t board) {
  return PGMT(BOARD_P[board]);
}
