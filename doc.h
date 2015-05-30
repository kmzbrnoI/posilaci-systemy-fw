////////////////////////////////////////////////////////////////////////////////
// leds.c
// Posilaci systemy
// System documantationn
// (c) Jan Horacek
// jan.horacek@seznam.cz
// 24.10.2014
// v1.0.0
// License: Apache License v2.0
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

//zelena  = zadost
//cervena = prijem

//	***** VYUZITI PINU PROCESORU *****
//				     _______
//				    |       |
//RESET		-	    |01	  28|	-	LED_Pn2
//READUSB	-	    |02	  27|	-	LED_Pn1
//SENDUSB	-	    |03	  26|	-	LED_Mn2
//TLAC_Zrus	-	    |04	  25|	-	LED_Mn1
//LED_ZAK	-	    |05	  24|	-
//IN_ZAK	-	    |06	  23|	-
//VCC		-	    |07	  22|	-	GND
//GND		-	    |08	  21|	-
//KRYSTAL1	-	    |09	  20|	-	VCC
//KRYSTAL2	-	    |10	  19|	-	Repracek
//TLAC_Pn	-	    |11	  18|	-	LED_Os2
//TLAC_Mn	-	    |12	  17|	-	LED_Os1
//TLAC_Os	-	    |13	  16|	-	LED_MOs2
//TLAC_MOs	-	    |14	  15|	-   LED_MOs1
//				    |_______|
//				
	
//			   	 ***** PINY PROCESORU *****
//						         _______
//						        |	   	|
//(RESET)		PC6   -	  		|01   28|   -	  PC5	(ADC5/SCL)
//(RXD)			PD0   -	  		|02   27|   -	  PC4	(ADC4/SDA)
//(TXD)			PD1   -	  		|03   26|   -	  PC3	(ADC3)
//(INT0)		PD2   -	  		|04   25|   -	  PC2	(ADC2)
//(INT1)		PD3   -	  		|05   24|   -	  PC1	(ADC1)
//(XCK/T0)	PD4   -	  			|06   23|   -	  PC0	(ADC0)
//				  VCC   -	  	|07   22|   -	  GND
//				  GND   -	  	|08   21|   -	  AREF
//(XTAL1/TOSC1)	PB6 - 			|09   20|   -	  AVCC
//(XTAL2/TOSC2)	PB7 - 			|10   19|   -	  PB5	(SCK)
//(T1)			PD5 -	  		|11   18|   -	  PB4	(MISO)
//(AIN0)		PD6 -	  		|12   17|   -	  PB3	(MOSI/OC2)
//(AIN1)		PD7 -	  		|13   16|   -	  PB2	(SS/OC1B)
//(ICP1)		PB0 -	  		|14   15|   -	  PB1	(OC1A)
//						        |_______|

/*
 Stav zadosti (in, out) :
  0 : nic se nedeje
  1 : zadost
  2 : udeleni souhlasu
  3 : ruseni
 ----------------------------
 Protokol komunikace mezi 2-mi krabickami posilacich systemu:
 	vstupni a vystupni zprava je formatem totozna
  0-byte : inicializacni byte
  1-byte : delka zpravy
  2-byte : typ zpravy
   1 : peer-to-peer zprava mezi PosS
  3-byte : data
   0-2 bit: typ vlaku
    0 : MOs
	  1 : Os
	  2 : Mn
	  3 : Pn
   3-5 bit: stav zadosti
    0 : nic
	  1 : zadost
	  2 : udeleni souhlasu
	  3 : ruseni
   6 bit: ZAK
   7 bit: Odklad
  4-byte : xor
 ----------------------------

 Protokol komunikace PC -> pos. s.
 0 : inicializacni byte
 1 : delka zpravy
 2 : typ zpravy
 	2 : pozadavek na zjisteni staistiky
	3 : ACK
	4 : pozadavek na zjisteni verze pos. s.
 --- data ---
 n-1 : xor

 ----------------------------

 Protokol komunikace pos. s. -> PC
 0 : delka zpravy
 1 : typ zpravy
 	2 : odeslani statistiky
	3 : ACK
	4 : odeslani verze pos. s.
 --- data ---
 n-1 : xor

 ----------------------------
 format statistiky:
  00 - 03	byte = MOs out
  04 - 07	byte = Os out
  08 - 11	byte = Mn out
  12 - 15	byte = Pn out
  16 - 19	byte = Zrus out
  20 - 23	byte = ZAK on out
  24		byte = date: day
  25		byte = date: month
  26		byte = date: year hi
  27		byte = date: year lo

 ----------------------------
 //format verze: 4 byty
 // 0 = major
 // 2 = minor
 // 3 = release
 // 4 = build
*/

/*
 prevedeni souboru z EEPROM - cmd - "avr-objcopy -I ihex -O binary PosSystemy.hex PosS.bin"
*/
