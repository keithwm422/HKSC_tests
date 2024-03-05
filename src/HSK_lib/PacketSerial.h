/*
 * PacketSerial.h
 *
 * A wrapper class for the Arduino 'Stream' library that enables the use of
 * (in this case) COBS encoding. Hijacked by PUEO for use of HKSC and SFC header
 *
 * Copyright (c) 2011 Christopher Baker <https://christopherbaker.net>
 * Copyright (c) 2011 Jacques Fortier <https://github.com/jacquesf/COBS-Consistent-Overhead-Byte-Stuffing>
 * -Keith added stuff...
 * SPDX-License-Identifier:	MIT
 *
 */

#pragma once
/* Max data length is:
 *	-- +1 Magic Byte
 *	-- +3 header bytes (cmd, length_upper, length_lower])
 *	-- +____ data bytes with length maximum now being 65536 its probably limited by uC stuff
 *  -- +1 Sequence number byte
 */
#define MAX_PACKET_LENGTH 2000  // start with it large and see what happens...
#define BYTELESS_TIME_LIMIT 10000
#define MAGIC_BYTE_VALUE 252
#include <Arduino.h>
#include "Core_protocol.h" // for all the hsk stuff to find the length to know how much data to copy into the buffer to find the seq number to say if packet is alright....

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
		/* Read in one byte */
		uint8_t data = _stream->read();

		/* Evaluate time stamps */
		if (checkForBadPacket()) return EBADLEN;

		/* If we have reached the data length +1 (+1 for sequence number) then the message is complete */
		if (_receiveBufferIndex==_totalsize-1)
		{
			/* Stop the clock */
			OK_toGetCurrTime = false;

			if (_onPacketFunction || _onPacketFunctionWithSender)
			{
				/*size_t numDecoded = COBS::decode(_receiveBuffer,
				                                 _receiveBufferIndex,
				                                 _decodeBuffer);
				// If one zero byte was found, discard it
				if (numDecoded == 0){
                                  _receiveBufferIndex=0;
                                  return 0;
                                }*/
				// Execute whichever function was defined (w/ or w/o sender)
				if (_onPacketFunction)
				{					
					size_t num_bytes_rcvd=_receiveBufferIndex;
					_onPacketFunction(_receiveBuffer, num_bytes_rcvd);
				}
				else if (_onPacketFunctionWithSender)
				{
					size_t num_bytes_rcvd=_receiveBufferIndex;
					_onPacketFunctionWithSender(this, _receiveBuffer, num_bytes_rcvd);
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

			if ((_receiveBufferIndex + 1) < MAX_PACKET_LENGTH){ // there is space to put the byte we just read
				_receiveBuffer[_receiveBufferIndex++] = data;
				if(_receiveBufferIndex==0){
					if(data!=_magicbyte) return EBADPARSE;
					else received_magicByte=true;
				}
				if(_receiveBufferIndex==3 && received_magicByte){ // we have read the magic byte, the cmd byte, and now both length bytes successfully...
			        hdr_rcvd = (housekeeping_hdr_t *) _receiveBuffer;
					_totalsize=hdr_rcvd->len + 1;
					received_magicByte=false;
				}
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
	_stream->write(buffer, size);
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
uint8_t _seqnumber=0; // starts at 0 but increments and rolls over
uint8_t _magicbyte=MAGIC_BYTE_VALUE; // let's use a constant
int _totalsize;
Stream* _stream = nullptr;

PacketHandlerFunction _onPacketFunction = nullptr;
PacketHandlerFunctionWithSender _onPacketFunctionWithSender = nullptr;

/* Timing variables for discarding incomplete packets */
long int time_LastByteReceived, time_Current;
long int byteless_interval;
bool OK_toGetCurrTime = false;
bool received_magicByte=false;
housekeeping_hdr_t * hdr_rcvd{nullptr};

};


/// \brief A typedef for the default COBS PacketSerial class.
typedef PacketSerial_ PacketSerial;
