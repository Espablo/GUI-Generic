#include "Markup.h"

void addFormHeader(String& html, const String& name) {
  html += F("<div class='w'>");
  html += F("<h3>");
  html += name;
  html += F("</h3>");
}

void addFormHeaderEnd(String& html) {
  html += F("</div>");
}

void addTextBox(String& html,
                const String& input_id,
                const String& value_key,
                const String& name,
                int minlength,
                int maxlength,
                bool required,
                bool readonly,
                bool password) {
  html += F("<i><input name='");
  html += input_id;
  if (password) {
    if (ConfigESP->configModeESP != NORMAL_MODE) {
      html += F("' type='password' ");
    }
  }
  html += F("' value='");
  html += String(ConfigManager->get(value_key.c_str())->getValue());
  if (minlength > 0) {
    html += F("'minlength='");
    html += minlength;
  }
  if (maxlength > 0) {
    html += F("' length=");
    html += maxlength;
  }
  if (readonly) {
    html += F(" readonly ");
  }

  if (required) {
    html += F(" required ");
  }

  html += F("><label>");
  html += name;
  html += F("</label></i> ");
}

void addTextBoxPassword(
    String& html, const String& input_id, const String& value_key, const String& name, int minlength, int maxlength, bool required) {
  return addTextBox(html, input_id, value_key, name, minlength, maxlength, required, false, true);
}