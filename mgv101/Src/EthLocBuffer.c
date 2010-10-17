
/**
 *******************************************************************************************
 * @file        EthLocBuffer.c
 * @ingroup     EthLocBuffer
 * @attention
 *******************************************************************************************
 */

/*
 *******************************************************************************************
 * Standard include files
 *******************************************************************************************
 */

#include <inttypes.h>
#include <string.h>
#include "stack.h"
#include "UserIo.h"
#include "enc28j60.h"
#include "ln_sw_uart.h"
#include "EthLocBuffer.h"

/*
 *******************************************************************************************
 * Macro definitions
 *******************************************************************************************
 */

/* *INDENT-OFF* */

#define ETH_LOC_BUFFER_MAX_LOCONET_TX     20          /**< Max number of tries * LN_TX_RETRIES_MAX !! */
#define ETH_LOC_BUFFER_BUFFER_TCP_PORT    1235        /**< Port to listen / write */
/*
 *******************************************************************************************
 * Types
 *******************************************************************************************
  */

/*
 *******************************************************************************************
 * Variables
 *******************************************************************************************
 */

static LnBuf      EthLocBufferLocoNet;                /**< Loconet buffer */
static uint8_t    EthLocBufferTcpIpIndex;             /**< Index for TcpIp transmit */
static uint8_t    EthLocBufferTcpContinue;            /**< Process next Loconet message */
static lnMsg      EthLocBufferTcpLoconetData;         /**< Copy of last received Loconet data for retry on TCPIP */
static uint8_t    EthLocBufferTcpLoconetDataLenght;   /**< Copy of length last received Loconet data for retry on TCPIP */
/* *INDENT-ON* */

/*
 *******************************************************************************************
 * Routines implementation
 *******************************************************************************************
 */

/**
 *******************************************************************************************
 * @fn         static void EthLocBufferRcvLocoNet(lnMsg * LnPacket)
 * @brief      Process the content of a received message from the Loconet bus and
 *             forward it on to the TCP/IP.
 * @param      LnPacket  Pointer to Loconet variable.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */
static void EthLocBufferRcvLocoNet(lnMsg * LnPacket)
{
   uint8_t                                 OpCode;
   uint8_t                                 OpcodeFamily;
   uint8_t                                 RxLength;

   /* Blink led to indicate something is received and processed from Loconet bus */
   UserIoSetLed(userIoLed6, userIoLedSetFlash);

   /* Check received data from Loconet */
   OpCode = (uint8_t) LnPacket->sz.command;
   OpcodeFamily = (OpCode >> 5);
   switch (OpcodeFamily)
   {
      /* *INDENT-OFF* */
      case 0b100:
         /* 2 data bytes, inc checksum */
         RxLength = 2;
         break;
      case 0b101:
         /* 4 data bytes, inc checksum */
         RxLength = 4;
         break;
      case 0b110:
         /* 6 data bytes, inc checksum */
         RxLength = 6;
         break;
      case 0b111:
         /* N data bytes, inc checksum, next octet is N */
         RxLength = LnPacket->sz.mesg_size;
         break;
      default:
         /* Unknown... */
         RxLength = 0;
         break;
      /* *INDENT-ON* */
   }

   /* Copy the data so it can be transmitted to RocRail */
   if (RxLength != 0)
   {
      /* If a connection to us is present then transmit new data */
      if (EthLocBufferTcpIpIndex != 255)
      {
         memcpy(&eth_buffer[TCP_DATA_START_FIX], (char *)LnPacket->data, RxLength);
         tcp_entry[EthLocBufferTcpIpIndex].status = ACK_FLAG | PSH_FLAG;
         create_new_tcp_packet(RxLength, EthLocBufferTcpIpIndex);
         tcp_packet_retry_tx_reset(EthLocBufferTcpIpIndex);
         tcp_packet_retry_tx_set(EthLocBufferTcpIpIndex);

         memcpy(EthLocBufferTcpLoconetData.data, (char *)LnPacket->data, RxLength);
         EthLocBufferTcpLoconetDataLenght = RxLength;
         EthLocBufferTcpContinue = 0;
      }
   }
}

/**
 *******************************************************************************************
 * @fn	      void EthLocBufferRcvEthernet(uint8_t *RcvData, uint16_t RcvLength)
 * @brief      Check the content of a received message from the ethernet bus.
 * @param      RcvData  Pointer to array with received data.
 * @param      RcvLength Number of received bytes.
 * @return     None
 * @attention  The stack uses calls this function also in case a retry must be generated.
 *******************************************************************************************
 */

