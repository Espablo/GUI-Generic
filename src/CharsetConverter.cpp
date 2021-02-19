#include <arduino.h>
#include "CharsetConverter.h"

// In ESP8266 Arduino core v2.3.0 missing bsearch: https://github.com/esp8266/Arduino/issues/2314
// Part of GNU C Library
void * gnu_c_bsearch (const void *key, const void *base, size_t nmemb, size_t size, int (*compar) (const void *, const void *)) {
  size_t l, u, idx;
  const void *p;
  int comparison;

  l = 0;
  u = nmemb;
  while (l < u) {
    idx = (l + u) / 2;

    p = (void *) (((const char *) base) + (idx * size));
    comparison = (*compar) (key, p);
    if (comparison < 0)
      u = idx;
    else if (comparison > 0)
      l = idx + 1;
    else
      return (void *) p;
  }

  return NULL;
}

// compare function for bsearch
int charset_table_cmp(const void* p_key, const void* p_item) {
  const uint16_t key = *(uint16_t *) p_key;
  const uint16_t item = (*((char *) p_item)) << 8 | *(((char *) p_item) + 1);

  if (key < item) return -1;
  else if (key == item) return 0;
  else return 1;
}

// convert utf-8 character to windows-1250
// if utf-8 char continue with next byte, returns 0, otherwise windows-1250 char
// for unconvertable char returns 0
char utf8win1250(const uint8_t ch) {
  static uint16_t uChar;
  static uint8_t len;
  char *found;

  if ((ch & 0x80) == 0x00) {
    uChar = len = 0;
    return ch;
  } else if ((ch & 0xE0) == 0xC0) {
    uChar = ch & 0x1F; len = 1;
    return 0;
  } else if ((ch & 0xF0) == 0xE0) {
    uChar = ch & 0x0F; len = 2;
    return 0;
  } else if ((ch & 0xF8) == 0xF0) {
    uChar = ch & 0x07; len = 3;
    return 0;
  } else if ((ch & 0xC0) == 0x80 && len > 0) {
    uChar = (uChar << 6) | (ch & 0x7F); len--;
    if (len > 0)
      return 0;
  } else {
    uChar = len = 0;
    return 0;
  }

  found = (char*) gnu_c_bsearch(&uChar, utf8_win1250_table + 1, utf8_win1250_table[0], 3 * sizeof(char), charset_table_cmp);

  if (found != NULL) {
    uChar = len = 0;
    return *(found + 2); // return win1250 char at 3rd position;
  }

  return 0;
}
