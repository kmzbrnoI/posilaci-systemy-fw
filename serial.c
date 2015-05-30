////////////////////////////////////////////////////////////////////////////////
// serial.c
// Posilaci systemy
// Serial bus (RS232, uart) executable file
// (c) Jan Horacek
// jan.horacek@seznam.cz
// 24.10.2014
// v1.0.0
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

#include "serial.h"
#include "eeprom.h"
#include "main.h"

#include "leds.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

////////////////////////////////////////////////////////////////////////////////
// definitions for serial.c:

#define SERIAL_BUF_LEN    16
#define INIT_BYTE       0xAA

#define SERIAL_BUSY_WAIT { while (serial_snd_buf_pos > 0) _delay_ms(1); }

////////////////////////////////////////////////////////////////////////////////
// variables for serial.c:

byte serial_rcv_buf[SERIAL_BUF_LEN];						 // serial receive buffer
byte serial_snd_buf[SERIAL_BUF_LEN];						 // serial send buffer
byte serial_rcv_buf_pos = 0;			               // serial receive buffer pointer: 0 = no transmission,  1...n = positions in receive array
byte serial_rcv_len = 0;                         // length of serial received data
volatile byte serial_snd_buf_pos = 0;		         // serial send buffer pointer    : 0 = no transmission,  1...n = positions in receive array
volatile byte serial_timeout = 0;		             // serial timeout: decrementing from x to 0, when reached zero -> reset incoming transmission
volatile byte serial_snd_cnt = 0;
volatile byte send_operation = 0;

#define _OP_NULL          0
#define _OP_SEND_EEPROM   1
#define _OP_SEND_VERSION  2
#define _OP_SEND_ACK      3

#define SET_SEND_OPERATION(op) { if (send_operation == _OP_NULL) send_operation = op; }

////////////////////////////////////////////////////////////////////////////////
// function headers:

void serial_received(void);
void serial_send_data(void);

////////////////////////////////////////////////////////////////////////////////
// main sending function:

void serial_send_data()
{
  byte i, xor;

  // total amount of bytes ++, because of XOR byte
  // so, second byte is count with itself (with xor and WITHOUT init byte)
  serial_snd_cnt = serial_snd_buf[0];
  for (i = serial_snd_cnt; i > 0; i--) serial_snd_buf[i] = serial_snd_buf[i-1];
  serial_snd_cnt += 2;
  serial_snd_buf[0] = INIT_BYTE;
  serial_snd_buf[1] = serial_snd_cnt-1;

  // XOR calculation
  xor = 0xFF;
  for(i = 0; i < serial_snd_cnt-1; i++) xor ^= serial_snd_buf[i];
  serial_snd_buf[serial_snd_cnt-1] = xor;

  // send first byte:
  serial_snd_buf_pos = 1;
  UDR = serial_snd_buf[0];
}

// send all bytes after first byte:
ISR(USART_TXC_vect)
{
 if (serial_snd_buf_pos < serial_snd_cnt)
   UDR = serial_snd_buf[serial_snd_buf_pos++];
 else
   serial_snd_buf_pos = 0;
}

////////////////////////////////////////////////////////////////////////////////

