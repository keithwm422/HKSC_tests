/*
 * PacketSerial.h
 *
 * A wrapper class for the Arduino 'Stream' library that enables the use of
 * (in this case) COBS encoding.
 *
 * Copyright (c) 2011 Christopher Baker <https://christopherbaker.net>
 * Copyright (c) 2011 Jacques Fortier <https://github.com/jacquesf/COBS-Consistent-Overhead-Byte-Stuffing>
 *
 * SPDX-License-Identifier:	MIT
 *
 */

#pragma once
/* Max data length is:
 *	-- +4 header bytes
 *	-- +255 data bytes
 *  -- +1 CRC (checksum) byte
 * For max packet size,
 *	-- +2 COBS overhead
 *	-- +1 COBS packet marker
 */
#define MAX_PACKET_LENGTH 1000
#define EBADLEN -3
#define BYTELESS_TIME_LIMIT 10000

#include <Arduino.h>
#include "COBS_encoding.h"

class PacketSerial_
{
public:

/* Packet handler functions are called when a full packet is decoded */
typedef void (*PacketHandlerFunction)(const uint8_t* buffer, size_t size);


typedef void (*PacketHandlerFunctionWithSender)(const void* sender, const uint8_t* buffer, size_t size);

/// \brief Construct a default PacketSerial_ device.
PacketSerial_() :
	_receiveBufferIndex(0),
	_stream(nullptr),
	_onPacketFunction(nullptr),
	_onPacketFunctionWithSender(nullptr)
{
}

/// \brief Destroy the PacketSerial_ device.
~PacketSerial_()
{
}

/* Begin the serial connection */
void begin(unsigned long speed)
{
	Serial.begin(speed);
  #if ARDUINO >= 100 && !defined(CORE_TEENSY)
	while (!Serial) {;}
  #endif
	setStream(&Serial);
}


void begin(unsigned long speed, size_t port) __attribute__ ((deprecated))
{
	switch(port)
	{
  #if defined(UBRR1H)
	case 1:
		Serial1.begin(speed);
    #if ARDUINO >= 100 && !defined(CORE_TEENSY)
		while (!Serial1) {;}
    #endif
		setStream(&Serial1);
		break;
  #endif
  #if defined(UBRR2H)
	case 2:
		Serial2.begin(speed);
    #if ARDUINO >= 100 && !defined(CORE_TEENSY)
		while (!Serial1) {;}
    #endif
		setStream(&Serial2);
		break;
  #endif
  #if defined(UBRR3H)
	case 3:
		Serial3.begin(speed);
    #if ARDUINO >= 100 && !defined(CORE_TEENSY)
		while (!Serial3) {;}
    #endif
		setStream(&Serial3);
		break;
  #endif
	default:
		begin(speed);
	}
}


void begin(Stream* stream) __attribute__ ((deprecated))
{
	_stream = stream;
}

void setStream(Stream* stream)
{
	_stream = stream;
}

/* Function flow:
 * --Reads in one byte at a time
 * --If the packetMarker is received, the function decodes the COBS encoded
 *   packet and executes the PacketReceivedFunction.
 * --Returns the number of bytes decoded.
 *
 * Function variables:
 * data:                Buffer to store one byte from the incoming data stream
 * _decodeBuffer:       Buffer create to store decoded packet
 * _receiveBuffer:      Buffer to store incoming data
 * _receiveBufferIndex: Current location of the read from the data stream
 * time_LastByteReceived:   Time stamp for when the last byte without a complete packet
 * time_Current:            Time stamp for the current time
 * clockNeedsReset:         Bool for when the time stamp should reset
 *
 */
uint8_t update()
{
	/* If this instance has not been passed a stream, return */
	if (_stream == nullptr) return 0;

	/* Evaluate time stamps */
	if (checkForBadPacket()) return EBADLEN;

	/* While there are bytes available to read */
	while (_stream->available() > 0)
	{
		/* Read in one bte */
		uint8_t data = _stream->read();

		/* Evaluate time stamps */
		if (checkForBadPacket()) return EBADLEN;

		/* If that byte is the packet marker, decode the message */
		if (data == PACKETMARKER)
		{
			/* Stop the clock */
			OK_toGetCurrTime = false;

			uint8_t _decodeBuffer[_receiveBufferIndex];

			if (_onPacketFunction || _onPacketFunctionWithSender)
			{
				size_t numDecoded = COBS::decode(_receiveBuffer,
				                                 _receiveBufferIndex,
				                                 _decodeBuffer);
				/* If one zero byte was found, discard it */
				if (numDecoded == 0){
                                  _receiveBufferIndex=0;
                                  return 0;
                                }
				/* Execute whichever function was defined (w/ or w/o sender) */
				if (_onPacketFunction)
				{
					_onPacketFunction(_decodeBuffer, numDecoded);
				}
				else if (_onPacketFunctionWithSender)
				{
					_onPacketFunctionWithSender(this, _decodeBuffer, numDecoded);
				}
			}
			/* Clear the buffer */
			_receiveBufferIndex = 0;
			return 0;
		}
		/* If not, add it to the encoded packet being received */
		else
		{
			time_LastByteReceived = micros();
			OK_toGetCurrTime = true;

			if ((_receiveBufferIndex + 1) < MAX_PACKET_LENGTH)
			{
				_receiveBuffer[_receiveBufferIndex++] = data;
			}
			// Error, buffer overflow if we write.
			else
			{
				_receiveBufferIndex = 0;
				return EBADLEN;
			}
		}
	}

	return 0;
}

/* Function flow:
 * --Send function takes a non-COBS encoded input, encodes it, and writes it
 *   to the serial line.
 *
 * Function params:
 * buffer:		The message that you want to encode and send
 * size:        The size of the message in bytes
 *
 * Function variables:
 * encodedBuffer:	buffer to put the encoded message into
 *
 */
void send(const uint8_t* buffer, size_t size) const
{
	if(_stream == nullptr || buffer == nullptr || size == 0) return;

	uint8_t _encodeBuffer[COBS::getEncodedBufferSize(size)];

	size_t numEncoded = COBS::encode(buffer, size, _encodeBuffer);

	_stream->write(_encodeBuffer, numEncoded);
}

bool checkForBadPacket()
{
	if (OK_toGetCurrTime)
	{
/*		time_Current = micros();
		byteless_interval = time_Current - time_LastByteReceived;
*/
	    byteless_interval=(long) (micros() - time_LastByteReceived);
		/* If an incomplete packet was received */
		if (byteless_interval > BYTELESS_TIME_LIMIT)
		{
			OK_toGetCurrTime = false;
			_receiveBufferIndex = 0;
			return true;
		}
	}
	return false;
}

/* Sets the packet handler for what to do if a packet is received */
void setPacketHandler(PacketHandlerFunction onPacketFunction)
{
	_onPacketFunction = onPacketFunction;
	_onPacketFunctionWithSender = nullptr;
}

void setPacketHandler(PacketHandlerFunctionWithSender onPacketFunctionWithSender)
{
	_onPacketFunction = nullptr;
	_onPacketFunctionWithSender = onPacketFunctionWithSender;
}

private:

PacketSerial_(const PacketSerial_&);
PacketSerial_& operator = (const PacketSerial_&);

uint8_t _receiveBuffer[MAX_PACKET_LENGTH] = {0};
size_t _receiveBufferIndex = 0;

Stream* _stream = nullptr;

PacketHandlerFunction _onPacketFunction = nullptr;
PacketHandlerFunctionWithSender _onPacketFunctionWithSender = nullptr;

/* Timing variables for discarding incomplete packets */
long int time_LastByteReceived, time_Current;
long int byteless_interval;
bool OK_toGetCurrTime = false;
};


/// \brief A typedef for the default COBS PacketSerial class.
typedef PacketSerial_ PacketSerial;
