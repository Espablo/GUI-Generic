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

#include "SuplaTemplateBoard.h"
#include "SuplaDeviceGUI.h"

#ifdef SUPLA_TEMPLATE_BOARD_JSON

namespace Supla {
namespace TanplateBoard {

void chooseTemplateBoard(String board) {
  bool newVersion = false;

  ConfigESP->clearEEPROM();
  ConfigManager->deleteGPIODeviceValues();
  templateBoardWarning = "";

  ConfigManager->set(KEY_MAX_BUTTON, "0");
  ConfigManager->set(KEY_MAX_RELAY, "0");
  ConfigManager->set(KEY_MAX_LIMIT_SWITCH, "0");
  ConfigManager->set(KEY_MAX_RGBW, "0");
  ConfigManager->set(KEY_CFG_MODE, CONFIG_MODE_10_ON_PRESSES);

  const size_t capacity = JSON_ARRAY_SIZE(14) + JSON_OBJECT_SIZE(4) + 200;
  DynamicJsonBuffer jsonBuffer(capacity);

  JsonObject& root = jsonBuffer.parseObject(board);
  JsonArray& GPIO = root["GPIO"];

  if (GPIO.size() == 14)
    newVersion = true;

  templateBoardWarning += F("Wersja: ") + String(newVersion) + F("<br>");
  if (GPIO.size() == 0) {
    templateBoardWarning += "Błąd wczytania<br>";
    return;
  }

  String name = root["NAME"];
  ConfigManager->set(KEY_HOST_NAME, name.c_str());

  for (size_t i = 0; i < GPIO.size(); i++) {
    int gpioJSON = (int)GPIO[i];
    int gpio = getGPIO(i);

    if (newVersion) {
      switch (gpioJSON) {
        case FunctionOld::Relay1 + Shift::Relay:
        case FunctionOld::Relay2 + Shift::Relay:
        case FunctionOld::Relay3 + Shift::Relay:
        case FunctionOld::Relay4 + Shift::Relay:
          gpioJSON = gpioJSON - Shift::Relay;
          break;

        case FunctionOld::Relay1i + Shift::Relayi:
        case FunctionOld::Relay2i + Shift::Relayi:
        case FunctionOld::Relay3i + Shift::Relayi:
        case FunctionOld::Relay4i + Shift::Relayi:
          gpioJSON = gpioJSON - Shift::Relayi;
          break;

        case FunctionOld::Button1 + Shift::Button:
        case FunctionOld::Button2 + Shift::Button:
        case FunctionOld::Button3 + Shift::Button:
        case FunctionOld::Button4 + Shift::Button:
          gpioJSON = gpioJSON - Shift::Button;
          break;

        case FunctionOld::Button1n + Shift::Buttonn:
        case FunctionOld::Button2n + Shift::Buttonn:
        case FunctionOld::Button3n + Shift::Buttonn:
        case FunctionOld::Button4n + Shift::Buttonn:
          gpioJSON = gpioJSON - Shift::Buttonn;
          break;

        case FunctionOld::Led1 + Shift::Led:
        case FunctionOld::Led2 + Shift::Led:
        case FunctionOld::Led3 + Shift::Led:
        case FunctionOld::Led4 + Shift::Led:
          gpioJSON = gpioJSON - Shift::Led;
          break;
        case FunctionOld::LedLink + 387:
          gpioJSON = gpioJSON - 387;
          break;

        case FunctionOld::Led1i + Shift::Ledi:
        case FunctionOld::Led2i + Shift::Ledi:
        case FunctionOld::Led3i + Shift::Ledi:
        case FunctionOld::Led4i + Shift::Ledi:
          gpioJSON = gpioJSON - Shift::Ledi;
          break;
        case FunctionOld::LedLinki + 418:
          gpioJSON = gpioJSON - 418;
          break;

        case FunctionOld::BL0937CF + 2586:
          gpioJSON = gpioJSON - 2586;
          break;
        case FunctionOld::HLWBLCF1 + 2524:
          gpioJSON = gpioJSON - 2524;
          break;
        case FunctionOld::HLWBLSELi + 2493:
          gpioJSON = gpioJSON - 2493;
          break;
      }
    }

    switch (gpioJSON) {
      case FunctionOld::None:
        break;
      case FunctionOld::Users:
        break;
      case FunctionOld::Relay1:
        Supla::TanplateBoard::addRelay(0, gpio);
        break;
      case FunctionOld::Relay2:
        Supla::TanplateBoard::addRelay(1, gpio);
        break;
      case FunctionOld::Relay3:
        Supla::TanplateBoard::addRelay(2, gpio);
        break;
      case FunctionOld::Relay4:
        Supla::TanplateBoard::addRelay(3, gpio);
        break;

      case FunctionOld::Relay1i:
        Supla::TanplateBoard::addRelay(0, gpio, LOW);
        break;
      case FunctionOld::Relay2i:
        Supla::TanplateBoard::addRelay(1, gpio, LOW);
        break;
      case FunctionOld::Relay3i:
        Supla::TanplateBoard::addRelay(2, gpio, LOW);
        break;
      case FunctionOld::Relay4i:
        Supla::TanplateBoard::addRelay(3, gpio, LOW);
        break;

      case FunctionOld::Button1:
        Supla::TanplateBoard::addButton(0, gpio);
        Supla::TanplateBoard::addButtonCFG(gpio);
        break;
      case FunctionOld::Button2:
        Supla::TanplateBoard::addButton(1, gpio);
        break;
      case FunctionOld::Button3:
        Supla::TanplateBoard::addButton(2, gpio);
        break;
      case FunctionOld::Button4:
        Supla::TanplateBoard::addButton(3, gpio);
        break;

      case FunctionOld::Button1n:
        Supla::TanplateBoard::addButton(0, gpio, Supla::Event::ON_PRESS, Supla::Action::TOGGLE, false, true);
        Supla::TanplateBoard::addButtonCFG(gpio);
        break;
      case FunctionOld::Button2n:
        Supla::TanplateBoard::addButton(1, gpio, Supla::Event::ON_PRESS, Supla::Action::TOGGLE, false, true);
        break;
      case FunctionOld::Button3n:
        Supla::TanplateBoard::addButton(2, gpio, Supla::Event::ON_PRESS, Supla::Action::TOGGLE, false, true);
        break;
      case FunctionOld::Button4n:
        Supla::TanplateBoard::addButton(3, gpio, Supla::Event::ON_PRESS, Supla::Action::TOGGLE, false, true);
        break;

      case FunctionOld::Led1:
        // pierwszy led przeznaczony jest dla LED konfiguracyjnego
        Supla::TanplateBoard::addLedCFG(gpio);
        break;
      case FunctionOld::Led2:
        Supla::TanplateBoard::addLed(0, gpio);
        break;
      case FunctionOld::Led3:
        Supla::TanplateBoard::addLed(1, gpio);
        break;
      case FunctionOld::Led4:
        Supla::TanplateBoard::addLed(2, gpio);
        break;
      case FunctionOld::LedLink:
        Supla::TanplateBoard::addLed(0, gpio);
        break;

      case FunctionOld::Led1i:
        Supla::TanplateBoard::addLedCFG(gpio, LOW);
        break;
      case FunctionOld::Led2i:
        Supla::TanplateBoard::addLed(1, gpio, LOW);
        break;
      case FunctionOld::Led3i:
        Supla::TanplateBoard::addLed(2, gpio, LOW);
        break;
      case FunctionOld::Led4i:
        Supla::TanplateBoard::addLed(3, gpio, LOW);
        break;
      case FunctionOld::LedLinki:
        Supla::TanplateBoard::addLed(0, gpio, LOW);
        break;

      case FunctionOld::BL0937CF:
        ConfigESP->setGpio(gpio, FUNCTION_CF);
        break;
      case FunctionOld::HLWBLCF1:
        ConfigESP->setGpio(gpio, FUNCTION_CF1);
        break;
      case FunctionOld::HLWBLSELi:
        ConfigESP->setGpio(gpio, FUNCTION_SEL);
        break;

      default:
        templateBoardWarning += F("Brak funkcji: ") + String(gpioJSON) + F("<br>");
    }
  }
}

uint8_t getGPIO(uint8_t gpio) {
  if (gpio == 6 || gpio == 7)
    gpio = gpio + 3;
  else if (gpio >= 8)
    gpio = gpio + 4;

  return gpio;
}

void addButton(uint8_t nr, uint8_t gpio, uint8_t event, uint8_t action, bool pullUp, bool invertLogic) {
  ConfigESP->setEvent(gpio, event);
  ConfigESP->setAction(gpio, action);
  ConfigESP->setPullUp(gpio, pullUp);
  ConfigESP->setInversed(gpio, invertLogic);

  ConfigESP->setGpio(gpio, nr, FUNCTION_BUTTON);
  ConfigManager->set(KEY_MAX_BUTTON, nr + 1);
}

void addRelay(uint8_t nr, uint8_t gpio, uint8_t level) {
  ConfigESP->setLevel(gpio, level);
  ConfigESP->setMemory(gpio, MEMORY_RELAY_RESTORE);

  ConfigESP->setGpio(gpio, nr, FUNCTION_RELAY);
  ConfigManager->set(KEY_MAX_RELAY, nr + 1);
}

void addLedCFG(uint8_t gpio, uint8_t level) {
  ConfigESP->setLevel(gpio, level);
  ConfigESP->setGpio(gpio, FUNCTION_CFG_LED);
}

void addLed(uint8_t nr, uint8_t gpio, uint8_t level) {
  ConfigESP->setInversed(gpio, level);
  ConfigESP->setGpio(gpio, nr, FUNCTION_LED);
}

void addButtonCFG(uint8_t gpio) {
  ConfigESP->setGpio(gpio, FUNCTION_CFG_BUTTON);
}

String templateBoardWarning;

}  // namespace TanplateBoard
}  // namespace Supla
#else
void addButton(uint8_t gpio, uint8_t event, uint8_t action, bool pullUp, bool invertLogic) {
  uint8_t nr = ConfigManager->get(KEY_MAX_BUTTON)->getValueInt();

  ConfigESP->setEvent(gpio, event);
  ConfigESP->setAction(gpio, action);
  ConfigESP->setPullUp(gpio, pullUp);
  ConfigESP->setInversed(gpio, invertLogic);

  ConfigESP->setGpio(gpio, nr, FUNCTION_BUTTON);
  ConfigManager->set(KEY_MAX_BUTTON, nr + 1);
}

