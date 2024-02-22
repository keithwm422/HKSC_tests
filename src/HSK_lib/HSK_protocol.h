/*
 * HSK_protocol.h
 *
 * Declares the interface protocol for cross device communication.
 *
 * Housekeeping packet consists of header
 * 0-255 payload bytes
 * 1 byte CRCS (or checksum)
 */

/*****************************************************************************
 * Defines
 ****************************************************************************/
#pragma once
#include <stdint.h>

#define MIN_PERIOD      100

/*******************************************************************************
 * Typedef enums
 *******************************************************************************/

/* Command definitions */
namespace HSK_cmd {enum {
  // 2-248 are board-specific: these are test commands
	eIntSensorRead = 0x02,
  eConfigureGPIOs=0x03,
  eOutputsHigh=0x04,
  eOutputsLow=0x05,
  ePWRTopMuxSelect=0x06,
  ePWRMuxSelect=0x07,

  eTimeStore=0x54,

  eISR=0xA0,
  eBigPacket=0xA1,
  ePacketCount = 0xA2,
}; };


