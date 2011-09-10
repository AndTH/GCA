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


#include "project.h"
#include "isr.h"
#include "cangc2.h"
#include "io.h"

#pragma udata access VARS
near unsigned short long slot_timer;
near unsigned short long io_timer;

#pragma code APP

//
// Interrupt Service Routine
//
// TMR0 generates a heartbeat every 58uS to generate timing for the DCC bit
// stream. If no DCC packet to be transmitted then preamble is generated.
//
// One bit half cycle is 58usec. Zero bit half cycle is 116usec.
//
// A/D readings are stored in RAM.
//
#pragma interrupt isr_high
void isr_high(void) {
    // 13 clocks to get here after interrupt
    INTCONbits.T0IF = 0;
    TMR0L = tmr0_reload;

    //
    // I/O timeout - 50ms
    //
    if (--io_timer == 0) {
      io_timer = ((short long)25000)/58;
      doIOTimers();
    }

    //
    // Timer 500ms
    //
    if (--slot_timer == 0) {
        slot_timer = ((short long)250000)/58;
        doLEDs();

      if (can_transmit_timeout != 0) {
        --can_transmit_timeout;
      }
    }

}


//
// Low priority interrupt. Used for CAN receive.
//
#pragma interruptlow isr_low
void isr_low(void) {

  // If FIFO watermark interrupt is signalled then we send a high
  // priority OPC_HLT to halt the CBUS. The packet has been preloaded
  // in TXB0
  if (PIR3bits.FIFOWMIF == 1) {
    TXB0CONbits.TXREQ = 1;
    can_bus_off = 1;
    PIE3bits.FIFOWMIE = 0;
  }
  if (PIR3bits.ERRIF == 1) {
    if (TXB1CONbits.TXLARB) { // lost arbitration
      if (Latcount == 0) { // already tried higher priority
        can_transmit_failed = 1;
        TXB1CONbits.TXREQ = 0;
      } else if (--Latcount == 0) { // Allow tries at lower level priority first
        TXB1CONbits.TXREQ = 0;
        Tx1[sidh] &= 0b00111111; // change priority
        TXB1CONbits.TXREQ = 1; // try again
      }
    }
    if (TXB1CONbits.TXERR) { // bus error
      can_transmit_failed = 1;
      TXB1CONbits.TXREQ = 0;
    }

  }

  PIR3 = 0; // clear interrupts
}
