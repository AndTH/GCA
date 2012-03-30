/*
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

#include <string.h>

#include "StackTsk.h"
#include "TCP.h"
#include "Helpers.h"

#include "cbuseth.h"
#include "cbus.h"
#include "commands.h"
#include "utils.h"
#include "io.h"
#include "cangc1e.h"
#include "cbusdefs.h"

#define WAIT_FOR_ALL_READY

static ram CANMsg ETHMsgs[CANMSG_QSIZE];
static ram byte lastRC = 0;


/*
 * CBUSETH Connection Info - one for each connection.
 */
typedef struct _CBUSETH_INFO
{
    TCP_SOCKET socket;
    unsigned char idle;
} CBUSETH_INFO;
typedef BYTE CBUSETH_HANDLE;




static CBUSETH_INFO HCB[MAX_CBUSETH_CONNECTIONS];
static byte nrClients = 0;



static byte CBusEthProcess(void);



    /* Frame ASCII format
     * :ShhhhNd0d1d2d3d4d5d6d7d; :XhhhhhhhhNd0d1d2d3d4d5d6d7d; :ShhhhR; :SB020N;
     * :S    -> S=Standard X=extended start CAN Frame
     * hhhh  -> SIDH<bit7,6,5,4=Prio bit3,2,1,0=high 4 part of ID> SIDL<bit7,6,5=low 3 part of ID>
     * Nd    -> N=normal R=RTR
     * 0d    -> OPC 2 byte HEXA
     * 1d-7d -> data 2 byte HEXA
     * ;     -> end of frame
     */

