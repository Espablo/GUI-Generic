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

#include <Arduino.h>
#include <SuplaDevice.h>

#include "timer.h"

#if defined(ARDUINO_ARCH_ESP32)
#include <esp_timer.h>
#endif

namespace {
#if defined(ARDUINO_ARCH_ESP8266)
ETSTimer supla_esp_timer;
ETSTimer supla_esp_fastTimer;

void esp_timer_cb(void *timer_arg) {
  SuplaDevice.onTimer();
}

void esp_fastTimer_cb(void *timer_arg) {
  SuplaDevice.onFastTimer();
}
#elif defined(ARDUINO_ARCH_ESP32)
hw_timer_t *supla_esp_timer = NULL;
hw_timer_t *supla_esp_fastTimer = NULL;

void IRAM_ATTR esp_timer_cb() {
  SuplaDevice.onTimer();
}

void IRAM_ATTR esp_fastTimer_cb() {
  SuplaDevice.onFastTimer();
}
#else
ISR(TIMER1_COMPA_vect) {
  SuplaDevice.onTimer();
}
ISR(TIMER2_COMPA_vect) {
  SuplaDevice.onFastTimer();
}
#endif
};  // namespace

namespace Supla {
void initTimers() {
#if defined(ARDUINO_ARCH_ESP8266)

  os_timer_disarm(&supla_esp_timer);
  os_timer_setfn(&supla_esp_timer, (os_timer_func_t *)esp_timer_cb, NULL);
  os_timer_arm(&supla_esp_timer, 10, 1);

  os_timer_disarm(&supla_esp_fastTimer);
  os_timer_setfn(&supla_esp_fastTimer, (os_timer_func_t *)esp_fastTimer_cb, NULL);
  os_timer_arm(&supla_esp_fastTimer, 1, 1);

#elif defined(ARDUINO_ARCH_ESP32)
  supla_esp_timer = timerBegin(0, 80, true);                   // timer 0, div 80
  timerAttachInterrupt(supla_esp_timer, &esp_timer_cb, true);  // attach callback
  timerAlarmWrite(supla_esp_timer, 10 * 1000, false);          // set time in us
  timerAlarmEnable(supla_esp_timer);                           // enable interrupt

  supla_esp_fastTimer = timerBegin(1, 80, true);
  timerAttachInterrupt(supla_esp_fastTimer,
                       &esp_fastTimer_cb,
                       true);                       // attach callback
  timerAlarmWrite(supla_esp_fastTimer, 1 * 1000, false);  // set time in us
  timerAlarmEnable(supla_esp_fastTimer);                  // enable interrupt
#else
  // Timer 1 for interrupt frequency 100 Hz (10 ms)
  TCCR1A = 0;  // set entire TCCR1A register to 0
  TCCR1B = 0;  // same for TCCR1B
  TCNT1 = 0;   // initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 155;  // (16*10^6) / (100*1024) - 1 (must be <65536) == 155.25
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei();  // enable interrupts

  // TIMER 2 for interrupt frequency 2000 Hz (0.5 ms)
  cli();       // stop interrupts
  TCCR2A = 0;  // set entire TCCR2A register to 0
  TCCR2B = 0;  // same for TCCR2B
  TCNT2 = 0;   // initialize counter value to 0
  // set compare match register for 2000 Hz increments
  OCR2A = 249;  // = 16000000 / (32 * 2000) - 1 (must be <256)
  // turn on CTC mode
  TCCR2B |= (1 << WGM21);
  // Set CS22, CS21 and CS20 bits for 32 prescaler
  TCCR2B |= (0 << CS22) | (1 << CS21) | (1 << CS20);
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);
  sei();  // allow interrupts
#endif
}

};  // namespace Supla