void addRelay(uint8_t gpio, uint8_t level) {
  uint8_t nr = ConfigManager->get(KEY_MAX_RELAY)->getValueInt();

  ConfigESP->setLevel(gpio, level);
  ConfigESP->setMemory(gpio, MEMORY_RELAY_RESTORE);
  ConfigESP->setGpio(gpio, nr, FUNCTION_RELAY);
  ConfigManager->set(KEY_MAX_RELAY, nr + 1);
}

void addLimitSwitch(uint8_t gpio) {
  uint8_t nr = ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt();

  ConfigESP->setGpio(gpio, nr, FUNCTION_LIMIT_SWITCH);
  ConfigManager->set(KEY_MAX_LIMIT_SWITCH, nr + 1);
}

void addLedCFG(uint8_t gpio, uint8_t level) {
  ConfigESP->setLevel(gpio, level);
  ConfigESP->setGpio(gpio, FUNCTION_CFG_LED);
}

void addLed(uint8_t gpio) {
  ConfigESP->setGpio(gpio, FUNCTION_LED);
}

void addButtonCFG(uint8_t gpio) {
  ConfigESP->setGpio(gpio, FUNCTION_CFG_BUTTON);
}

#ifdef SUPLA_HLW8012
void addHLW8012(int8_t pinCF, int8_t pinCF1, int8_t pinSEL) {
  ConfigESP->setGpio(pinCF, FUNCTION_CF);
  ConfigESP->setGpio(pinCF1, FUNCTION_CF1);
  ConfigESP->setGpio(pinSEL, FUNCTION_SEL);
  // Supla::GUI::addHLW8012(ConfigESP->getGpio(FUNCTION_CF), ConfigESP->getGpio(FUNCTION_CF1), ConfigESP->getGpio(FUNCTION_SEL));
}
#endif

