/*
 * usb_bootloader.h
 *
 *  Created on: Jun 20, 2023
 *      Author: dennis
 */

#ifndef INC_USB_BOOTLOADER_H_
#define INC_USB_BOOTLOADER_H_

#include <stdio.h>
#include "usart.h"
#include "usb_host.h"
#include "fatfs.h"

#include "hexFileParser.h"
#include "flash_manager.h"

#define BOOT_FILE_NAME  "zephyr.hex"
#define LOG_FILE_NAME	"error_log.txt"

extern void boot_from_USBFlash();


#endif /* INC_USB_BOOTLOADER_H_ */
