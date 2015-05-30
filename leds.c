////////////////////////////////////////////////////////////////////////////////
// leds.c
// Posilaci systemy
// LEDs settings - header file
// (c) Jan Horacek
// jan.horacek@seznam.cz
// 24.12.2014
// v1.1.0
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

#include <avr/io.h>
#include <util/delay.h>

#include "leds.h"
#include "main.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// leds.c variables:

// leds order:
//  [0] = MOs_red
//  [1] = Os_red
//  [2] = Mn_red
//  [3] = Pn_red

//  [4] = MOs_green
//  [5] = Os_green
//  [6] = Mn_green
//  [7] = Pn_green

//  [8] = ZAK
//  [9] = Odklad

// here are defined all LEDs
byte leds_P[LEDS_CNT]                 =  {PB1   , PB3   , PC0   , PC2   , PB2   , PB4   , PC1   , PC3   , PD3   , PC4   };
volatile uint8_t* leds_PORT[LEDS_CNT] =  {&PORTB, &PORTB, &PORTC, &PORTC, &PORTB, &PORTB, &PORTC, &PORTC, &PORTD, &PORTC};
volatile uint8_t* leds_DDR[LEDS_CNT]  =  {&DDRB , &DDRB , &DDRC , &DDRC , &DDRB , &DDRB , &DDRC , &DDRC , &DDRD , &DDRC};
volatile byte locked = 0;

////////////////////////////////////////////////////////////////////////////////

void leds_init(void)
{
  // setup pins as outputs
  int i;
  for (i = 0; i < LEDS_CNT; i++)
   *leds_DDR[i] |= BV(leds_P[i]);
}

//nastavi stav jedne LEDky
void leds_set(byte index, byte status)
{
  if (status)
    *leds_PORT[index] |= BV(leds_P[index]);
  else
    *leds_PORT[index] &= ~BV(leds_P[index]);
}

//hlavni logicke nastavovani dat do LEDek podle kroku
void leds_update(void)
{
  int i;

  if (locked == 1) return;

  if ((host_remote.odklad) || (host_local.odklad))
    leds_set(9, 1);
  else
    leds_set(9, 0);

  if (host_remote.ZAK)
  {
    for (i = 4; i < 9; i++)
      leds_set(i, 1);
  } else {
    if (host_local.ZAK > 0)
    {
      leds_set(8, 1);
      for (i = 0; i < 4; i++)
        leds_set(i, 1);
    } else {
	    for (i = 0; i < 9; i++)
        leds_set(i, 0);
    }
  }

  if (host_remote.status == 1)
  {
    if (host_local.status == 0) leds_set(host_remote.spr+4, blik);
    if (host_local.status == 2) leds_set(host_remote.spr+4, 1);
  }
  if (host_local.status == 1)
  {
    if (host_remote.status == 0) leds_set(host_local.spr, blik);
    if (host_remote.status == 2) leds_set(host_local.spr, 1);
  }

  if ((host_remote.status == 3) || (host_local.status == 3))
    leds_set(8, 1);
}

////////////////////////////////////////////////////////////////////////////////

//po startu otestuje LEDky
void leds_test(void)
 {
  int i;

  locked = 1;

  leds_set(8, 1);
  leds_set(9, 1);

  for (i = 0; i < 4; i++) leds_set(i, 1);
  _delay_ms(250);
  for (i = 0; i < 4; i++) leds_set(i, 0);
  for (i = 4; i < 8; i++) leds_set(i, 1);
  _delay_ms(250);
  for (i = 4; i < 8; i++) leds_set(i, 0);

  leds_set(8, 0);
  leds_set(9, 0);

  locked = 0;
 }//procedure

////////////////////////////////////////////////////////////////////////////////

