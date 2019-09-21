/*
 * boot_link.h
 *
 *  Created on: 8 sept. 2019
 *      Author: gmsj
 */

#ifndef BOOT_BSP_BOOT_LINK_H_
#define BOOT_BSP_BOOT_LINK_H_

#include "stm32f7xx_hal.h"
#include "core_cm7.h"

#ifdef __cplusplus
 extern "C" {
#endif


#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08040000) /* Base address of Sector 5, 256 Kbytes */

#define BOOT_ADDRESS       		 	0x08000000
#define APPLICATION_ADDRESS        	ADDR_FLASH_SECTOR_5
#define DOWNLOAD_REQUEST_ADDRESS	0x2000FFF0

typedef enum {FALSE, TRUE} boolean;
typedef void ( * pFunction ) ( void ) ;

boolean applicationDownloadRequested ( void ) ;
void jumpToApplication 				 ( void ) ;
void setupSoftwareReprogramming		 ( void ) ;
void performSoftwareReboot 			 ( void ) ;

#ifdef __cplusplus
}
#endif

#endif /* BOOT_BSP_BOOT_LINK_H_ */
