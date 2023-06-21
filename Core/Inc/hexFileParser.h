/*
 * usb_otg_logging.h
 *
 *  Created on: Jun 19, 2023
 *      Author: dennis
 */

#ifndef INC_HEXFILEPARSER_H_
#define INC_HEXFILEPARSER_H_

//#include "fatfs.h"
//#include "usb_host.h"
#include <stdint.h>
#include <stdio.h>

#define HEX_PARSE_SUCCESS		0
#define CHECKSUM_MISMATCH		1
#define INVALID_RECORD_TYPE		2
#define INVALID_START_CHARACTER	3

typedef enum{
	DataRecord 				= 0x00,
	EndOfFile 				= 0x01,
	ExtendedSegmentAddress 	= 0x02,
	ExtendedLinearAddress 	= 0x04,
	StartLinearAddress 		= 0x05
}HexRecord_e;

typedef struct{
	uint8_t  	length;
	uint16_t 	lowAddress;
	HexRecord_e recordType;
	uint8_t 	data[0x10];
	uint8_t  	checksum;
	uint8_t  	calc_checksum;
	uint16_t 	upperAddress;
	uint16_t	segmentAddress;
}HexFileRow_t;

extern int parseHexLine(HexFileRow_t* row, uint8_t* readBuf, uint8_t numBytes);


#endif /* INC_HEXFILEPARSER_H_ */
