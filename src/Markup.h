#ifndef Markup_h
#define Markup_h

#include "SuplaDeviceGUI.h"

void addFormHeader(String& html, const String& name);
void addFormHeaderEnd(String& html);
void addTextBox(String& html, const String& input_id, const String& value_key, const String& name, int minlength, int maxlength, bool required, bool readonly = false, bool password = false);
void addTextBoxPassword(String& html, const String& input_id, const String& value_key, const String& name, int minlength, int maxlength, bool required);

#endif  // Markup_h


