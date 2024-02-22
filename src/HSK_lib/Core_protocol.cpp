/*
 * Core_protocol.cpp
 *
 * Defines the interface protocol for cross device communication.
 *
 * Housekeeping packet consists of header
 * 0-255 payload bytes
 * 1 byte CRCS (or checksum)
 *
 */

/*****************************************************************************
 * Defines
 ****************************************************************************/
#include "Core_protocol.h"

/*******************************************************************************
* Functions
*******************************************************************************/
/* Function flow:
 * --Takes in the location of the first byte of a full packet
 * --Sums all bytes, then inserts that sum mod 255 in the last packet spot
 *
 * Function Params:
 * p:				pointer to the outgoing packet
 *
 * Function variables:
 * data:		pointer to the beginning of the data
 * cksum:		pointer to the location of where the checksum should be
 *
 */
void fillChecksum(uint8_t* p)
{
	housekeeping_hdr_t* hdr = (housekeeping_hdr_t*)p;
	uint8_t* data = p + sizeof(housekeeping_hdr_t);
	uint8_t* cksum = data + hdr->len;
	*cksum = 0;
	for (; p != cksum; p++) *cksum -= *p;
}

/* Function flow:
 * --Adds all bytes in an incoming packet, checks if they sum to 0
 * --Returns true if the sender checksum matches the computed checksum
 *
 * Function Params:
 * p:				pointer to the incoming packet
 *
 * Function variables:
 * data:		pointer to the beginning of the data
 * cksum:		pointer to the location of where the checksum should be
 * sum:			buffer to keep the checksum in
 *
 */
bool verifyChecksum(uint8_t* p)
{
	housekeeping_hdr_t* hdr = (housekeeping_hdr_t*)p;
	uint8_t* data = p + sizeof(housekeeping_hdr_t);
	uint8_t* cksum = data + hdr->len;
	uint8_t sum = 0;
	for (; p <= cksum; p++) sum += *p;
	if ((uint8_t)sum != 0) return false;
	else return true;
}

/* Function flow:
 * --Checks if a device address is inside a given array of addresses
 * --If the device is known (within the list), function returns its location
 *	 in the list
 * --If the device is not known, returns the location of the end of the list
 *
 * Function params:
 * first:		The location of the address known to this device
 * last:		The location of the last address known to this device
 * address:		The name of the address we are trying to find
 *
 */
uint8_t * findMe(uint8_t * first, uint8_t * last, uint8_t address)
{
	while (first!=last)
	{
		if (*first==address) return first;
		++first;
	}
	return last;
}
