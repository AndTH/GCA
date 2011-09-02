//
// CANCMD Programmer/Command Station (C) 2009 SPROG DCC
//	web:	http://www.sprog-dcc.co.uk
//	e-mail:	sprog@sprog-dcc.co.uk
//

#include "project.h"
#include "isr.h"
#include "cangc2.h"
#include "io.h"

#pragma udata access VARS
near unsigned short long slot_timer;

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
    // TMR0 is enabled all the time and we send a continuous preamble
    // as required for booster operation. The power may, however, be off
    // resulting in no actual DCC ooutput
    //
    // Outputs are toggled here to set the output to the state
    // determined during the previous interrupt. This ensures the output
    // always toggles at the same time relative to the interrupt, regardless
    // of the route taken through the switch() statement for bit generation.
    //
    // This hardware does not use MOSFETs and does not need shoot-through
    // protection delay
    //


    //
    // Slot timeout and other timers - every half second
    //
    if (--slot_timer == 0) {
        slot_timer = ((short long)250000)/58;
        doIOTimers();
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