static char hexa[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
static byte msg2ASCII(CANMsg* msg, char* s) {
  if( msg->b[sidl] & 0x08 ) {
    /* Extended Frame */
    byte len = msg->b[dlc] & 0x0F;
    byte i;
    s[0] = ':';
    s[1] = 'X';
    s[2] = hexa[msg->b[sidh] >> 4];
    s[3] = hexa[msg->b[sidh] & 0x0F];
    s[4] = hexa[msg->b[sidl] >> 4];
    s[5] = hexa[msg->b[sidl] & 0x0F];
    s[6] = hexa[msg->b[eidh] >> 4];
    s[7] = hexa[msg->b[eidh] & 0x0F];
    s[8] = hexa[msg->b[eidl] >> 4];
    s[9] = hexa[msg->b[eidl] & 0x0F];
    s[10] = 'N';
    for( i = 0; i < len; i++) {
      s[11 + i*2]     = hexa[msg->b[d0+i] >> 4];
      s[11 + i*2 + 1] = hexa[msg->b[d0+i] & 0x0F];
    }
    s[11+i*2] = ';';
    return 11+i*2+1;
  }
  else {
    /* Standard Frame */
    byte len = getDataLen(msg->b[d0], ((msg->b[dlc]&0x80)?TRUE:FALSE));
    byte i;
    byte idx = 9;
    s[0] = ':';
    s[1] = ((msg->b[dlc] & 0x80) ? 'Y':'S');
    s[2] = '0';
    s[3] = '0';
    s[4] = '0';
    s[5] = '0';
    s[6] = 'N';
    s[7] = hexa[msg->b[d0] >> 4];
    s[8] = hexa[msg->b[d0] & 0x0F];
    for( i = 0; i < len; i++) {
      s[idx + i*2]     = hexa[msg->b[d1+i] >> 4];
      s[idx + i*2 + 1] = hexa[msg->b[d1+i] & 0x0F];
    }
    s[idx+i*2] = ';';
    return idx+i*2+1;
  }

}

/*  StrOp.fmtb( frame+1, ":%c%02X%02XN%02X;", eth?'Y':'S', (0x80 + (prio << 5) + (cid >> 3)) &0xFF, (cid << 5) & 0xFF, cmd[0] );*/
static char hexb[] = {0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,0,10,11,12,13,14,15};
static byte ASCII2Msg(unsigned char* ins, byte inlen, CANMsg* msg) {
  byte len = 0;
  byte i;
  byte type = CAN_FRAME;
  byte sidh, sidl, canid;

  unsigned char* s = ins;
  for( i = 0; i < inlen; i++ ) {
    if( s[i] == ':' && ( s[i+1] == 'S' || s[i+1] == 'Y' ) ) {
      if( s[i+1] == 'Y' ) {
        type = ETH_FRAME;
      }
      s += i;
      break;
    }
    else if( s[i] == ':' && s[i+1] == 'X' ) {
      type = EXT_FRAME;
      s += i;
      break;
    }
  }
  if( i == inlen ) {
    lastRC = RC_UNKNOWN_ASCII_FRAME;
    return 0;
  }

  if( type == EXT_FRAME ) {
    msg->b[sidh] = (hexb[s[2]-0x30]<<4) + hexb[s[3]-0x30];
    msg->b[sidl] = (hexb[s[4]-0x30]<<4) + hexb[s[5]-0x30];
    msg->b[eidh] = (hexb[s[6]-0x30]<<4) + hexb[s[7]-0x30];
    msg->b[eidl] = (hexb[s[8]-0x30]<<4) + hexb[s[9]-0x30];
    // copying all data bytes:
    for( i = 0; i < 8 && s[10+2*i] != ';'; i++ ) {
      msg->b[d0+i] = (hexb[s[10+2*i]-0x30]<<4) + hexb[s[10+2*i+1]-0x30];
    }
    msg->b[dlc] = i;
    msg->b[sidl] |= 0x08; // signal extended message
  }
  else {
    sidh = (hexb[s[1]-0x30]<<4) + hexb[s[2]-0x30];
    sidl = (hexb[s[3]-0x30]<<4) + hexb[s[4]-0x30];
    canid = (sidl >> 5 ) + ((sidh&0x0F) << 3);

    if( CANID != canid ) {
      CANID = canid;
    }

    msg->b[d0] = (hexb[s[7]-0x30]<<4) + hexb[s[8]-0x30];
    len = getDataLen(msg->b[d0], FALSE);
    for( i = 0; i < len; i++ ) {
      msg->b[d1+i] = (hexb[s[9+2*i]-0x30]<<4) + hexb[s[9+2*i+1]-0x30];
    }
    msg->b[dlc] = len + 1;
  }
  return type;
}

void CBusEthInit(void)
{
  BYTE i;

  for ( i = 0; i <  MAX_CBUSETH_CONNECTIONS; i++ ) {
    HCB[i].socket = TCPListen(CBUSETH_PORT);
    HCB[i].idle = 0;
  }

  for( i = 0; i < CANMSG_QSIZE; i++ ) {
    ETHMsgs[i].status = CANMSG_FREE;
  }

}


void CBusEthBroadcastAll(void) {
  BYTE i;
  char idx = -1;
  for( i = 0; i < CANMSG_QSIZE; i++ ) {
    if( ETHMsgs[i].status == CANMSG_PENDING )
      ETHMsgs[i].status = CANMSG_FREE;
    else if( idx == -1 && ETHMsgs[i].status == CANMSG_OPEN ) {
      idx = i;
    }
  }

  if( idx != -1 ) {
    if( nrClients == 0 || CBusEthBroadcast(&ETHMsgs[idx]) ) {
      ETHMsgs[idx].status = CANMSG_PENDING;
    }
  }

}


void CBusEthServer(void)
{
  BYTE i;
  char idx = -1;
  byte nrconn = 0;

  nrconn = CBusEthProcess();

  if( nrconn != nrClients ) {
    CANMsg canmsg;
    canmsg.b[d0]  = 1;
    canmsg.b[d1]  = 0;
    canmsg.b[d2]  = nrconn;
    canmsg.b[d3]  = maxcanq;
    canmsg.b[d4]  = maxethq;
    canmsg.b[dlc] = 0x80 + 5;
    ethQueue(&canmsg);
    nrClients = nrconn;
  }


  CBusEthBroadcastAll();

}


static byte CBusEthProcess(void)
{
  byte conn;
  byte nrconn = 0;

  for ( conn = 0;  conn < MAX_CBUSETH_CONNECTIONS; conn++ ) {
    BYTE cbusData[MAX_CBUSETH_CMD_LEN+1];
    CBUSETH_INFO* ph = &HCB[conn];

    if ( !TCPIsConnected(ph->socket) ) {
      ph->idle = 0;
      continue;
    }
    nrconn++;

    if ( TCPIsGetReady(ph->socket) ) {
        BYTE len = 0;
        byte type;
        CANMsg canmsg;
        ph->idle = 0;

        while( len < MAX_CBUSETH_CMD_LEN && TCPGet(ph->socket, &cbusData[len++]) );
        cbusData[len] = '\0';
        TCPDiscard(ph->socket);
        // TODO: Check if the message is valid.
        type = ASCII2Msg(cbusData, len, &canmsg);
        if( type > 0 ) {
          if( parseCmdEth(&canmsg, type) ) {
            canQueue(&canmsg);
          }
        }
    }
    else if( ph->idle > IdleTime ) {
      TCPDisconnect(ph->socket);
      ph->idle = 0;
      if( NV1 & CFG_POWEROFF_ATIDLE ) {
        CANMsg canmsg;
        canmsg.b[d0]  = OPC_RTOF;
        canmsg.b[dlc] = 1;
        canQueue(&canmsg);
      }
    }

  }
    return nrconn;
}


byte CBusEthBroadcast(CANMsg* msg)
{

    BYTE conn;
    char s[32];
    byte len;
    byte wait;

#ifdef WAIT_FOR_ALL_READY
    for ( conn = 0;  conn < MAX_CBUSETH_CONNECTIONS; conn++ ) {
      CBUSETH_INFO* ph = &HCB[conn];
      if ( TCPIsConnected(ph->socket) ) {
        if( !TCPIsPutReady(ph->socket) ) {
          return FALSE;
        }
      }
    }
#endif
    
    len = msg2ASCII(msg, s);
    //len = msg2BinASCII(msg, s);
    for ( conn = 0;  conn < MAX_CBUSETH_CONNECTIONS; conn++ ) {
      CBUSETH_INFO* ph = &HCB[conn];
      if ( TCPIsConnected(ph->socket) )
      {
        wait = 0;
        
#ifndef WAIT_FOR_ALL_READY
        while( !TCPIsPutReady(ph->socket) && wait < 10 ) {
          wait++;
          delay();
        }
        
#endif
        if( TCPIsPutReady(ph->socket) ) {
          TCPPutArray(ph->socket, (byte*)s, len);
          TCPFlush(ph->socket);
        }
        else {
          LED3 = LED_OFF; /* signal error */
          led3timer = 40;
        }
    }
  }
  return TRUE;

}



byte ethQueue(CANMsg* msg) {
  int i = 0;
  for( i = 0; i < CANMSG_QSIZE; i++ ) {
    if( ETHMsgs[i].status == CANMSG_FREE ) {
      memcpy(&ETHMsgs[i], (const void*)msg, sizeof(CANMsg));
      ETHMsgs[i].status = CANMSG_OPEN;
      if( i > maxethq )
        maxethq = i;
      return 1;
    }
  }

  LED3 = LED_OFF; /* signal error */
  led3timer = 40;
  return 0;
}


byte ethQueueRaw(void) {
  int i = 0;

  rx_ptr->con = 0;
  if (can_bus_off) {
    // At least one buffer is now free
    can_bus_off = 0;
    PIE3bits.FIFOWMIE = 1;
  }

  for( i = 0; i < CANMSG_QSIZE; i++ ) {
    if( ETHMsgs[i].status == CANMSG_FREE ) {
      memcpy(ETHMsgs[i].b, (void*)rx_ptr, 14);
      ETHMsgs[i].status = CANMSG_OPEN;
      currentEthQ = i+1;
      if( i+1 > maxethq )
        maxethq = i+1;
      return 1;
    }
  }

  LED3 = LED_OFF; /* signal error */
  led3timer = 40;
  return 0;
}


/* called every 500ms */
void CBusEthTick(void) {
  byte conn;
  if( NV1 & CFG_IDLE_TIMEOUT ) {
    for ( conn = 0;  conn < MAX_CBUSETH_CONNECTIONS; conn++ ) {
      CBUSETH_INFO* ph = &HCB[conn];
      if( TCPIsConnected(ph->socket) ) {
        ph->idle++;
      }
    }
  }
}
