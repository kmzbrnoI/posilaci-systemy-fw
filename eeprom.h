////////////////////////////////////////////////////////////////////////////////
// eeprom.h
// Posilaci systemy
// eeprom logging header file
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

#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <avr/eeprom.h>
#include "common_defs.h"

#define EEPROM_LEN          8
#define EEPROM_INDEX_DATE   7

extern dword eeprom_data[EEPROM_LEN];

void eeprom_init(void);
void eeprom_log_button(byte eeprom_index);
void eeprom_write_date(byte day, byte month, unsigned short int year);
void eeprom_clear(void);

#endif