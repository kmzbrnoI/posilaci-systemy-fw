////////////////////////////////////////////////////////////////////////////////
// buttons.c
// Posilaci systemy
// Buttons handling - executable file
// (c) Jan Horacek
// jan.horacek@seznam.cz
// 24.12.2014
// v1.0.1
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
#include "buttons.h"
#include "main.h"

////////////////////////////////////////////////////////////////////////////////

#define BTN_PRESSED_MIN  50         // button must be hold at least for BTN_PRESSED_MIN cycles to be considered as pressed
#define BTN_CNT           7

////////////////////////////////////////////////////////////////////////////////
// buttons.c variables:

// buttons order:
//  [0] = MOs
//  [1] = Os
//  [2] = Mn
//  [3] = Pn
//  [4] = Reset
//  [5] = ZAK
//  [6] = Odklad

// here are defined all buttons
byte btn_P[BTN_CNT]                 =  {PB0   , PD7   , PD6   , PD5   , PC5   , PD4   , PD2   };
volatile uint8_t* btn_PIN[BTN_CNT]  =  {&PINB , &PIND , &PIND , &PIND , &PINC , &PIND , &PIND };
volatile uint8_t* btn_PORT[BTN_CNT] =  {&PORTB, &PORTD, &PORTD, &PORTD, &PORTC, &PORTD, &PORTD};

struct
{
  signed char index;
  int time;      // button must be pressed at least BTN_PRESSED_MIN cycles to be considered as presses
} btn_pressed = {-1, 0};


////////////////////////////////////////////////////////////////////////////////

// init buttons = set pins as inputs and turn on inside pull-up resistors
void btn_init(void)
{
  int i;

  // setup pull-up resistors on inputs
  for (i = 0; i < BTN_CNT; i++)
    *btn_PORT[i] |= BV(btn_P[i]);
}

////////////////////////////////////////////////////////////////////////////////

// update buttons
void btn_update(void)
{
  if (btn_pressed.index > -1)
  {
    // index > -1 = button already pressed
    if (!(*btn_PIN[btn_pressed.index] & BV(btn_P[btn_pressed.index])))
    {
     if (btn_pressed.time < BTN_PRESSED_MIN)
       btn_pressed.time++;
     else {
       if (btn_pressed.time == BTN_PRESSED_MIN)
       {
         btn_pressed.time++;
         main_button_pressed(btn_pressed.index);        // call press event
       }
     }//else
    } else
     btn_pressed.index = -1;
  } else {
    // index = -1 = button not pressed
    for (int i = 0; i < BTN_CNT; i++)
    {
      if (!(*btn_PIN[i] & BV(btn_P[i])))
      {
        btn_pressed.index = i;
        btn_pressed.time  = 0;
        return;
      }
    }//for
  }//else
}

////////////////////////////////////////////////////////////////////////////////

