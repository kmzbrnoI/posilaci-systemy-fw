/****************************************************************************
    Copyright (C) 2002 Alex Shepherd

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************

 Title :   Commin definitions
 Author:   Alex Shepherd <kiwi64ajs@sourceforge.net>
 Date:     16-Mar-2003
 Software:  AVR-GCC
 Target:    AtMega8

 DESCRIPTION
       LocoNet throttle common definitions

*****************************************************************************/

#ifndef __COMMON_DEFS_DEFINED
#define __COMMON_DEFS_DEFINED

#ifndef __BYTE_DEFINED
#define __BYTE_DEFINED
typedef unsigned char byte;
#endif

#ifndef __WORD_DEFINED
#define __WORD_DEFINED
#ifdef __BORLANDC__
typedef unsigned short word ;
#else
typedef unsigned int word ;
#endif
#endif

#ifndef __DWORD_DEFINED
#define __DWORD_DEFINED
#ifdef __BORLANDC__
typedef unsigned int dword;
#else
typedef unsigned long dword;
#endif
#endif

#define true 1
#define false 0
#define BV(bit) _BV(bit)

#define hi(x)      ((x)>>8)
#define lo(x)      ((x) & 0xFF)

#endif
