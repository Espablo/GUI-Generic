#ifndef Markup_h
#define Markup_h

#include "SuplaDeviceGUI.h"

void addFormHeader(String& html, const String& name);
void addFormHeaderEnd(String& html);
void addTextBox(String& html,
                const String& input_id,
                const String& name,
                uint8_t value_key,
                const String& placeholder,
                int minlength,
                int maxlength,
                bool required,
                bool readonly = false,
                bool password = false);
void addTextBox(String& html,
                const String& input_id,
                const String& name,
                uint8_t value_key,
                int minlength,
                int maxlength,
                bool required,
                bool readonly = false);
void addTextBoxPassword(
    String& html, const String& input_id, const String& name, uint8_t value_key, int minlength, int maxlength, bool required);

void addNumberBox(String& html, const String& input_id, const String& name, uint8_t value_key, uint16_t max);

void addListGPIOBox(String& html, const String& input_id, const String& name, uint8_t function, uint8_t nr = 0);

void addListGPIOLinkBox(String& html, const String& input_id, const String& name, uint8_t function, const String& url, uint8_t nr = 0);

void addListBox(String& html, const String& input_id, const String& name, const char* const* list_P, uint8_t size, uint8_t selected);

String addListGPIOSelect(const char* input, uint8_t function, uint8_t nr = 0);
#endif  // Markup_h
