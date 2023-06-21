/*
 * flash_manager.c
 *
 *  Created on: Jun 20, 2023
 *      Author: dennis
 */

#include "flash_manager.h"

/******* Private Function Declarations *******/
static void flashWriteByte(uint32_t flashAddress, uint8_t hexByte);
static void eraseFlashMemorySector(uint32_t sector);
static void lockFlashAccess(void);
static void unlockFlashAccess(void);
static void waitForLastOperation(void);

/************ Public Functions ************/
extern void initFlashMemoryManager(void)
{
	unlockFlashAccess();

	// Currently, the bootloader is in sectors 0 through 3.
	// We will allocate everything else for the application.
	// See reference manual page 44
	eraseFlashMemorySector(4);
	eraseFlashMemorySector(5);
	eraseFlashMemorySector(6);
	eraseFlashMemorySector(7);
}
extern void exitFlashMemoryManager(void)
{
	// Currently only locks memory when we are done writing in application.
	// Maybe a check could be added here.
	lockFlashAccess();
}

extern void writeProgramFlash(uint32_t flashAddress, uint8_t* hexData, uint8_t dataLen)
{
	// Disable interrupts
	__disable_irq();
	// Iterate through array and write to flash
	for(int i = 0; i < dataLen; i++)
	{
		flashWriteByte(flashAddress, hexData[i]);
		flashAddress++;
	}
	// Re-enable interrupts
	__enable_irq();
}

static void flashWriteByte(uint32_t flashAddress, uint8_t hexByte)
{

	waitForLastOperation();

	// If the previous operation is completed, proceed to program the new data
	FLASH->CR &= CR_PSIZE_MASK;
	FLASH->CR |= FLASH_PSIZE_BYTE;
	FLASH->CR |= FLASH_CR_PG;

	*(__IO uint8_t*)flashAddress = hexByte;

	// Data synchronous Barrier (DSB) Just after the write operation
	//     This will force the CPU to respect the sequence of instruction (no optimization).
	__DSB();

}

static void eraseFlashMemorySector(uint32_t sector)
{
	waitForLastOperation();

	unlockFlashAccess();


	/* If the previous operation is completed, proceed to erase the sector */
	FLASH->CR &= CR_PSIZE_MASK;
	FLASH->CR |= FLASH_PSIZE_WORD;


	CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
	CLEAR_BIT(FLASH->CR, FLASH_CR_SNB);

	FLASH->CR |= FLASH_CR_SER | (sector << FLASH_CR_SNB_Pos);
	FLASH->CR |= FLASH_CR_STRT;

	/* Data synchronous Barrier (DSB) Just after the write operation
    	This will force the CPU to respect the sequence of instruction (no optimization).*/
	__DSB();
}

static void lockFlashAccess(void)
{
	if(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 1)
	{
		SET_BIT(FLASH->CR, FLASH_CR_LOCK);
	}
}
static void unlockFlashAccess(void)
{
	if(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0)
	{
		FLASH->KEYR |= KEY1;
		FLASH->KEYR |= KEY2;
	}
}

static void waitForLastOperation(void)
{
	while(READ_BIT(FLASH->SR, FLASH_SR_BSY));
}
