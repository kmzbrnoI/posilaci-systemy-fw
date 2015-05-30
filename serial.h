////////////////////////////////////////////////////////////////////////////////
// serial.h
// Posilaci systemy
// Serial bus (RS232, uart) header file
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

#ifndef _SERIAL_H_
#define _SERIAL_H_

////////////////////////////////////////////////////////////////////////////////

void serial_update_timeout(void);
void serial_update_send(void);

void serial_send_eeprom(void);
void serial_send_version(void);
void serial_send_stquery(void);
void serial_send_ACK(void);

////////////////////////////////////////////////////////////////////////////////

#endif