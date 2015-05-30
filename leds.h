////////////////////////////////////////////////////////////////////////////////
// leds.h
// Posilaci systemy
// LEDs settings - header file
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

#ifndef _LEDS_H_
#define _LEDS_H_

////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include "common_defs.h"

////////////////////////////////////////////////////////////////////////////////

#define LEDS_CNT  10

#define __LEDS_SET_TRAIN_INCOMING (index, status) leds_set(index, status)
#define __LEDS_SET_TRAIN_SENDING (index, status) leds_set(index+4, status)
#define __LEDS_SET_TRAIN_OFF for (int i = 0; i < 8) leds_set(i, 0)

////////////////////////////////////////////////////////////////////////////////

void leds_set(byte index, byte status);
void leds_update(void);
void leds_test(void);
void leds_init(void);

////////////////////////////////////////////////////////////////////////////////


#endif
