////////////////////////////////////////////////////////////////////////////////
// main.h
// Posilaci systemy
// main header file
// (c) Jan Horacek
// jan.horacek@seznam.cz
// 24.12.2014
// v4.1.0.0
// License: Apache License v2.0
////////////////////////////////////////////////////////////////////////////////

/*
   Copyright 2014 Jan Horacek

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef _MAIN_H_
#define _MAIN_H_

#include "common_defs.h"

////////////////////////////////////////////////////////////////////////////////

#define VERSION   0x04010000;			// firmware version (format: 0xMAJOR 0xMINOR 0xRELEASE 0xBUILD)

#define OUT_Repr			PB5
#define DDR_Repr			DDRB
#define PIN_Repr			PINB
#define PORT_Repr			PORTB
#define __REPR_PORT_ON     PORT_Repr |= BV(OUT_Repr)
#define __REPR_PORT_OFF    PORT_Repr &= ~BV(OUT_Repr)

#define __REPR_ON(freq)    { OCR2 = ((F_CPU/(16U*freq))-1); TCCR2 |= BV(CS21); }
#define __REPR_OFF         TCCR2 = 0;

#define __IS_REPR_ON  (TCCR2 > 0)

#define __REPR_BEEP(duration, callback) { \
    __REPR_ON(4000) \
    main_timer_beep.ms = duration; \
    main_timer_beep.event = &callback; }

#define __IS_REPR_PORT_ON (PIN_Repr & BV(OUT_Repr))

#define ODKLAD_TIMEOUT 180      // po 3 minutach (180 sekundach) zacne PS v pripade odkladu oept pipat

extern byte ZAK_counter;
extern long odklad_time;

////////////////////////////////////////////////////////////////////////////////

typedef struct {
  byte spr;
  byte status;
  byte ZAK;
  byte odklad;
} host;

extern host host_remote;
extern host host_local;

struct {
  byte ms;
  void (*event)(void);
} main_timer_beep;

byte blik;

////////////////////////////////////////////////////////////////////////////////

void main_button_pressed(byte btn_index);

void main_timer_beep_mute(void);
void main_timer_beep_zadost(void);
void main_timer_beep_ZAK(void);

void beep_zadost_update(void);

////////////////////////////////////////////////////////////////////////////////

#endif