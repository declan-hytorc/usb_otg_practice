/*
 * flash_manager.h
 *
 *  Created on: Jun 20, 2023
 *      Author: dennis
 */

#ifndef INC_FLASH_MANAGER_H_
#define INC_FLASH_MANAGER_H_

#include "stm32f411xe.h"
#include "stm32f4xx.h"


#define KEY1   				0x45670123
#define KEY2   				0xCDEF89AB


extern void writeProgramFlash(uint32_t flashAddress, uint8_t* data, uint8_t dataLen);
extern void initFlashMemoryManager(void);
extern void exitFlashMemoryManager(void);

#endif /* INC_FLASH_MANAGER_H_ */