// serial receive byte interrupt:
ISR(USART_RXC_vect)
{
  int i;
  serial_timeout = 20;    // timeout = 200ms

  serial_rcv_buf[serial_rcv_buf_pos++] = UDR;

  // check for init byte at start
  if (serial_rcv_buf_pos == 1)
  {
    if (serial_rcv_buf[0] != INIT_BYTE)
    {
      // init byte not correct -> dismiss
      serial_rcv_buf_pos = 0;
      serial_rcv_buf[1]  = 3;   // count seed
    }

  } else {
    // all other bytes -> receive
    if (serial_rcv_buf_pos > serial_rcv_buf[1])
    {
      serial_rcv_len     = serial_rcv_buf_pos;
      serial_rcv_buf_pos = 0;
      serial_timeout     = 0;

      // XOR check
      byte xor = 0xFF;
      for(i = 0; i < serial_rcv_buf[1]+1; i++) xor ^= serial_rcv_buf[i];
      if (xor == 0) serial_received();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

// parse serial data:
void serial_received(void)
{
  host new_remote;

	switch (serial_rcv_buf[2])
  {
	  case 1: // status message
      new_remote.spr    = (serial_rcv_buf[3] & 0b00000111);
      new_remote.status = ((serial_rcv_buf[3] >> 3) & 0b00000111);
      new_remote.ZAK    = ((serial_rcv_buf[3] >> 6) & 0b00000001);
      new_remote.odklad = ((serial_rcv_buf[3] >> 7) & 0b00000001);

  		if (host_remote.status != new_remote.status)
 		  {
        if ((new_remote.status == 2) || (new_remote.status == 3))
        {
          // beep only when trainset is accepted or canceled
          __REPR_BEEP(50, main_timer_beep_mute);
        }
 		  }

		  if (new_remote.ZAK != host_remote.ZAK)
 		  {
        ZAK_counter = 0;
        __REPR_BEEP(10, main_timer_beep_ZAK);
 		  }

		  //if ((new_remote.odklad != host_remote.odklad) && (new_remote.odklad))
      //  __REPR_BEEP(10, main_timer_beep_mute);


      // cancelling train
      if ((new_remote.status == 3) ||
      ((host_local.status == 3) && (host_remote.status == 0)))
      {
        host_local.status = 0;
        host_local.odklad = false;
        serial_send_stquery();
      }

      if ((new_remote.odklad) && (!host_remote.odklad)) odklad_time = 0;

      host_remote = new_remote;
		break;

	  case 2:
      SET_SEND_OPERATION(_OP_SEND_EEPROM);
	  break;

	  case 3:
      //mazani LOGu
      if (serial_rcv_len < 8) return;
      eeprom_clear();
	    eeprom_write_date(serial_rcv_buf[3], serial_rcv_buf[4], (serial_rcv_buf[5] << 8) + serial_rcv_buf[6]);
      SET_SEND_OPERATION(_OP_SEND_ACK);
	  break;

	  case 4:
      SET_SEND_OPERATION(_OP_SEND_VERSION);
	  break;

  }//switch

  leds_update();
  beep_zadost_update();
}

////////////////////////////////////////////////////////////////////////////////

// serial receive timeout update
// timeout = 200 ms
void serial_update_timeout(void)
{
  if (serial_timeout > 0)
   if (--serial_timeout < 1)
    serial_rcv_buf_pos = 0;
}

////////////////////////////////////////////////////////////////////////////////

void serial_send_eeprom(void)
{
  byte i;
  for (i = 0; i < EEPROM_LEN; i++)
  {
    SERIAL_BUSY_WAIT
    serial_snd_buf[0] = 7;
    serial_snd_buf[1] = 2;
  	serial_snd_buf[2] = i;
  	serial_snd_buf[3] = eeprom_data[i] >> 24;
  	serial_snd_buf[4] = (eeprom_data[i] >> 16) & 0xFF;
  	serial_snd_buf[5] = (eeprom_data[i] >> 8) & 0xFF;
  	serial_snd_buf[6] = eeprom_data[i] & 0xFF;
    serial_send_data();
  }
}

////////////////////////////////////////////////////////////////////////////////

void serial_send_version(void)
{
  dword ver = VERSION;
  SERIAL_BUSY_WAIT
 	serial_snd_buf[0] = 6;
 	serial_snd_buf[1] = 4;
 	serial_snd_buf[2] = (ver >> 24) & 0xFF;
	serial_snd_buf[3] = (ver >> 16) & 0xFF;
	serial_snd_buf[4] = (ver >> 8) & 0xFF;
	serial_snd_buf[5] = ver & 0xFF;
  serial_send_data();
}

////////////////////////////////////////////////////////////////////////////////
// send standard serial query:

void serial_send_stquery(void)
{
  // in this case, we dont mind not sending one standard query, so we can exit without problem
  if (serial_snd_buf_pos > 0) return;
  serial_snd_buf[0] = 3;
  serial_snd_buf[1] = 1;
  serial_snd_buf[2] = host_local.spr + (host_local.status << 3) + (host_local.ZAK << 6) + (host_local.odklad << 7);
  serial_send_data();
}

////////////////////////////////////////////////////////////////////////////////

void serial_send_ACK(void)
{
  SERIAL_BUSY_WAIT
  serial_snd_buf[0] = 2;
  serial_snd_buf[1] = 3;
  serial_send_data();
}

////////////////////////////////////////////////////////////////////////////////

void serial_update_send(void)
{
  if (send_operation == _OP_SEND_EEPROM) serial_send_eeprom();
  else if (send_operation == _OP_SEND_ACK) serial_send_ACK();
  else if (send_operation == _OP_SEND_VERSION) serial_send_version();
  send_operation = _OP_NULL;
}

////////////////////////////////////////////////////////////////////////////////
