////////////////////////////////////////////////////////////////////////////////
// eeprom.c
// Posilaci systemy
// eeprom logging execitable file
// (c) Jan Horacek
// jan.horacek@seznam.cz
// 24.12.2014
// v4.1
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

#include "common_defs.h"
#include "eeprom.h"
#include <avr/eeprom.h>

dword EEPROM [EEPROM_LEN] EEMEM = {0,0,0,0,0,0,0};
dword eeprom_data[EEPROM_LEN];

// macro to write data into eeprom
#define __UPDATE_EEPROM(addr, val) \
  eeprom_data[addr] = val; \
  eeprom_busy_wait(); \
  eeprom_write_dword(&EEPROM[addr], eeprom_data[addr])

void eeprom_init()
{
  int i;
  for (i = 0; i < EEPROM_LEN; i++)
  {
    eeprom_busy_wait();
    eeprom_data[i] = eeprom_read_dword(&EEPROM[i]);
    if (eeprom_data[i]+1 == 0) __UPDATE_EEPROM(i, 0);   // bacause of default EEPROM
  }
}

void eeprom_log_button(byte eeprom_index)
{
  __UPDATE_EEPROM(eeprom_index, eeprom_data[eeprom_index]+1);
}

void eeprom_write_date(byte day, byte month, unsigned short int year)
{
  int eeprom_index = EEPROM_INDEX_DATE;
  __UPDATE_EEPROM(eeprom_index, (((dword)day << 24) + ((dword)month << 16) + year));
}

void eeprom_clear()
{
  int i;
	for (i = 0; i < EEPROM_LEN; i++)
    __UPDATE_EEPROM(i, 0);
}