static void EthLocBufferTcpRcvEthernet(unsigned char TcpFpIndex)
{
   uint8_t                                 TxMaxCnt = 0;
   uint8_t                                 OpCode = 0;
   uint8_t                                 OpcodeFamily = 0;
   LN_STATUS                               TxStatus = LN_DONE;
   lnMsg                                   LocoNetSendPacket;

   if ((tcp_entry[TcpFpIndex].status & PSH_FLAG) == PSH_FLAG)
   {
      /* Check if received data is Loconet format */
      OpCode = (uint8_t) eth_buffer[TCP_DATA_START_FIX];
      OpcodeFamily = (OpCode >> 5);
      switch (OpcodeFamily)
      {
           /* *INDENT-OFF* */
           case 0b100:
              /* 2 data bytes, inc checksum */
              LocoNetSendPacket.sz.mesg_size = 2;
              break;
           case 0b101:
              /* 4 data bytes, inc checksum */
              LocoNetSendPacket.sz.mesg_size = 4;
              break;
           case 0b110:
              /* 6 data bytes, inc checksum */
              LocoNetSendPacket.sz.mesg_size = 6;
              break;
           case 0b111:
              /* N data bytes, inc checksum, next octet is N */
              LocoNetSendPacket.sz.mesg_size = eth_buffer[TCP_DATA_START_FIX+1];
              break;
           default:
              /* Unknown... */
              LocoNetSendPacket.sz.mesg_size = 0;
              break;
           /* *INDENT-ON* */
      }

      if (LocoNetSendPacket.sz.mesg_size > 0)
      {
         /* Blink led to indicate Loconet data is received from RocRail */
         UserIoSetLed(userIoLed5, userIoLedSetFlash);

         /* Copy the data so it can be transmitted to the Loconet */
         memcpy(LocoNetSendPacket.data, &eth_buffer[TCP_DATA_START_FIX], LocoNetSendPacket.sz.mesg_size);

         if (EthLocBufferTcpIpIndex == 255)
         {
            EthLocBufferTcpIpIndex = TcpFpIndex;
         }

         do
         {
            TxStatus = sendLocoNetPacket(&LocoNetSendPacket);
            TxMaxCnt++;
         }
         while ((TxStatus != LN_DONE) && (TxMaxCnt < ETH_LOC_BUFFER_MAX_LOCONET_TX));
         /* Message probably transmitted, ack the TCP/IP data */
         tcp_entry[TcpFpIndex].status = ACK_FLAG;
         create_new_tcp_packet(0, TcpFpIndex);
      }
   }
   else if ((tcp_entry[TcpFpIndex].status & RETRY_FLAG) == RETRY_FLAG)
   {
      /* Retransmit last transmitted data */
      tcp_entry[EthLocBufferTcpIpIndex].status = ACK_FLAG | PSH_FLAG;
      memcpy(&eth_buffer[TCP_DATA_START_FIX], EthLocBufferTcpLoconetData.data, EthLocBufferTcpLoconetDataLenght);
      create_new_tcp_packet(EthLocBufferTcpLoconetDataLenght, EthLocBufferTcpIpIndex);
      tcp_packet_retry_tx_set(EthLocBufferTcpIpIndex);
   }
   else if ((tcp_entry[TcpFpIndex].status & ACK_FLAG) == ACK_FLAG)
   {
      /* Ack received on last transmitted data. New messages from Loconet cam be forwarded. */
      tcp_packet_retry_tx_clear(EthLocBufferTcpIpIndex);
      EthLocBufferTcpContinue = 1;
   }
   else if ((tcp_entry[TcpFpIndex].status & FIN_FLAG) == FIN_FLAG)
   {
      /* Connection ended from PC side. */
      EthLocBufferTcpIpIndex = 255;
      EthLocBufferTcpContinue = 255;
      /* Blink TCPIP led indicating no connection is present to RocRail */
      UserIoSetLed(userIoLed4, userIoLedSetOff);
      UserIoSetLed(userIoLed4, userIoLedSetBlink);
      UserIoSetLed(userIoLed5, userIoLedSetBlink);
   }
   else if ((tcp_entry[TcpFpIndex].status & RETRY_ABORT_FLAG) == RETRY_ABORT_FLAG)
   {
      /* Connection ended because max number of retries for tx. */
      EthLocBufferTcpIpIndex = 255;
      EthLocBufferTcpContinue = 255;
      /* Blink TCPIP led indicating no connection is present to RocRail */
      UserIoSetLed(userIoLed4, userIoLedSetOff);
      UserIoSetLed(userIoLed4, userIoLedSetBlink);
      UserIoSetLed(userIoLed5, userIoLedSetBlink);
   }
}

/**
 *******************************************************************************************
 * @fn	      void EthLocBufferInit(void)
 * @brief      Init the ENC2860 network chip and init the loconet interface.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */

void EthLocBufferInit(void)
{
   uint8_t                                 IpAddress[4];
   uint8_t                                 NetMask[4];
   uint8_t                                 RouterIp[4];
   uint8_t                                 MacData[6] = { 0x4D, 0x47, 0x56, 0x31, 0x30, 0x31 };

   /* Set up the network interface */
   UserIoIpSettingsGet(IpAddress, NetMask, RouterIp);
   stack_set_ip_settings(IpAddress, NetMask, RouterIp);
   stack_init();
   enc28j60SetMac(MacData);
   add_tcp_app(ETH_LOC_BUFFER_BUFFER_TCP_PORT, (void (*)(unsigned char))EthLocBufferTcpRcvEthernet);

   /* Init loconet */
   initLocoNet(&EthLocBufferLocoNet);

   /* Set used variables to initial values */
   EthLocBufferTcpIpIndex = 255;
   EthLocBufferTcpContinue = 0;
}

/**
 *******************************************************************************************
 * @fn	      void EthLocBufferMain(void)
 * @brief      Verify if data is present in the network interface or locnet <br>
 *             interface and process it.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */

void EthLocBufferMain(void)
{
   lnMsg                                  *RxPacket;

   /* Update the network stack */
   eth_get_data();

   /* Handle ping stuff */
   if (ping.result)
   {
      UserIoSetLed(userIoLed5, userIoLedSetFlash);
      ping.result = 0;
   }

   /* If no TCPIP connection discard all received Loconet data */
   if (EthLocBufferTcpIpIndex == 255)
   {
      RxPacket = recvLocoNetPacket();
      if (RxPacket)
      {
         UserIoSetLed(userIoLed6, userIoLedSetFlash);
      }
   }
   else if (EthLocBufferTcpContinue != 0)
   {
      RxPacket = recvLocoNetPacket();
      if (RxPacket)
      {
         /* Something received from Loconet interface, process it */
         EthLocBufferTcpContinue = 0;
         EthLocBufferRcvLocoNet(RxPacket);
      }
   }
}
