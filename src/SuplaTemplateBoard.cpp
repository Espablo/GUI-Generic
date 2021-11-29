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

namespace Supla {
namespace TanplateBoard {
void addTemplateBoard() {
#ifdef SUPLA_TEMPLATE_BOARD_JSON
#ifdef TEMPLATE_JSON
  Supla::TanplateBoard::chooseTemplateBoard(TEMPLATE_JSON);
#endif
#elif SUPLA_TEMPLATE_BOARD_OLD
  chooseTemplateBoard(ConfigESP->getDefaultTamplateBoard());
#endif
}
}  // namespace TanplateBoard
}  // namespace Supla

#ifdef SUPLA_TEMPLATE_BOARD_JSON
namespace Supla {
namespace TanplateBoard {

void chooseTemplateBoard(String board) {
  ConfigESP->clearEEPROM();
  ConfigManager->deleteGPIODeviceValues();
  templateBoardWarning = "";

  ConfigManager->set(KEY_MAX_BUTTON, "0");
  ConfigManager->set(KEY_MAX_RELAY, "0");
  ConfigManager->set(KEY_VIRTUAL_RELAY, "");
  ConfigManager->set(KEY_MAX_LIMIT_SWITCH, "0");
  ConfigManager->set(KEY_MAX_RGBW, "0");
  ConfigManager->set(KEY_CFG_MODE, CONFIG_MODE_10_ON_PRESSES);
  ConfigManager->set(KEY_ANALOG_BUTTON, "");
  ConfigManager->set(KEY_ANALOG_INPUT_EXPECTED, "");

  const size_t capacity = JSON_ARRAY_SIZE(14) + JSON_OBJECT_SIZE(4) + 200;
  DynamicJsonBuffer jsonBuffer(capacity);

  JsonObject& root = jsonBuffer.parseObject(board);
  JsonArray& GPIO = root["GPIO"];

  JsonArray& analogButtons = root["AnalogButtons"];
  //"AnalogButtons":[250, 500, 750]
  for (size_t i = 0; i < analogButtons.size(); i++) {
    addButtonAnalog(i, analogButtons[i]);
  }

  String name = root["NAME"];
  ConfigManager->set(KEY_HOST_NAME, name.c_str());

  if (GPIO.size() == 0) {
    templateBoardWarning += "Błąd wczytania<br>";
    return;
  }

#ifdef ARDUINO_ARCH_ESP8266
  bool oldVersion = false;

  if (GPIO.size() == 13)
    oldVersion = true;

  templateBoardWarning += F("Wersja: ");
  if (oldVersion)
    templateBoardWarning += 1;
  else
    templateBoardWarning += 2;
  templateBoardWarning += F("<br>");
#elif ARDUINO_ARCH_ESP32

#endif

  for (size_t i = 0; i < GPIO.size(); i++) {
    int gpioJSON = (int)GPIO[i];
    int gpio = getGPIO(i);

#ifdef ARDUINO_ARCH_ESP8266
    if (oldVersion)
      gpioJSON = convert(gpioJSON);
#endif

    switch (gpioJSON) {
      case FunctionNew::NewNone:
        break;
      case FunctionNew::NewUsers:
        break;

      case FunctionNew::NewRelay1:
        Supla::TanplateBoard::addRelay(0, gpio);
        break;
      case FunctionNew::NewRelay2:
        Supla::TanplateBoard::addRelay(1, gpio);
        break;
      case FunctionNew::NewRelay3:
        Supla::TanplateBoard::addRelay(2, gpio);
        break;
      case FunctionNew::NewRelay4:
        Supla::TanplateBoard::addRelay(3, gpio);
        break;

      case FunctionNew::NewRelay1i:
        Supla::TanplateBoard::addRelay(0, gpio, LOW);
        break;
      case FunctionNew::NewRelay2i:
        Supla::TanplateBoard::addRelay(1, gpio, LOW);
        break;
      case FunctionNew::NewRelay3i:
        Supla::TanplateBoard::addRelay(2, gpio, LOW);
        break;
      case FunctionNew::NewRelay4i:
        Supla::TanplateBoard::addRelay(3, gpio, LOW);
        break;

      case FunctionNew::NewSwitch1:
        if (ConfigESP->getGpio(0, FUNCTION_BUTTON) != OFF_GPIO) {
          ConfigESP->clearGpio(ConfigESP->getGpio(0, FUNCTION_BUTTON), FUNCTION_BUTTON);
          ConfigManager->set(KEY_MAX_BUTTON, ConfigManager->get(KEY_MAX_BUTTON)->getValueInt() - 1);
        }
        else {
          Supla::TanplateBoard::addButtonCFG(gpio);
        }

        Supla::TanplateBoard::addButton(0, gpio, Supla::Event::ON_CHANGE, Supla::Action::TOGGLE, true, true);

        break;
      case FunctionNew::NewSwitch2:
        Supla::TanplateBoard::addButton(1, gpio, Supla::Event::ON_CHANGE, Supla::Action::TOGGLE, true, true);
        break;
      case FunctionNew::NewSwitch3:
        Supla::TanplateBoard::addButton(2, gpio, Supla::Event::ON_CHANGE, Supla::Action::TOGGLE, true, true);
        break;
      case FunctionNew::NewSwitch4:
        Supla::TanplateBoard::addButton(3, gpio, Supla::Event::ON_CHANGE, Supla::Action::TOGGLE, true, true);
        break;
      case FunctionNew::NewSwitch5:
        Supla::TanplateBoard::addButton(4, gpio, Supla::Event::ON_CHANGE, Supla::Action::TOGGLE, true, true);
        break;
      case FunctionNew::NewSwitch6:
        Supla::TanplateBoard::addButton(5, gpio, Supla::Event::ON_CHANGE, Supla::Action::TOGGLE, true, true);
        break;
      case FunctionNew::NewSwitch7:
        Supla::TanplateBoard::addButton(6, gpio, Supla::Event::ON_CHANGE, Supla::Action::TOGGLE, true, true);
        break;

      case FunctionNew::NewSwitch1n:
        if (ConfigESP->getGpio(0, FUNCTION_BUTTON) != OFF_GPIO) {
          ConfigESP->clearGpio(ConfigESP->getGpio(0, FUNCTION_BUTTON), FUNCTION_BUTTON);
          ConfigManager->set(KEY_MAX_BUTTON, ConfigManager->get(KEY_MAX_BUTTON)->getValueInt() - 1);
        }
        else {
          Supla::TanplateBoard::addButtonCFG(gpio);
        }

        Supla::TanplateBoard::addButton(0, gpio, Supla::Event::ON_CHANGE, Supla::Action::TOGGLE, false, false);

        break;
      case FunctionNew::NewSwitch2n:
        Supla::TanplateBoard::addButton(1, gpio, Supla::Event::ON_CHANGE, Supla::Action::TOGGLE, false, false);
        break;
      case FunctionNew::NewSwitch3n:
        Supla::TanplateBoard::addButton(2, gpio, Supla::Event::ON_CHANGE, Supla::Action::TOGGLE, false, false);
        break;
      case FunctionNew::NewSwitch4n:
        Supla::TanplateBoard::addButton(3, gpio, Supla::Event::ON_CHANGE, Supla::Action::TOGGLE, false, false);
        break;
      case FunctionNew::NewSwitch5n:
        Supla::TanplateBoard::addButton(4, gpio, Supla::Event::ON_CHANGE, Supla::Action::TOGGLE, false, false);
        break;
      case FunctionNew::NewSwitch6n:
        Supla::TanplateBoard::addButton(5, gpio, Supla::Event::ON_CHANGE, Supla::Action::TOGGLE, false, false);
        break;
      case FunctionNew::NewSwitch7n:
        Supla::TanplateBoard::addButton(6, gpio, Supla::Event::ON_CHANGE, Supla::Action::TOGGLE, false, false);
        break;

      case FunctionNew::NewButton1:
        Supla::TanplateBoard::addButton(0, gpio);
        Supla::TanplateBoard::addButtonCFG(gpio);
        break;
      case FunctionNew::NewButton2:
        Supla::TanplateBoard::addButton(1, gpio);
        break;
      case FunctionNew::NewButton3:
        Supla::TanplateBoard::addButton(2, gpio);
        break;
      case FunctionNew::NewButton4:
        Supla::TanplateBoard::addButton(3, gpio);
        break;

      case FunctionNew::NewButton1n:
        Supla::TanplateBoard::addButton(0, gpio, Supla::Event::ON_PRESS, Supla::Action::TOGGLE, false, false);
        Supla::TanplateBoard::addButtonCFG(gpio);
        break;
      case FunctionNew::NewButton2n:
        Supla::TanplateBoard::addButton(1, gpio, Supla::Event::ON_PRESS, Supla::Action::TOGGLE, false, false);
        break;
      case FunctionNew::NewButton3n:
        Supla::TanplateBoard::addButton(2, gpio, Supla::Event::ON_PRESS, Supla::Action::TOGGLE, false, false);
        break;
      case FunctionNew::NewButton4n:
        Supla::TanplateBoard::addButton(3, gpio, Supla::Event::ON_PRESS, Supla::Action::TOGGLE, false, false);
        break;

      case FunctionNew::NewLed1:
        Supla::TanplateBoard::addLed(0, gpio);
        break;
      case FunctionNew::NewLed2:
        Supla::TanplateBoard::addLed(1, gpio);
        break;
      case FunctionNew::NewLed3:
        Supla::TanplateBoard::addLed(2, gpio);
        break;
      case FunctionNew::NewLed4:
        Supla::TanplateBoard::addLed(3, gpio);
        break;
      case FunctionNew::NewLedLink:
        Supla::TanplateBoard::addLedCFG(gpio);
        break;

      case FunctionNew::NewLed1i:
        Supla::TanplateBoard::addLed(0, gpio, LOW);
        break;
      case FunctionNew::NewLed2i:
        Supla::TanplateBoard::addLed(1, gpio, LOW);
        break;
      case FunctionNew::NewLed3i:
        Supla::TanplateBoard::addLed(2, gpio, LOW);
        break;
      case FunctionNew::NewLed4i:
        Supla::TanplateBoard::addLed(3, gpio, LOW);
        break;
      case FunctionNew::NewLedLinki:
        Supla::TanplateBoard::addLedCFG(gpio, LOW);
        break;

      case FunctionNew::NewPWM1:
        ConfigESP->setGpio(gpio, FUNCTION_RGBW_RED);
        ConfigManager->set(KEY_MAX_RGBW, ConfigManager->get(KEY_MAX_RGBW)->getValueInt() + 1);
        break;
      case FunctionNew::NewPWM2:
        ConfigESP->setGpio(gpio, FUNCTION_RGBW_GREEN);
        break;
      case FunctionNew::NewPWM3:
        ConfigESP->setGpio(gpio, FUNCTION_RGBW_BLUE);
        break;
      case FunctionNew::NewPWM4:
        ConfigESP->setGpio(gpio, FUNCTION_RGBW_BRIGHTNESS);
        break;

      case FunctionNew::NewBL0937CF:
        ConfigESP->setGpio(gpio, FUNCTION_CF);
        break;
      case FunctionNew::NewHLWBLCF1:
        ConfigESP->setGpio(gpio, FUNCTION_CF1);
        break;
      case FunctionNew::NewHLWBLSELi:
        ConfigESP->setGpio(gpio, FUNCTION_SEL);
        break;

      case FunctionNew::NewTemperatureAnalog:
        ConfigESP->setGpio(gpio, FUNCTION_NTC_10K);
        break;

      case FunctionNew::NewSI7021:
        ConfigESP->setGpio(gpio, FUNCTION_SI7021_SONOFF);
        break;

      case FunctionNew::NewCSE7766Rx:
        ConfigESP->setGpio(gpio, FUNCTION_CSE7766_RX);
        break;

      default:
        templateBoardWarning += "Brak funkcji: ";
        templateBoardWarning += gpioJSON;
        templateBoardWarning += "<br>";
    }
  }
}

int convert(int gpioJSON) {
  switch (gpioJSON) {
    case FunctionOld::None:
      return FunctionNew::NewNone;
    case FunctionOld::Users:
      return FunctionNew::NewUsers;

    case FunctionOld::Relay1:
      return FunctionNew::NewRelay1;
    case FunctionOld::Relay2:
      return FunctionNew::NewRelay2;
    case FunctionOld::Relay3:
      return FunctionNew::NewRelay3;
    case FunctionOld::Relay4:
      return FunctionNew::NewRelay4;

    case FunctionOld::Relay1i:
      return FunctionNew::NewRelay1i;
    case FunctionOld::Relay2i:
      return FunctionNew::NewRelay2i;
    case FunctionOld::Relay3i:
      return FunctionNew::NewRelay3i;
    case FunctionOld::Relay4i:
      return FunctionNew::NewRelay4i;

    case FunctionOld::Switch1:
      return FunctionNew::NewSwitch1;
    case FunctionOld::Switch2:
      return FunctionNew::NewSwitch2;
    case FunctionOld::Switch3:
      return FunctionNew::NewSwitch3;
    case FunctionOld::Switch4:
      return FunctionNew::NewSwitch4;
    case FunctionOld::Switch5:
      return FunctionNew::NewSwitch5;
    case FunctionOld::Switch6:
      return FunctionNew::NewSwitch6;
    case FunctionOld::Switch7:
      return FunctionNew::NewSwitch7;

    case FunctionOld::Switch1n:
      return FunctionNew::NewSwitch1n;
    case FunctionOld::Switch2n:
      return FunctionNew::NewSwitch2n;
    case FunctionOld::Switch3n:
      return FunctionNew::NewSwitch3n;
    case FunctionOld::Switch4n:
      return FunctionNew::NewSwitch4n;
    case FunctionOld::Switch5n:
      return FunctionNew::NewSwitch5n;
    case FunctionOld::Switch6n:
      return FunctionNew::NewSwitch6n;
    case FunctionOld::Switch7n:
      return FunctionNew::NewSwitch7n;

    case FunctionOld::Button1:
      return FunctionNew::NewButton1;
    case FunctionOld::Button2:
      return FunctionNew::NewButton2;
    case FunctionOld::Button3:
      return FunctionNew::NewButton3;
    case FunctionOld::Button4:
      return FunctionNew::NewButton4;

    case FunctionOld::Button1n:
      return FunctionNew::NewButton1n;
    case FunctionOld::Button2n:
      return FunctionNew::NewButton2n;
    case FunctionOld::Button3n:
      return FunctionNew::NewButton3n;
    case FunctionOld::Button4n:
      return FunctionNew::NewButton4n;

    case FunctionOld::Led1:
      return FunctionNew::NewLed1;
    case FunctionOld::Led2:
      return FunctionNew::NewLed2;
    case FunctionOld::Led3:
      return FunctionNew::NewLed3;
    case FunctionOld::Led4:
      return FunctionNew::NewLed4;
    case FunctionOld::LedLink:
      return FunctionNew::NewLedLink;

    case FunctionOld::Led1i:
      return FunctionNew::NewLed1i;
    case FunctionOld::Led2i:
      return FunctionNew::NewLed2i;
    case FunctionOld::Led3i:
      return FunctionNew::NewLed3i;
    case FunctionOld::Led4i:
      return FunctionNew::NewLed4i;
    case FunctionOld::LedLinki:
      return FunctionNew::NewLedLinki;

    case FunctionOld::PWM1:
      return FunctionNew::NewPWM1;
    case FunctionOld::PWM2:
      return FunctionNew::NewPWM2;
    case FunctionOld::PWM3:
      return FunctionNew::NewPWM3;
    case FunctionOld::PWM4:
      return FunctionNew::NewPWM4;

    case FunctionOld::BL0937CF:
      return FunctionNew::NewBL0937CF;
    case FunctionOld::HLWBLCF1:
      return FunctionNew::NewHLWBLCF1;
    case FunctionOld::HLWBLSELi:
      return FunctionNew::NewHLWBLSELi;

    case FunctionOld::TemperatureAnalog:
      return FunctionNew::NewTemperatureAnalog;

    case FunctionOld::SI7021:
      return FunctionNew::NewSI7021;

    case FunctionOld::CSE7766Tx:
      return FunctionNew::NewCSE7766Tx;
    case FunctionOld::CSE7766Rx:
      return FunctionNew::NewCSE7766Rx;
  }
  return FunctionNew::NewNone;
}

uint8_t getGPIO(uint8_t gpio) {
#ifdef ARDUINO_ARCH_ESP8266
  if (gpio == 6 || gpio == 7)
    gpio = gpio + 3;
  else if (gpio >= 8)
    gpio = gpio + 4;
#elif ARDUINO_ARCH_ESP32
  if (gpio == 6 || gpio == 7)
    gpio = gpio + 3;
  else if (gpio >= 8 && gpio <= 23)
    gpio = gpio + 4;
  else if (gpio == 24)
    gpio = 6;
  else if (gpio == 25)
    gpio = 7;
  else if (gpio == 26)
    gpio = 8;
  else if (gpio == 27)
    gpio = 11;
  else if (gpio >= 28)
    gpio = gpio + 4;
#endif

  return gpio;
}

void addButton(uint8_t nr, uint8_t gpio, uint8_t event, uint8_t action, bool pullUp, bool invertLogic) {
  uint8_t maxButton = ConfigManager->get(KEY_MAX_BUTTON)->getValueInt();

  ConfigESP->setEvent(gpio, event);
  ConfigESP->setAction(gpio, action);
  ConfigESP->setPullUp(gpio, pullUp);
  ConfigESP->setInversed(gpio, invertLogic);

  if (ConfigESP->getGpio(FUNCTION_CFG_BUTTON) == OFF_GPIO) {
    Supla::TanplateBoard::addButtonCFG(gpio);
  }

  ConfigESP->setGpio(gpio, nr, FUNCTION_BUTTON);
  ConfigManager->set(KEY_MAX_BUTTON, maxButton + 1);
}

void addButtonAnalog(uint8_t nr, int expected) {
  uint8_t maxButton = ConfigManager->get(KEY_MAX_BUTTON)->getValueInt();

  if (expected == 0) {
    ConfigManager->set(KEY_MAX_BUTTON, maxButton + 1);
    return;
  }

  ConfigESP->setAction(A0, Supla::Action::TOGGLE);
  ConfigManager->setElement(KEY_ANALOG_BUTTON, nr, true);
  ConfigManager->setElement(KEY_ANALOG_INPUT_EXPECTED, nr, expected);
  ConfigManager->set(KEY_MAX_BUTTON, maxButton + 1);
}

void addRelay(uint8_t nr, uint8_t gpio, uint8_t level) {
  uint8_t maxRelay = ConfigManager->get(KEY_MAX_RELAY)->getValueInt();

  ConfigESP->setLevel(gpio, level);
  ConfigESP->setMemory(gpio, MEMORY_RELAY_RESTORE);
  ConfigESP->setGpio(gpio, nr, FUNCTION_RELAY);

  ConfigManager->setElement(KEY_NUMBER_BUTTON, nr, nr);
  ConfigManager->set(KEY_MAX_RELAY, maxRelay + 1);
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
  for (uint8_t nr = 0; nr <= OFF_GPIO; nr++) {
    ConfigESP->clearGpio(nr, FUNCTION_CFG_BUTTON);
  }

  ConfigESP->setGpio(gpio, FUNCTION_CFG_BUTTON);
}

String templateBoardWarning;

}  // namespace TanplateBoard
}  // namespace Supla
#elif SUPLA_TEMPLATE_BOARD_OLD
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