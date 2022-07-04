/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

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

#include "tools.h"
#include "string.h"

void float2DoublePacked(float number, uint8_t *bar, int byteOrder) {
  (void)(byteOrder);
  _FLOATCONV fl = {};
  fl.f = number;
  _DBLCONV dbl;
  dbl.p.s = fl.p.s;
  dbl.p.e = fl.p.e - 127 + 1023;  // exponent adjust
  dbl.p.m = fl.p.m;

#ifdef IEEE754_ENABLE_MSB
  if (byteOrder == LSBFIRST) {
#endif
    for (int i = 0; i < 8; i++) {
      bar[i] = dbl.b[i];
    }
#ifdef IEEE754_ENABLE_MSB
  } else {
    for (int i = 0; i < 8; i++) {
      bar[i] = dbl.b[7 - i];
    }
  }
#endif
}

float doublePacked2float(uint8_t *bar) {
  _FLOATCONV fl;
  _DBLCONV dbl = {};
  for (int i = 0; i < 8; i++) {
    dbl.b[i] = bar[i];
  }
  fl.p.s = dbl.p.s;
  fl.p.m = dbl.p.m;
  fl.p.e = dbl.p.e + 127 - 1023;  // exponent adjust

  return fl.f;
}

int64_t adjustRange(int64_t input,
    int64_t inMin,
    int64_t inMax,
    int64_t outMin,
    int64_t outMax) {
  int64_t result = (input - inMin) * (outMax - outMin) / (inMax - inMin);
  return result + outMin;
}

bool isArrayEmpty(void* array, size_t arraySize) {
  auto buf = reinterpret_cast<char *>(array);
  for (size_t i = 0; i <  arraySize; i++) {
    if (buf[i] != 0) {
      return false;
    }
  }
  return true;
}

int generateHexString(const void *input,
    char *output,
    int inputLength,
    char separator) {
  const char hexMap[] = "0123456789ABCDEF";
  int destIdx = 0;

  for (int i = 0; i < inputLength; i++) {
    if (i && separator) {
      output[destIdx++] = separator;
    }
    int high = static_cast<const uint8_t *>(input)[i] / 16;
    if (high > 15) {
      high = 15;
    }
    output[destIdx++] = hexMap[high];
    output[destIdx++] = hexMap[static_cast<const uint8_t *>(input)[i] % 16];
  }
  output[destIdx] = 0;
  return destIdx;
}

void hexStringToArray(const char *input, char *output, int outputLength) {
  for (int i = 0; i < outputLength; i++) {
    output[i] = hexStringToInt(input + 2*i, 2);
  }
  return;
}

uint32_t hexStringToInt(const char *str, int len) {
  uint32_t result = 0;

  for (int i = 0; i < len; i++) {
    if (i) {
      result <<= 4;
    }
    uint32_t n = 0;

    if (str[i] >= 'A' && str[i] <= 'F') {
      n = str[i] - 55;
    } else if (str[i] >= 'a' && str[i] <= 'f') {
      n = str[i] - 87;
    } else if (str[i] >= '0' && str[i] <= '9') {
      n = str[i] - 48;
    }

    result += n;
  }

  return result;
}

uint32_t stringToUInt(const char *str, int len) {
  if (len == -1) {
    len = strlen(str);
  }

  uint32_t result = 0;

  for (int i = 0; i < len; i++) {
    if (str[i] < '0' || str[i] > '9') {
      return 0;
    }
    if (i) {
      result *= 10;
    }

    result += static_cast<uint8_t>(str[i]-'0');
  }

  return result;
}

void urlDecodeInplace(char *buffer, int size) {
  auto insertPtr = buffer;
  auto parserPtr = buffer;
  auto endPtr = &buffer[size];
  while (parserPtr < endPtr && *parserPtr != '\0') {
    if (*parserPtr == '+') {
      *insertPtr = ' ';
    } else if (*parserPtr == '%') {
      parserPtr++;  // skip '%'
      if (parserPtr + 1 < endPtr) {
        *insertPtr = static_cast<char>(hexStringToInt(parserPtr, 2));
        parserPtr++;  // there are 2 bytes, so we shift one here
                      // decode %HEX
      } else {
        *insertPtr = '\0';
        parserPtr = endPtr;
        // invalid character at the end of buffer - may be result of
        // limitted buffer size -> truncate it
      }
    } else {
      *insertPtr = *parserPtr;
    }
    insertPtr++;
    parserPtr++;
  }
  *insertPtr = '\0';
}

int urlEncode(char *input, char *output, int outputMaxSize) {
  auto parserPtr = input;
  auto outputPtr = output;
  auto lastOutputPtr = output + outputMaxSize - 1;
  while (*parserPtr && outputPtr < lastOutputPtr) {
    if ((*parserPtr >= '0' && *parserPtr <= '9')
        || (*parserPtr >= 'A' && *parserPtr <= 'Z')
        || (*parserPtr >= 'a' && *parserPtr <= 'z')
        || *parserPtr == '-'
        || *parserPtr == '_'
        || *parserPtr == '.'
        || *parserPtr == '~') {
      *outputPtr++ = *parserPtr;
    } else {
      if (outputPtr + 3 <= lastOutputPtr) {
        *outputPtr++ = '%';
        outputPtr += generateHexString(parserPtr, outputPtr, 1, 0);
      } else {
        break;
      }
    }
    parserPtr++;
  }
  *outputPtr = '\0';

  return outputPtr - output;
}

int stringAppend(char *output, const char *input, int maxSize) {
  int inputSize = strlen(input);
  if (inputSize < maxSize) {
    strncpy(output, input, inputSize);
    return inputSize;
  }
  return 0;
}

