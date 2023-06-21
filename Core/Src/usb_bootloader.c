/*
 * usb_bootloader.c
 *
 *  Created on: Jun 20, 2023
 *      Author: dennis
 */


#include "usb_bootloader.h"

FIL HexFile, LogFile;                   /* File object */

static int processHexLine(HexFileRow_t* hexRow);
static void flush_buffer(uint8_t* buffer, uint16_t size);

extern void boot_from_USBFlash()
{
	uint32_t byteswritten;                     /* File write/read counts */
	uint8_t hexRawLine[46];                                   /* File read buffer */
	HexFileRow_t hexRow;
	hexRow.upperAddress = 0;
	hexRow.segmentAddress = 0;
	int hexLineCounter = 0;
	char log_str[200];
	uint8_t booting = 1; // Used as boolean
	int err;

	// Attempt to open files
	f_open(&LogFile, "error_log.txt", FA_CREATE_ALWAYS | FA_WRITE);
	if(f_open(&HexFile, "blinky.hex", FA_READ) != FR_OK)
	{
		HAL_UART_Transmit(&huart2, (uint8_t*)"Open Read Failed\r\n", 18, 100);
		return;
	}
	// Start with upper address of 0 until changed
	hexRow.upperAddress = 0;

	initFlashMemoryManager();
	while(booting)
	{
		hexLineCounter++;
		if(hexLineCounter > 9150) break;

		flush_buffer(hexRawLine, sizeof(hexRawLine));

		// Get next line from hex file
		f_gets((char *)hexRawLine, sizeof(hexRawLine), &HexFile);

	  	//Process hex line
	  	err = parseHexLine(&hexRow, hexRawLine, sizeof(hexRawLine));

	  	// Decide What to do based on Record Type:
	  	booting = processHexLine(&hexRow);

	  	if(err)
	  	{
	  		flush_buffer((uint8_t*)log_str, sizeof(log_str));
	  		sprintf(log_str, "\ERROR CODE: %d\nExiting on Hex Line: %d\nLength: %x\nLower Addr: %x\nUpper Addr: %x\nType: %x\nData: %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\nChecksum: %x\nVerify Checksum: %x\n",
	  		err, hexLineCounter, hexRow.length, hexRow.lowAddress, hexRow.upperAddress, hexRow.recordType,
						hexRow.data[0], hexRow.data[1], hexRow.data[2], hexRow.data[3],
						hexRow.data[4], hexRow.data[5], hexRow.data[6], hexRow.data[7],
						hexRow.data[8], hexRow.data[9], hexRow.data[10], hexRow.data[11],
						hexRow.data[12], hexRow.data[13], hexRow.data[14],
						hexRow.data[15], hexRow.checksum, hexRow.calc_checksum);

	  	  	// Log Checksum Error
	  	  	byteswritten = sizeof(log_str);
	  	  	f_write(&LogFile, log_str, byteswritten, (void *)&byteswritten);
	  	  	break;
	  	}


	}
	exitFlashMemoryManager();

	HAL_UART_Transmit(&huart2, (uint8_t*)"Boot Terminated.\r\n", 18, 100);
	f_close(&LogFile);
	f_close(&HexFile);
}

static int processHexLine(HexFileRow_t* hexRow)
{
	uint16_t addrBuf;
	uint32_t fullAddress;

  	switch(hexRow->recordType)
  	{
  	case DataRecord:
  		// write data to flash
  		fullAddress = (((uint32_t)hexRow->upperAddress << 16) | (uint32_t)(hexRow->lowAddress)) + ((uint32_t)hexRow->segmentAddress << 4);
  		writeProgramFlash(fullAddress, hexRow->data, hexRow->length);
  		break;
  	case EndOfFile:
  		// end boot
  		return 0;
  		break;
  	case ExtendedSegmentAddress:
  		// Edit memory addresses
  		hexRow->segmentAddress = ((uint16_t)hexRow->data[0] << 8) | (uint16_t)hexRow->data[1];
  		break;
  	case ExtendedLinearAddress:
  		addrBuf = ((uint16_t)hexRow->data[0] << 8) | (uint16_t)hexRow->data[1];
  		hexRow->upperAddress = addrBuf;
  		break;
  	case StartLinearAddress:
  		hexRow->upperAddress = ((uint16_t)hexRow->data[0] << 8) | (uint16_t)hexRow->data[1];
  		hexRow->lowAddress = ((uint16_t)hexRow->data[2] << 8) | (uint16_t)hexRow->data[3];
  		break;
  	default:
  		break;
  	}
  	return 1;

}

static void flush_buffer(uint8_t* buffer, uint16_t size)
{
	for(int i = 0; i < size; i++)
	{
		buffer[i] = '\0';
	}
}


