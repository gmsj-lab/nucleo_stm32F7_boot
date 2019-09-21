/*
 * boot_link.cpp
 *
 *  Created on: 8 sept. 2019
 *      Author: gmsj
 */
#include "boot_link.h"

// Called from boot main to detect a download request
boolean applicationDownloadRequested () {
	volatile uint32_t *downloadRequestAddress = (uint32_t *) DOWNLOAD_REQUEST_ADDRESS ;
	if ( *downloadRequestAddress == 0xDEADCAFE ) {
		*downloadRequestAddress = 0 ;
		return TRUE ;
	}
	return FALSE ;
}

// Called from boot main to jump to application
void jumpToApplication () {

	// No request to download, jump to application
	pFunction application_start ;
	uint32_t application_SP ;

	/* Get the application stack pointer (First entry in the application vector table) */
	application_SP = (uint32_t) *((volatile uint32_t*)APPLICATION_ADDRESS) ;

	/* Get the application entry point (Second entry in the application vector table) */
	application_start = (pFunction) *(volatile uint32_t*) (APPLICATION_ADDRESS + 4) ;

	/* Reconfigure vector table offset register to match the application location */
	SCB->VTOR = APPLICATION_ADDRESS ;

	/* Set the application stack pointer */
	__set_MSP ( application_SP ) ;

	/* Start the application */
	application_start () ;

	while ( 1 ) ;
}

// Called from Appli to jump to boot with a request for reprogramming
void setupSoftwareReprogramming () {
	// MAGIC WORD address is shared between boot and appli
	volatile uint32_t *downloadRequestAddress = (uint32_t *) DOWNLOAD_REQUEST_ADDRESS ;
	*downloadRequestAddress = 0xDEADCAFE ;

	/* Get the boot stack pointer (First entry in the boot vector table) */
	uint32_t boot_SP = (uint32_t) *((__IO uint32_t*)BOOT_ADDRESS) ;

	/* Reconfigure vector table offset register to match the boot location */
	SCB->VTOR = BOOT_ADDRESS ;

	/* Set the boot stack pointer */
	__set_MSP ( boot_SP ) ;

	/* Reset to start the boot */
	NVIC_SystemReset () ;
}

// Called from Appli to perform a sysreset
void performSoftwareReboot ( void ) {
	/* Get the boot stack pointer (First entry in the boot vector table) */
	uint32_t boot_SP = (uint32_t) *((__IO uint32_t*)BOOT_ADDRESS) ;

	/* Reconfigure vector table offset register to match the boot location */
	SCB->VTOR = BOOT_ADDRESS ;

	/* Set the boot stack pointer */
	__set_MSP ( boot_SP ) ;

	/* Reset to start the boot */
	NVIC_SystemReset () ;
}


