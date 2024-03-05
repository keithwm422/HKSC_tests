/*
 * Core_protocol.cpp
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

/* Standard error types */
#define EBADCOMMAND     -2  // Packet contained an unknown command
#define EBADLEN         -3  // Packet was the wrong length for the command
#define EBADARGS        -4  // Packet's command arguments (packets of data)
#define EBADPARSE       -5  // Packet's command arguments (packets of data)


/*******************************************************************************
* Typedef enums
*******************************************************************************/
/* Command definitions */
typedef enum housekeeping_cmd
{
	ePingPong = 0x00, // 0
	eSetPriority = 0x01, // 1
	// 2-244 are board specific
	eSetPrioVersion = 0xF5, // 245
	eRestorePrioDefault = 0xF6, // 246
	eSavePrioDefault = 0xF7, // 247
	eSetPrioDefaultLock = 0xF8, // 248
	eTestMode = 0xF9, //249
	eSendLowPriority = 0xFA, // 250
	eSendMedPriority = 0xFB, // 251
	eSendHiPriority = 0xFC, // 252 
	eSendAll = 0xFD, //253
	eReset = 0xFE,  // 254
	eError = 0xFF, // 255
} housekeeping_cmd;


/* Priority definitions */
/*typedef enum housekeeping_prio_type
{
	eNoPriority = 0,
	eLowPriority = 1,
	eMedPriority = 2,
	eHiPriority = 3
} housekeeping_prio_type_t;

*/

/*******************************************************************************
* Typedef structs
*******************************************************************************/
typedef struct housekeeping_hdr_t
{
	    uint8_t magic; // magic byte
        uint8_t cmd; // Command (or response) type
        uint16_t len; // Bytes to follow - 1
} housekeeping_hdr_t;

typedef struct housekeeping_err_t
{
	uint8_t cmd; // Command which caused the error
	uint8_t error; // Error that occured
} housekeeping_err_t;

/*typedef struct housekeeping_prio_t
{
	uint8_t command; // The command we are setting the priority of
	uint8_t prio_type; // The priority level we are setting it to
} housekeeping_prio_t;
*/

/*******************************************************************************
* Functions
*******************************************************************************/
/* Fills the last byte of outgoing packet *p with its checksum */
void fillChecksum(uint8_t* p);

/* Sums packet to make sure its sum is 0 */
bool verifyChecksum(uint8_t * p);

/* Find address in an array of addresses */
uint8_t * findMe(uint8_t * first, uint8_t * last, uint8_t address);
