////////////////////////////////////////////////////////////////////////////////
// buttons.h
// Posilaci systemy
// Buttons handling - header file
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

#ifndef _BUTTONS_H_
#define _BUTTONS_H_

////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include "common_defs.h"

////////////////////////////////////////////////////////////////////////////////

// buttons order:
//  [0] = MOs
//  [1] = Os
//  [2] = Mn
//  [3] = Pn
//  [4] = Zrusit
//  [5] = ZAK
//  [6] = Odklad

////////////////////////////////////////////////////////////////////////////////

void btn_init(void);
void btn_update(void);

////////////////////////////////////////////////////////////////////////////////


#endif
