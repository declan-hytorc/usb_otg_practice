/*
 * usb_otg_logging.c
 *
 *  Created on: Jun 19, 2023
 *      Author: dennis
 */

#include "hexFileParser.h"

static uint8_t Char_to_Hex(uint8_t character);
static uint8_t String_to_Byte(uint8_t upper, uint8_t lower);
static void flushHexRow(HexFileRow_t* row);

extern int parseHexLine(HexFileRow_t* row, uint8_t* readBuf, uint8_t numBytes)
{
	int err = HEX_PARSE_SUCCESS;

	if(readBuf[0] != ':')
	{
		err = INVALID_START_CHARACTER;
	}

	flushHexRow(row);
	// Get Record Length
	row->length = String_to_Byte(readBuf[1], readBuf[2]);

	// Get Record Address
	row->lowAddress = (String_to_Byte(readBuf[3], readBuf[4]) << 8) |
					   String_to_Byte(readBuf[5], readBuf[6]);

	// Get Record Type
	row->recordType = String_to_Byte(readBuf[7], readBuf[8]);
	if(row->recordType > 5)
	{
		err = INVALID_RECORD_TYPE;
	}

	// Store Data
	uint8_t dataStart = 9;
	for(int i = 0; i < row->length; i++)
	{
		row->data[i] = String_to_Byte(readBuf[dataStart], readBuf[dataStart + 1]);
		dataStart = dataStart+2;
	}

	// Get Checksum
	row->checksum = String_to_Byte(readBuf[dataStart], readBuf[dataStart + 1]);

	// Calculate Checksum Based on Data in Structure
	row->calc_checksum = row->length + (uint8_t)(row->lowAddress & 0x00FF) + (uint8_t)((row->lowAddress & 0xFF00) >> 8) + (uint8_t)row->recordType;
	for(int i = 0; i < row->length; i++)
	{
		row->calc_checksum += row->data[i];
	}

	row->calc_checksum = 1 + ~(row->calc_checksum);

	if(row->calc_checksum != row->checksum)
	{
		err = CHECKSUM_MISMATCH;
	}

	return err;
}

static void flushHexRow(HexFileRow_t* row)
{
	row->length = 0;
	row->lowAddress = 0;
	row->recordType = 0;
	row->checksum = 0;
	row->calc_checksum = 0;
	for(int i = 0; i<16; i++){ row->data[i] = 0; }
}

static uint8_t String_to_Byte(uint8_t upper, uint8_t lower)
{
	upper = Char_to_Hex(upper) << 4;
	lower = Char_to_Hex(lower);

	return (upper | lower);
}

static uint8_t Char_to_Hex(uint8_t character)
{
	uint8_t ret;
	switch(character)
	{
	case '0':
		ret = 0x00;
		break;
	case '1':
		ret = 0x01;
		break;
	case '2':
		ret = 0x02;
		break;
	case '3':
		ret = 0x03;
		break;
	case '4':
		ret = 0x04;
		break;
	case '5':
		ret = 0x05;
		break;
	case '6':
		ret = 0x06;
		break;
	case '7':
		ret = 0x07;
		break;
	case '8':
		ret = 0x08;
		break;
	case '9':
		ret = 0x09;
		break;
	case 'A':
		ret = 0x0A;
		break;
	case 'B':
		ret = 0x0B;
		break;
	case 'C':
		ret = 0x0C;
		break;
	case 'D':
		ret = 0x0D;
		break;
	case 'E':
		ret = 0x0E;
		break;
	case 'F':
		ret = 0x0F;
		break;
	default:
		//ERROR
		ret = 0xFF;
	}
	return ret;
}
