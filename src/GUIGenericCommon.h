#ifndef GUI_GENERIC_COMMON_H
#define GUI_GENERIC_COMMON_H

#include "language/common.h"
#ifndef UI_LANGUAGE
#define UI_LANGUAGE pl
#endif
#define QUOTE(x)        QUOTE_1(x)
#define QUOTE_1(x)      #x
#define INCLUDE_FILE(x) QUOTE(language/x.h)
#include INCLUDE_FILE(UI_LANGUAGE)

#include "SuplaDeviceGUI.h"

uint8_t *HexToBytes(String _value);
int getCountSensorChannels();

#endif  // GUI_GENERIC_COMMON_H
