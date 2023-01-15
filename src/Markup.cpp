/*
  Copyright (C) krycha88

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

void addBr(String& html) {
  html += F("<br>");
}

void addLabel(String& html, const String& name) {
  html += F("<i><label>");
  html += name;
  html += F("</label></i>");
}

void addTextBox(String& html,
                const String& input_id,
                const String& name,
                const String& value,
                const String& placeholder,
                int minlength,
                int maxlength,
                bool required,
                bool readonly,
                bool password,
                bool underline) {
  if (underline) {
    html += F("<i>");
  }
  else {
    html += F("<i style='border-bottom:none !important;'>");
  }

  html += F("<input name='");
  html += input_id;
  if (password) {
    if (ConfigESP->configModeESP != Supla::DEVICE_MODE_NORMAL) {
      html += F("' type='password");
    }
  }

  html += F("' value='");
  if (value != placeholder) {
    html += value;
  }

  if (placeholder != "") {
    html += F("' placeholder='");
    html += placeholder;
  }

  if (minlength > 0) {
    html += F("' minlength='");
    html += minlength;
  }
  if (maxlength > 0) {
    html += F("' maxlength='");
    html += maxlength;
  }
  html += F("'");
  if (readonly) {
    html += F(" readonly");
  }

  if (required) {
    html += F(" required");
  }

  html += F("><label>");
  html += name;
  html += F("</label></i> ");
  WebServer->sendHeader();
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
  String value = String(ConfigManager->get(value_key)->getValue());
  return addTextBox(html, input_id, name, value, placeholder, minlength, maxlength, required, readonly, password);
}

void addTextBox(
    String& html, const String& input_id, const String& name, uint8_t value_key, int minlength, int maxlength, bool required, bool readonly) {
  return addTextBox(html, input_id, name, value_key, "", minlength, maxlength, required, readonly, false);
}

void addTextBox(
    String& html, const String& input_id, const String& name, const String& value, int minlength, int maxlength, bool required, bool readonly) {
  return addTextBox(html, input_id, name, value, "", minlength, maxlength, required, readonly, false);
}

void addTextBox(String& html, const String& value) {
  html += F("<style><input[name='board']{padding-left: 48px;width: calc(100% - 52px);}</style>");
  html += F("<p style='color:#000;'>");
  html += value;
  html += F("</p>");
}
void addTextBoxPassword(String& html, const String& input_id, const String& name, uint8_t value_key, int minlength, int maxlength, bool required) {
  return addTextBox(html, input_id, name, value_key, "", minlength, maxlength, required, false, true);
}

void addCheckBox(String& html, const String& input_id, const String& name, bool checked) {
  html += F("<i><label>");
  html += name;
  html += F("</label><input type='checkbox' name='");
  html += input_id;
  html += F("'");
  if (checked) {
    html += F(" checked");
  }
  html += F("></i>");
}

void addNumberBox(String& html, const String& input_id, const String& name, uint8_t value_key, int max) {
  html += F("<i><label>");
  html += name;
  html += F("</label><input name='");
  html += input_id;
  html += F("' type='number' placeholder='0' step='1' min='0'");

  if (max >= 0) {
    html += F(" max='");
    html += String(max);
    html += F("'");
  }

  html += F(" value='");
  html += String(ConfigManager->get(value_key)->getValue());
  html += F("'></i>");
  WebServer->sendHeader();
}

void addNumberBox(String& html, const String& input_id, const String& name, const String& placeholder, bool required, const String& value) {
  html += F("<i><label>");
  html += name;
  html += F("</label><input name='");
  html += input_id;
  html += F("' type='number'");
  if (!placeholder.isEmpty()) {
    html += F(" placeholder='");
    html += placeholder.c_str();
    html += F("'");
  }
  html += F(" step='0.01' value='");
  html += value.c_str();
  html += F("'");

  if (required) {
    html += F(" required");
  }

  html += F("></i>");
  WebServer->sendHeader();
}

void addLinkBox(String& html, const String& name, const String& url) {
  html += F("<i>");
  html += F("<label>");
  html += F("<a href='");
  html += PATH_START;
  html += url;
  html += F("'>");
  html += name;
  // html += FPSTR(ICON_EDIT);
  html += F("</a>");
  html += F("</label>");
  html += F("</i>");
  WebServer->sendHeader();
}

void addHyperlink(String& html, const String& name, const String& url) {
  html += F("<i>");
  html += F("<label>");
  html += F("<a href='");
  html += url;
  html += F("' target='_self'>");
  html += name;
  html += F("</a>");
  html += F("</label>");
  html += F("</i>");
  WebServer->sendHeader();
}

void addListGPIOLinkBox(String& html, const String& input_id, const String& name, const String& url, uint8_t function) {
  addListGPIOBox(html, input_id, name, function, 0, true, url, true);
}

void addListGPIOLinkBox(String& html, const String& input_id, const String& name, const String& url, uint8_t function, uint8_t nr, bool no_number) {
  addListGPIOBox(html, input_id, name, function, nr, true, url, no_number);
}

void addListGPIOLinkBox(String& html, const String& input_id, const String& name, const String& url, uint8_t function, uint8_t nr) {
  addListGPIOBox(html, input_id, name, function, nr, true, url);
}

void addListGPIOBox(String& html, const String& input_id, const String& name, uint8_t function) {
  addListGPIOBox(html, input_id, name, function, 0, true, "", true);
}

void addListGPIOBox(
    String& html, const String& input_id, const String& name, uint8_t function, uint8_t nr, bool underline, const String& url, bool no_number) {
  uint8_t gpio;

  gpio = ConfigESP->getGpio(nr, function);

  if (underline) {
    html += F("<i>");
  }
  else {
    html += F("<i style='border-bottom:none !important;'>");
  }
  html += F("<label>");

  if (!url.isEmpty() && gpio != OFF_GPIO) {
    html += F("<a href='");
    html += PATH_START;
    html += url;
    html += nr;
    html += F("'>");

    if (!no_number) {
      html += nr + 1;
      html += F(".");
    }

    html += F(" ");
    html += name;
    html += FPSTR(ICON_EDIT);
    html += F("</a>");
    WebServer->sendHeader();
  }
  else {
    if (!no_number) {
      html += nr + 1;
      html += F(".");
    }

    html += F(" ");
    html += name;
  }

  html += F("</label>");

  html += F("<select name='");
  html += input_id;
  html += nr;
  html += F("'>");

  if (function == FUNCTION_RELAY && nr < MAX_VIRTUAL_RELAY)
    addGPIOOptionValue(html, GPIO_VIRTUAL_RELAY, gpio, String(S_SPACE) + "VIRTUAL");

#ifdef ARDUINO_ARCH_ESP8266
  for (uint8_t suported = 0; suported <= OFF_GPIO; suported++)
    if (ConfigESP->checkBusyGpio(suported, function) || suported == gpio)
      addGPIOOptionValue(html, suported, gpio, FPSTR(GPIO_P[suported]));

#elif ARDUINO_ARCH_ESP32

  for (uint8_t suported = 0; suported <= OFF_GPIO; suported++)
    if ((ConfigESP->checkBusyGpio(suported, function) || suported == gpio))
      addGPIOOptionValue(html, suported, gpio, FPSTR(GPIO_P[suported]));

#endif
  WebServer->sendHeader();
  html += F("</select>");

  html += F("</i>");
}

void addGPIOOptionValue(String& html, uint8_t gpio, uint8_t selectedGpio, const String& name) {
  html += F("<option value='");
  html += gpio;
  html += F("'");

  if (gpio == selectedGpio) {
    html += F(" selected");
  }
  if (gpio == OFF_GPIO) {
    html += F(">");
  }
  else {
    html += F("> GPIO");
  }
  html += name;
}

#ifdef GUI_SENSOR_I2C_EXPENDER

void addListExpanderBox(String& html, const String& input_id, const String& name, uint8_t function, uint8_t nr, const String& url) {
  uint8_t type = ConfigManager->get(KEY_ACTIVE_EXPENDER)->getElement(function).toInt();

  if (nr == 0) {
    addListBox(html, INPUT_EXPENDER_TYPE, S_TYPE, EXPENDER_LIST_P, 4, type);
  }

  if (ConfigESP->checkActiveMCP23017(function)) {
    addListExpanderGPIOBox(webContentBuffer, input_id, name, function, nr, url);
  }
  else {
    addListGPIOLinkBox(webContentBuffer, input_id, name, getParameterRequest(url, ARG_PARM_NUMBER), function, nr);
  }
}

void addListExpanderGPIOBox(String& html, const String& input_id, const String& name, uint8_t function, uint8_t nr, const String& url) {
  uint8_t address, type, maxNr;
  const char* const* listAdressExpender;
  const char* const* listExpender;

  type = ConfigManager->get(KEY_ACTIVE_EXPENDER)->getElement(function).toInt();

  if (type == EXPENDER_PCF8574) {
    maxNr = 8;
    listAdressExpender = EXPENDER_PCF8574_P;
    listExpender = GPIO_PCF_8574_P;
  }
  else {
    maxNr = 16;
    listAdressExpender = EXPENDER_P;
    listExpender = GPIO_MCP23017_P;
  }

  if (nr == 0 || nr == maxNr) {
    for (uint8_t gpio = nr; gpio <= OFF_GPIO_EXPENDER; gpio++) {
      address = ConfigESP->getAdressMCP23017(gpio, function);
      if (address != OFF_ADDRESS_MCP23017) {
        break;
      }
    }

    if (url != "")
      addListLinkBox(html, String(INPUT_ADRESS_MCP23017) + nr, String(S_ADDRESS) + S_SPACE + 1, listAdressExpender, 5, address, url);
    else
      addListBox(html, String(INPUT_ADRESS_MCP23017) + nr, String(S_ADDRESS) + S_SPACE + 1, listAdressExpender, 5, address);
  }

  addListExpanderGPIO(webContentBuffer, input_id, name, function, nr, listExpender, 18, getParameterRequest(url, ARG_PARM_NUMBER) + nr);
}

void addListExpanderGPIO(String& html,
                         const String& input_id,
                         const String& name,
                         uint8_t function,
                         uint8_t nr,
                         const char* const* array_P,
                         uint8_t size,
                         const String& url) {
  html += F("<i><label><a href='");
  html += PATH_START;
  html += url;
  html += F("'>");
  html += nr + 1;
  html += F(". ");
  html += name;
  html += F("</a>");
  html += "</label><select name='";
  html += input_id;
  html += "mcp";
  html += nr;
  html += F("'>");

  uint8_t selected = ConfigESP->getGpioMCP23017(nr, function);

  for (uint8_t suported = 0; suported < size; suported++) {
    if (!String(FPSTR(array_P[suported])).isEmpty()) {
      if (ConfigESP->checkBusyGpioMCP23017(suported, nr, function) || selected == suported) {
        html += F("<option value='");
        html += suported;
        html += F("'");
        if (selected == suported) {
          html += F(" selected");
        }
        html += F(">");
        html += FPSTR(array_P[suported]);
      }
    }
  }
  WebServer->sendHeader();
  html += F("</select></i>");
}
#endif

void addListBox(String& html, const String& input_id, const String& name, const char* const* array_P, uint8_t size, uint8_t selected, uint8_t nr) {
  html += F("<i><label>");
  if (nr != 0) {
    html += nr;
    html += F(". ");
  }
  html += name;
  html += "</label><select name='";
  html += input_id;
  if (nr != 0) {
    html += nr;
  }
  html += F("'>");

  for (uint8_t suported = 0; suported < size; suported++) {
    if (!String(FPSTR(array_P[suported])).isEmpty()) {
      html += F("<option value='");
      html += suported;
      html += F("'");
      if (selected == suported) {
        html += F(" selected");
      }
      html += F(">");
      html += FPSTR(array_P[suported]);
    }
  }
  WebServer->sendHeader();
  html += F("</select></i>");
}

void addListNumbersBox(String& html, const String& input_id, const String& name, uint8_t size, uint8_t selected) {
  html += F("<i><label>");
  html += name;
  html += "</label><select name='";
  html += input_id;
  html += F("'>");

  for (uint8_t suported = 0; suported < size; suported++) {
    html += F("<option value='");
    html += suported;
    html += F("'");
    if (selected == suported) {
      html += F(" selected");
    }
    html += F(">");
    html += (suported + 1);
  }
  WebServer->sendHeader();
  html += F("</select></i>");
}

void addListLinkBox(String& html,
                    const String& input_id,
                    const String& name,
                    const char* const* array_P,
                    uint8_t size,
                    uint8_t selected,
                    const String& url,
                    uint8_t nr) {
  html += F("<i><label><a href='");
  html += PATH_START;
  html += url;
  html += F("'>");

  if (nr != 0) {
    html += nr;
    html += F(". ");
  }
  html += name;
  html += F("</a>");
  html += "</label><select name='";
  html += input_id;
  if (nr != 0) {
    html += nr;
  }
  html += F("'>");

  for (uint8_t suported = 0; suported < size; suported++) {
    if (!String(FPSTR(array_P[suported])).isEmpty()) {
      html += F("<option value='");
      html += suported;
      html += F("'");
      if (selected == suported) {
        html += F(" selected");
      }
      html += F(">");
      html += FPSTR(array_P[suported]);
    }
  }
  WebServer->sendHeader();
  html += F("</select></i>");
}

void addButton(String& html, const String& name, const String& url) {
  html += F("<a href='");
  html += getURL(url);
  html += F("'><button>");
  html += name;
  html += F("</button></a>");
  html += F("<br><br>");
  WebServer->sendHeader();
}

void addButtonSubmit(String& html, const String& name) {
  html += F("<button type='submit'>");
  html += name;
  html += F("</button>");
  html += F("<br><br>");
  WebServer->sendHeader();
}

String getURL(const String& url) {
  return String(PATH_START) + url;
}

String getURL(const String& url, uint8_t nr) {
  return String(PATH_START) + url + nr;
}

String getInput(const String& input, uint8_t nr) {
  return input + nr;
}

String getParameterRequest(const String& url, const String& param, const String& value) {
  return url + String(F("?")) + param + String(F("=")) + value;
}

const String SuplaJavaScript(const String& java_return) {
  String java_script =
      F("<script type='text/javascript'>setTimeout(function(){var element=document.getElementById('msg');if(element != "
        "null){element.style.visibility='hidden';var url = window.location.pathname + window.location.search; if(url != '/");
  java_script += java_return;
  java_script += F("'){location.href='");
  java_script += java_return;
  java_script += F("'};}},4000);");
  java_script += F("if(window.top.location != window.location){window.top.location.href = window.location.href;}</script>\n");
  return java_script;
}

// TODO: @krycha88 Usunąć z SuplaSaveResult nieużywany status WRITE_ERROR_UNABLE_TO_READ_FILE_FS_PARTITION_MISSING```
const String SuplaSaveResult(int save) {
  String saveresult = "";
  saveresult += F("<div id=\"msg\" class=\"c\">");

  switch (save) {
    case SaveResult::DATA_SAVE:
      saveresult += S_DATA_SAVED;
      break;
    case SaveResult::RESTART_MODULE:
      saveresult += S_RESTART_MODULE;
      break;
    case SaveResult::DATA_ERASED_RESTART_DEVICE:
      saveresult += S_DATA_ERASED_RESTART_DEVICE; // do usunięcia
      break;
    case SaveResult::WRITE_ERROR_UNABLE_TO_READ_FILE_FS_PARTITION_MISSING: // do usunięcia
      saveresult += S_WRITE_ERROR_UNABLE_TO_READ_FILE_FS_PARTITION_MISSING;
      break;
    case SaveResult::DATA_SAVED_RESTART_MODULE:
      saveresult += S_DATA_SAVED_RESTART_MODULE;
      break;
    case SaveResult::WRITE_ERROR_BAD_DATA:
      saveresult += S_WRITE_ERROR_BAD_DATA;
      break;
    case SaveResult::DATA_SAVE_MODE_CONFIG:
      saveresult += F("data saved");
      break;
    case SaveResult::UPDATE_SUCCESS:
      saveresult += F("Aktualizacja zakończona.");
      break;
    case SaveResult::UPDATE_ERROR:
      saveresult += F("Błąd aktualizacji.");
      break;
    case SaveResult::UPDATE_WAIT:
      saveresult += F("Zostanie wygenerowana nowa wersja. Spróbuj ponownie za 5min.");
      break;
    case SaveResult::UPDATE_NO_UPDATES:
      saveresult += F("Brak aktualizacji.");
      break;
    case SaveResult::UPDATE_TOO_LESS_SPACE:
      saveresult += F("Wykonaj aktualizację 2 etapową.");
      break;

    default:
      return F("");
      break;
  }

  saveresult += F("</div>");
  return saveresult;
}

#ifdef SUPLA_PUSHOVER
namespace Html {
void addPushover(uint8_t nr) {
  if (nr <= MAX_PUSHOVER_MESSAGE) {
    addFormHeader(webContentBuffer, S_PUSHOVER);

    uint8_t selected = ConfigManager->get(KEY_PUSHOVER_SOUND)->getElement(nr).toInt();
    addListBox(webContentBuffer, INPUT_PUSHOVER_SOUND, S_SOUND, PUSHOVER_SOUND_LIST_P, Supla::PushoverSound::SOUND_COUNT, selected);

    String massage = ConfigManager->get(KEY_PUSHOVER_MASSAGE)->getElement(nr).c_str();
    addTextBox(webContentBuffer, INPUT_PUSHOVER_MESSAGE, S_MESSAGE, massage, 0, MAX_MESSAGE_SIZE, false);
    addFormHeaderEnd(webContentBuffer);
  }
}
}  // namespace Html
#endif