void addRGBW(int8_t redPin, int8_t greenPin, int8_t bluePin, int8_t brightnessPin) {
  uint8_t nr = ConfigManager->get(KEY_MAX_RGBW)->getValueInt();

  ConfigESP->setGpio(redPin, nr, FUNCTION_RGBW_RED);
  ConfigESP->setGpio(greenPin, nr, FUNCTION_RGBW_GREEN);
  ConfigESP->setGpio(bluePin, nr, FUNCTION_RGBW_BLUE);
  ConfigESP->setGpio(brightnessPin, nr, FUNCTION_RGBW_BRIGHTNESS);
  ConfigManager->set(KEY_MAX_RGBW, nr + 1);
}

void addDimmer(int8_t brightnessPin) {
  addRGBW(OFF_GPIO, OFF_GPIO, OFF_GPIO, brightnessPin);
}

void chooseTemplateBoard(uint8_t board) {
  ConfigESP->clearEEPROM();
  ConfigManager->deleteGPIODeviceValues();

  ConfigManager->set(KEY_BOARD, board);
  ConfigManager->set(KEY_MAX_BUTTON, "0");
  ConfigManager->set(KEY_MAX_RELAY, "0");
  ConfigManager->set(KEY_MAX_LIMIT_SWITCH, "0");
  ConfigManager->set(KEY_MAX_RGBW, "0");
  ConfigManager->set(KEY_CFG_MODE, CONFIG_MODE_10_ON_PRESSES);

  switch (board) {
    case BOARD_ELECTRODRAGON:
      addLedCFG(16);
      addButtonCFG(0);
      addButton(0);
      addButton(2);
      addRelay(12);
      addRelay(13);
      break;
    case BOARD_INCAN3:
      addLedCFG(2, LOW);
      addButtonCFG(0);
      addButton(14, Supla::Event::ON_CHANGE);
      addButton(12, Supla::Event::ON_CHANGE);
      addRelay(5);
      addRelay(13);
      addLimitSwitch(4);
      addLimitSwitch(16);
      break;
    case BOARD_INCAN4:
      addLedCFG(12);
      addButtonCFG(0);
      addButton(2, Supla::Event::ON_CHANGE);
      addButton(10, Supla::Event::ON_CHANGE);
      addRelay(4);
      addRelay(14);
      addLimitSwitch(4);
      addLimitSwitch(16);
      break;
    case BOARD_MELINK:
      addLedCFG(12);
      addButtonCFG(5);
      addButton(5);
      addRelay(4);
      break;
    case BOARD_NEO_COOLCAM:
      addLedCFG(4);
      addButtonCFG(13);
      addButton(13);
      addRelay(12);
      break;
    case BOARD_SHELLY1:
      addButtonCFG(5);
      addButton(5, Supla::Event::ON_PRESS, Supla::Action::TOGGLE, false, true);
      addRelay(4);
      break;
    case BOARD_SHELLY2:
      addLedCFG(16);
      addButtonCFG(12);
      addButton(12, Supla::Event::ON_PRESS, Supla::Action::TOGGLE, false, true);
      addButton(14, Supla::Event::ON_PRESS, Supla::Action::TOGGLE, false, true);
      addRelay(4);
      addRelay(5);
      break;
    case BOARD_SONOFF_BASIC:
      addLedCFG(13);
      addButtonCFG(0);
      addButton(0);
      addRelay(12);
      break;
    case BOARD_SONOFF_MINI:
      addLedCFG(13);
      addButtonCFG(4);
      addButton(4, Supla::Event::ON_PRESS, Supla::Action::TOGGLE);
      addRelay(12);
      break;
    case BOARD_SONOFF_DUAL_R2:
      addLedCFG(13);
      addButtonCFG(0);
      addButton(0);
      addButton(9);
      addRelay(12);
      addRelay(5);
      break;
    case BOARD_SONOFF_S2X:
      addLedCFG(13);
      addButtonCFG(0);
      addButton(0);
      addRelay(12);
      break;
    case BOARD_SONOFF_SV:
      addLedCFG(13);
      addButtonCFG(0);
      addButton(0);
      addRelay(12);
      addLimitSwitch(4);
      break;
    case BOARD_SONOFF_TH:
      addLedCFG(13);
      addButtonCFG(0);
      addButton(0);
      addRelay(12);
      ConfigESP->setGpio(14, FUNCTION_SI7021_SONOFF);
      break;
    case BOARD_SONOFF_TOUCH:
      addLedCFG(13);
      addButtonCFG(0);
      addButton(0);
      addRelay(12);
      break;
    case BOARD_SONOFF_TOUCH_2CH:
      addLedCFG(13);
      addButtonCFG(0);
      addButton(0);
      addButton(9);
      addRelay(12);
      addRelay(5);
      break;
    case BOARD_SONOFF_TOUCH_3CH:
      addLedCFG(13);
      addButtonCFG(0);
      addButton(0);
      addButton(9);
      addButton(10);
      addRelay(12);
      addRelay(5);
      addRelay(4);
      break;
    case BOARD_SONOFF_4CH:
      addLedCFG(13);
      addButtonCFG(0);
      addButton(0);
      addButton(9);
      addButton(10);
      addButton(14);
      addRelay(12);
      addRelay(5);
      addRelay(4);
      addRelay(15);
      break;
    case BOARD_YUNSHAN:
      addLedCFG(2, LOW);
      addButtonCFG(0);
      addButton(3);
      addRelay(4);
      break;

    case BOARD_YUNTONG_SMART:
      addLedCFG(15);
      addButtonCFG(12);
      addButton(12);
      addRelay(4);
      break;

    case BOARD_GOSUND_SP111:
      addLedCFG(2, LOW);
      addButtonCFG(13);
      addButton(13);
      addRelay(15);
      addLed(0);
#ifdef SUPLA_HLW8012
      addHLW8012(5, 4, 12);
      // Supla::GUI::counterHLW8012->setCurrentMultiplier(18388);
      // Supla::GUI::counterHLW8012->setVoltageMultiplier(247704);
      // Supla::GUI::counterHLW8012->setPowerMultiplier(2586583);
      // Supla::GUI::counterHLW8012->setMode(LOW);
#endif
      break;
    case BOARD_DIMMER_LUKASZH:
      addLedCFG(15);
      addButtonCFG(0);
      addDimmer(14);
      addDimmer(12);
      addDimmer(13);
      addButton(5);
      addButton(4);
      addButton(16);
      // ConfigESP->setGpio(GPIO_ANALOG_A0_ESP8266, FUNCTION_NTC_10K);
      break;
    case BOARD_H801:
      addLedCFG(1);
      addButtonCFG(0);
      addRGBW(15, 13, 12, 4);
      break;
    case BOARD_SHELLY_PLUG_S:
      addLedCFG(2, LOW);
      addButtonCFG(13);
      addButton(13);
      addRelay(15);
      addLed(0);
#ifdef SUPLA_HLW8012
      addHLW8012(5, 14, 12);
      // Supla::GUI::counterHLW8012->setCurrentMultiplier(18388);
      // Supla::GUI::counterHLW8012->setVoltageMultiplier(247704);
      // Supla::GUI::counterHLW8012->setPowerMultiplier(2586583);
#endif
      break;
    case BOARD_MINITIGER_1CH:
      addLedCFG(1);
      addButtonCFG(5);
      addButton(5, Supla::Event::ON_PRESS, Supla::Action::TOGGLE);
      addRelay(12);
      break;
    case BOARD_MINITIGER_2CH:
      addLedCFG(1);
      addButtonCFG(3);
      addButton(3, Supla::Event::ON_PRESS, Supla::Action::TOGGLE);
      addButton(4, Supla::Event::ON_PRESS, Supla::Action::TOGGLE);
      addRelay(13);
      addRelay(14);
      break;
    case BOARD_MINITIGER_3CH:
      addLedCFG(1);
      addButtonCFG(3);
      addButton(3, Supla::Event::ON_PRESS, Supla::Action::TOGGLE);
      addButton(5, Supla::Event::ON_PRESS, Supla::Action::TOGGLE);
      addButton(4, Supla::Event::ON_PRESS, Supla::Action::TOGGLE);
      addRelay(13);
      addRelay(12);
      addRelay(14);
      break;
  }
}
#endif