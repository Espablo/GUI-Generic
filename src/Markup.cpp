#include "Markup.h"
#include "SuplaCommonPROGMEM.h"

void addForm(String& html, const String& method, const String& action) {
  html += "<form method='" + method + "' action='" + action + "'>";
}

void addFormEnd(String& html) {
  html += "</form>";
}

void addFormHeader(String& html, const String& name) {
  html += F("<div class='w'>");
  if (name != "\n") {
    html += F("<h3>");
    html += name;
    html += F("</h3>");
  }
}

void addFormHeaderEnd(String& html) {
  html += F("</div>");
}

void addTextBox(String& html,
                const String& input_id,
                const String& name,
                uint8_t value_key,
                const String& placeholder,
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

  if (placeholder != "") {
    html += F("' placeholder='");
    html += placeholder;
  }

  html += F("' value='");
  String value = String(ConfigManager->get(value_key)->getValue());
  if (value != placeholder) {
    html += value;
  }
  if (minlength > 0) {
    html += F("' minlength='");
    html += minlength;
    html += F("'");
  }
  if (maxlength > 0) {
    html += F("' length='");
    html += maxlength;
    html += F("'");
  }
  if (readonly) {
    html += F(" readonly");
  }

  if (required) {
    html += F(" required");
  }

  html += F(" ><label>");
  html += name;
  html += F("</label></i> ");
}

void addTextBox(
    String& html, const String& input_id, const String& name, uint8_t value_key, int minlength, int maxlength, bool required, bool readonly) {
  return addTextBox(html, input_id, name, value_key, "", minlength, maxlength, required, readonly, false);
}

void addTextBoxPassword(String& html, const String& input_id, const String& name, uint8_t value_key, int minlength, int maxlength, bool required) {
  return addTextBox(html, input_id, name, value_key, "", minlength, maxlength, required, false, true);
}

void addNumberBox(String& html, const String& input_id, const String& name, uint8_t value_key, uint16_t max) {
  html += F("<i><label>");
  html += name;
  html += F("</label><input name='");
  html += input_id;
  html += F("' type='number' placeholder='0' step='1' min='0' max='");
  html += String(max);
  html += F("' value='");
  html += String(ConfigManager->get(value_key)->getValue());
  html += F("'></i>");
}

void addListGPIOBox(String& html, const String& input_id, const String& name, uint8_t function, uint8_t nr) {
  html += F("<i><label>");
  if (nr > 0) {
    html += nr;
    html += F(".");
  }
  html += F(" ");
  html += name;
  html += F("</label>");
  html += addListGPIOSelect(input_id.c_str(), function, nr);
  html += F("</i>");
}

void addListGPIOLinkBox(String& html, const String& input_id, const String& name, uint8_t function, const String& url, uint8_t nr) {
  uint8_t _nr;
  if (nr == 0) {
    _nr = 1;
  }
  else {
    _nr = nr;
  }
  html += F("<i>");
  html += F("<label>");
  if (ConfigESP->getGpio(_nr, function) != OFF_GPIO) {
    html += F("<a href='");
    html += PATH_START;
    html += url;
    if (nr > 0) {
      html += nr;
    }
    html += F("'>");
  }
  if (nr > 0) {
    html += nr;
    html += F(". ");
  }
  html += name;
  if (ConfigESP->getGpio(_nr, function) != OFF_GPIO) {
    html += WebServer->SuplaIconEdit();
    html += F("</a>");
  }
  html += F("</label>");
  html += addListGPIOSelect(input_id.c_str(), function, nr);
  html += F("</i>");
}

void addListBox(String& html, const String& input_id, const String& name, const char* const* array_P, uint8_t size, uint8_t selected) {
  html += F("<i><label>");
  html += name;
  html += "</label><select name='";
  html += input_id;
  html += F("'>");

  for (uint8_t suported = 0; suported < size; suported++) {
    html += F("<option value='");
    html += suported;
    if (selected == suported) {
      html += F("' selected>");
    }
    else {
      html += F("'>");
    }
    html += PGMT(array_P[suported]);
  }
  html += F("</select></i>");
}

void addButton(String& html, const String& name, const String& url) {
  html += F("<a href='");
  html += getURL(url);
  html += F("'><button>");
  html += name;
  html += F("</button></a>");
  html += F("<br><br>");
}

void addButtonSubmit(String& html, const String& name) {
  html += F("<button type='submit'>");
  html += name;
  html += F("</button>");
  html += F("<br><br>");
}

String addListGPIOSelect(const char* input, uint8_t function, uint8_t nr) {
  String page = "";
  page += F("<select name='");
  page += input;
  if (nr != 0) {
    page += nr;
  }
  else {
    nr = 1;
  }
  page += F("'>");

  uint8_t selected = ConfigESP->getGpio(nr, function);

  for (uint8_t suported = 0; suported < sizeof(GPIO_P) / sizeof(GPIO_P[0]); suported++) {
    if (ConfigESP->checkBusyGpio(suported, function) == false || selected == suported) {
      page += F("<option value='");
      page += suported;
      if (selected == suported) {
        page += F("' selected>");
      }
      else {
        page += F("'>");
      }
      page += PGMT(GPIO_P[suported]);
    }
  }
  page += F("</select>");
  return page;
}

String getURL(const String& url) {
  return String(F(PATH_START)) + url;
}

const String SuplaJavaScript(const String& java_return) {
  String java_script =
      F("<script type='text/javascript'>setTimeout(function(){var "
        "element=document.getElementById('msg');if( element != "
        "null){element.style.visibility='hidden';location.href='");
  java_script += java_return;
  java_script += F("';}},1600);</script>\n");
#ifdef SUPLA_OTA
  java_script +=
      F("<script type='text/javascript'>if(window.top.location != window.location){"
        "window.top.location.href = window.location.href;}</script>\n");
#endif
  return java_script;
}

const String SuplaSaveResult(int save) {
  if (save == 0)
    return F("");
  String saveresult = "";
  saveresult += F("<div id=\"msg\" class=\"c\">");
  if (save == 1) {
    saveresult += S_DATA_SAVED;
  }
  else if (save == 2) {
    saveresult += S_RESTART_MODULE;
  }
  else if (save == 3) {
    saveresult += S_DATA_ERASED_RESTART_DEVICE;
  }
  else if (save == 4) {
    saveresult += S_WRITE_ERROR_UNABLE_TO_READ_FILE_FS_PARTITION_MISSING;
  }
  else if (save == 5) {
    saveresult += S_DATA_SAVED_RESTART_MODULE;
  }
  else if (save == 6) {
    saveresult += S_WRITE_ERROR_BAD_DATA;
  }
  else if (save == 7) {
    saveresult += F("data saved");
  }
  saveresult += F("</div>");
  return saveresult;
}