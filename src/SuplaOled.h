#ifndef SuplaOled_H
#define SuplaOled_H

#ifdef SUPLA_OLED

#include "GUI-Generic_Config.h"
#include <pgmspace.h>
#include <supla/triggerable.h>
#include <supla/element.h>
#include <Wire.h>         // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"  //OLED 0,96"
#include "SH1106Wire.h"   //OLED 1.3"
#include "OLEDDisplayUi.h"

enum customActions
{
  TURN_ON_OLED
};

class SuplaOled : public Supla::Triggerable, public Supla::Element {
 public:
  SuplaOled();
  void addButtonOled(int pin);

 private:
  void iterateAlways();
  void runAction(int event, int action);

  OLEDDisplay *display;
  OLEDDisplayUi *ui;

  FrameCallback *frames;
  int frameCount = 0;
  OverlayCallback overlays[1];
  int overlaysCount = 1;

  int count = 0;

  unsigned long timeLastChangeOled = millis();
  bool oledON = true;
};

extern SuplaOled *oled;

// https://www.online-utility.org/image/convert/to/XBM
#define temp_width  32
#define temp_height 32
const uint8_t temp_bits[] PROGMEM = {0x00, 0x3C, 0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x00, 0xC3, 0x00, 0x00, 0x00, 0x81, 0xF8, 0x03, 0x00, 0x99, 0x00,
                                     0x00, 0x00, 0x99, 0x00, 0x00, 0x00, 0x99, 0x00, 0x00, 0x00, 0x99, 0xF8, 0x03, 0x00, 0x99, 0x00, 0x00, 0x00, 0x99,
                                     0x00, 0x00, 0x00, 0x99, 0x00, 0x00, 0x00, 0x99, 0xF8, 0x03, 0x00, 0x99, 0x00, 0x00, 0x00, 0x99, 0x00, 0x00, 0x00,
                                     0x99, 0x00, 0x00, 0x00, 0x99, 0x00, 0x00, 0x00, 0x99, 0x00, 0x00, 0x00, 0x99, 0x00, 0x00, 0x00, 0x99, 0x00, 0x00,
                                     0x00, 0x99, 0x00, 0x00, 0x00, 0x99, 0x00, 0x00, 0x80, 0x99, 0x01, 0x00, 0xC0, 0x18, 0x03, 0x00, 0x60, 0x3C, 0x06,
                                     0x00, 0x20, 0x7E, 0x04, 0x00, 0x20, 0x7E, 0x04, 0x00, 0x20, 0x7E, 0x04, 0x00, 0x60, 0x3C, 0x06, 0x00, 0xC0, 0x18,
                                     0x03, 0x00, 0x80, 0x81, 0x01, 0x00, 0x00, 0xC3, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00};

#define humidity_width  32
#define humidity_height 32
const uint8_t humidity_bits[] PROGMEM = {
    0x00, 0x80, 0x01, 0x00, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x30, 0x0C, 0x00, 0x00, 0x38, 0x1C, 0x00, 0x00, 0x18,
    0x18, 0x00, 0x00, 0x0C, 0x30, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x06, 0x40, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x80, 0x01, 0x80, 0x00,
    0x80, 0x01, 0x80, 0x01, 0xC0, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x03, 0x60, 0x30, 0x08, 0x06, 0x30, 0x48, 0x0C, 0x0C, 0x30, 0x48,
    0x06, 0x0C, 0x30, 0x30, 0x03, 0x0C, 0x30, 0x80, 0x01, 0x0C, 0x30, 0xC0, 0x00, 0x0C, 0x30, 0x60, 0x06, 0x0C, 0x30, 0x30, 0x09, 0x0C,
    0x30, 0x10, 0x09, 0x0C, 0x30, 0x00, 0x06, 0x0C, 0x20, 0x00, 0x00, 0x04, 0x60, 0x00, 0x00, 0x06, 0xC0, 0x00, 0x00, 0x03, 0x80, 0x01,
    0x80, 0x01, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x0E, 0x70, 0x00, 0x00, 0xFC, 0x3F, 0x00, 0x00, 0xF0, 0x0F, 0x00};

#define pressure_width  32
#define pressure_height 32
const uint8_t pressure_bits[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0,
    0x0F, 0x00, 0x00, 0xFC, 0x3F, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x80, 0x81, 0x81, 0x01, 0xC0, 0x80, 0x01, 0x03,
    0x60, 0x80, 0x01, 0x06, 0x30, 0x00, 0xC0, 0x0C, 0x30, 0x03, 0xE0, 0x18, 0x18, 0x07, 0x40, 0x10, 0x0C, 0x82, 0x01, 0x30, 0x0C, 0x80,
    0x01, 0x30, 0x0C, 0x80, 0x01, 0x30, 0x0C, 0x80, 0x01, 0x30, 0x0C, 0x80, 0x01, 0x30, 0x0C, 0x81, 0x81, 0x30, 0x8C, 0x83, 0xC1, 0x31,
    0x8C, 0x81, 0x81, 0x31, 0x08, 0xC0, 0x03, 0x10, 0x08, 0xE0, 0x07, 0x10, 0x00, 0xE0, 0x07, 0x00, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x80,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#endif
#endif  // SuplaOled_H