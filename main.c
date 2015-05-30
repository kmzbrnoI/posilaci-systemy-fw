////////////////////////////////////////////////////////////////////////////////
// main.c
// Posilaci systemy
// main executable file
// (c) Jan Horacek
// jan.horacek@seznam.cz
// 24.12.2014
// v4.1
////////////////////////////////////////////////////////////////////////////////

// This firmware is compatible with hardware version 3.0.

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

#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "main.h"
#include "doc.h"
#include "common_defs.h"
#include "eeprom.h"
#include "serial.h"
#include "buttons.h"
#include "leds.h"
#include "lib/uart.h"

////////////////////////////////////////////////////////////////////////////////

byte timers[2] = {0,0};				// main timers (100 and 500 ms)
byte ZAK_counter = 0;

////////////////////////////////////////////////////////////////////////////////

void init(void);

void beep(byte duration);
void main_timer_beep_update(void);

void timer_500ms(void);
void timer_100ms(void);

void main_btn_train(byte btn_index);
void main_btn_zrus(byte btn_index);
void main_btn_odklad(byte btn_index);
void main_btn_ZAK(byte btn_index);

////////////////////////////////////////////////////////////////////////////////

host host_remote = { .spr = 0, .status = 0, .ZAK = 0, .odklad = 0};
host host_local;
long odklad_time = 0;

////////////////////////////////////////////////////////////////////////////////

//main function
int main(void)
 {
  init();

  while (true)
  {
    btn_update();
    serial_update_send();
    _delay_us(10);
  }
}

////////////////////////////////////////////////////////////////////////////////

//system initialization
void init()
{
  // setup timers :
  TCCR1A = 0;
  TCCR1B = 8;
  TCCR1B |= 2;
  OCR1A = (9216); // 100Hz = 10ms
  TIMSK |= BV(OCIE1A);

  TIMSK |= BV(TOIE2);

  leds_init();
  btn_init();

  DDR_Repr |= BV(OUT_Repr);

  // serial comunication initialization
  uart_init();
  uart_interrupt_rx(true);
  uart_interrupt_tx(true);

  eeprom_init();

  // LEDs testing
  leds_test();

  sei();

  beep(50);
}

////////////////////////////////////////////////////////////////////////////////

// Timer1 every 10ms = 100hz
ISR(TIMER1_COMPA_vect)
{
  if (timers[0]++ == 50)
  {
	  timers[0] = 0;
	  timer_500ms();
  }
  if (timers[1]++ == 10)
  {
	  timers[1] = 0;
	  timer_100ms();
  }

  serial_update_timeout();
  main_timer_beep_update();
}


////////////////////////////////////////////////////////////////////////////////

ISR(TIMER2_OVF_vect)
{
  if (__IS_REPR_PORT_ON)
    __REPR_PORT_OFF;
  else
    __REPR_PORT_ON;
}

////////////////////////////////////////////////////////////////////////////////

void beep(byte duration)
{
  __REPR_ON(4000);
  _delay_ms(duration*10);
  __REPR_OFF;
}

////////////////////////////////////////////////////////////////////////////////

void timer_500ms()
{
  blik = !blik;
  leds_update();
  beep_zadost_update();

  if (host_local.odklad)
  {
    odklad_time++;
    if (odklad_time > (ODKLAD_TIMEOUT*2)) host_local.odklad = false;
  }
}

void timer_100ms()
{
  serial_send_stquery();
}

////////////////////////////////////////////////////////////////////////////////

void main_timer_beep_update(void)
{
  if (main_timer_beep.ms <= 0) return;

  main_timer_beep.ms--;
  if (main_timer_beep.ms <= 0)
  {
    main_timer_beep.ms = 0;
    main_timer_beep.event();
  }
}

////////////////////////////////////////////////////////////////////////////////

void main_timer_beep_mute(void)
{
  __REPR_OFF;
}

// update zadost beeping
void beep_zadost_update(void)
{
  if ((host_remote.status != 1) || (host_local.status != 0) || (host_local.odklad)) return;

  if (blik)
    __REPR_ON(4000)
  else
    __REPR_OFF
}

void main_timer_beep_ZAK(void)
{
  if (__IS_REPR_ON)
  {
    __REPR_OFF;
    if (ZAK_counter >= 2) return;
  } else {
    ZAK_counter++;
    __REPR_ON(4000);
  }

  main_timer_beep.ms    = 10;
  main_timer_beep.event = &main_timer_beep_ZAK;
}

////////////////////////////////////////////////////////////////////////////////

// main button press handling
void main_button_pressed(byte btn_index)
{
  if ((btn_index >= 0) && (btn_index <= 3))
    main_btn_train(btn_index);
  else if (btn_index == 4)
    main_btn_zrus(btn_index);
  else if (btn_index == 5)
    main_btn_ZAK(btn_index);
  else if (btn_index == 6)
    main_btn_odklad(btn_index);

  leds_update();
  serial_send_stquery();
}

void main_btn_train(byte btn_index)
{
  if (host_local.status != 0) return;

  if ((host_remote.status == 0) && (!host_remote.ZAK) && (!host_local.ZAK))
	{
    // new train from me
	  host_local.status = 1;
	  host_local.spr    = btn_index;
	  eeprom_log_button(btn_index);
	} else

  if ((host_remote.status == 1) && (host_remote.spr == btn_index))
  {
    // accept a train
    host_local.status = 2;
    host_local.odklad = false;
    __REPR_OFF;
  }
}

void main_btn_zrus(byte btn_index)
{
 if ((host_remote.status == 1) || ((host_local.status == 1) && (host_remote.status == 0)))
 {
   host_local.status = 3;
   host_local.odklad = false;
	 eeprom_log_button(btn_index);
   __REPR_OFF
 }
}

void main_btn_odklad(byte btn_index)
{
  if ((host_remote.status != 1) || (host_local.status != 0) || (host_local.odklad)) return;

  host_local.odklad = true;
  __REPR_OFF;
  eeprom_log_button(btn_index);
}

void main_btn_ZAK(byte btn_index)
{
  if ((host_remote.status) || (host_local.status) || (host_remote.ZAK)) return;

  if (host_local.ZAK == 0)
  {
	  host_local.ZAK = 1;
	  eeprom_log_button(btn_index);
  } else {
    host_local.ZAK = 0;
  }//else StavZAKOut == 0

 ZAK_counter = 0;
  __REPR_BEEP(10, main_timer_beep_ZAK)
}

////////////////////////////////////////////////////////////////////////////////
