#include "SuplaCommonPROGMEM.h"

static char buffer[15];

String GIPOString(uint8_t gpio) {
  strcpy_P(buffer, (char*)pgm_read_ptr(&(GPIO_P[gpio])));
  return buffer;
}

String BME280String(uint8_t adr) {
  strcpy_P(buffer, (char*)pgm_read_ptr(&(BME280_P[adr])));
  return buffer;
}

String SHT30String(uint8_t adr) {
  strcpy_P(buffer, (char*)pgm_read_ptr(&(SHT30_P[adr])));
  return buffer;
}

String StateString(uint8_t adr) {
  strcpy_P(buffer, (char*)pgm_read_ptr(&(STATE_P[adr])));
  return buffer;
}

String LevelString(uint8_t nr) {
  strcpy_P(buffer, (char*)pgm_read_ptr(&(LEVEL_P[nr])));
  return buffer;
}

String MemoryString(uint8_t nr) {
  strcpy_P(buffer, (char*)pgm_read_ptr(&(MEMORY_P[nr])));
  return buffer;
}

String TriggerString(uint8_t nr) {
  strcpy_P(buffer, (char*)pgm_read_ptr(&(TRIGGER_P[nr])));
  return buffer;
}
