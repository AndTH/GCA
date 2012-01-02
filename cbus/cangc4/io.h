/*
 Copyright (C) MERG CBUS

 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#ifndef __IO_H
#define __IO_H


#define LED3    PORTBbits.RB5   // cbus activity
#define LED2    PORTBbits.RB7   // learning mode
#define LED1    PORTBbits.RB6   // running

#define SW      PORTAbits.RA2	// Flim switch


#define RFID1   PORTCbits.RC0
#define RFID2   PORTCbits.RC1
#define RFID3   PORTCbits.RC2
#define RFID4   PORTCbits.RC3
#define RFID5   PORTCbits.RC7
#define RFID6   PORTCbits.RC6
#define RFID7   PORTCbits.RC5
#define RFID8   PORTCbits.RC4

#define SENS1   PORTAbits.RA0
#define SENS2   PORTAbits.RA1
#define SENS3   PORTAbits.RA3
#define SENS4   PORTAbits.RA4
#define SENS5   PORTAbits.RA5
#define SENS6   PORTBbits.RB0
#define SENS7   PORTBbits.RB4
#define SENS8   PORTBbits.RB1


typedef struct {
  byte   config;
  byte   data[5];
  int    addr;
} RFIDDef;

typedef struct {
  byte   config;
  byte   status;
  int    addr;
} SENSDef;

#define SERVOCONF_POLAR  0x01
#define SERVOCONF_EXTSEN 0x02

#define TURNOUT_UNKNOWN 0
#define TURNOUT_STRAIGHT 1
#define TURNOUT_THROWN 2


extern ram RFIDDef Reader[8];
extern ram SENSDef Sensor[8];
extern near unsigned char led1timer;


void setupIO(byte clr);
void doLEDTimers(void);
unsigned char checkInput(unsigned char idx, unsigned char sod);
void doLED250(void);
void doLEDs(void);
void saveOutputStates(void);
void setOutput(ushort nn, ushort addr, byte on);
unsigned char checkFlimSwitch(void);

#endif	// __IO_H
