/*
 * downloadApplication.cpp
 *
 *  Created on: 8 sept. 2019
 *      Author: gmsj
 */

#include <stdlib.h>
#include <stdio.h>
#include <crc.h>

#include <usart_driver.h>
#include "boot_link.h"
#include "download_application.h"

// Warning : Shared with appli :
#define SOM							'@'
#define SOFTWARE_REPROG_ACK			'Z'
#define LENGTH_SEPARATOR			'&'
#define EOM							'#'
#define _EOF						'\0'

UsartDriver  usartDriver ;
ReprogHeader header ;
uint8_t * 	 ramBuffer ;

boolean otar ( void ) {
	BSP_LED_Off ( LED_GREEN ) ;
	BSP_LED_On  ( LED_BLUE  ) ;

	if ( downloadApplication () == FALSE ) {
		return FALSE ;
	}
	BSP_LED_On ( LED_GREEN  ) ;
	BSP_LED_On  ( LED_BLUE  ) ;

	if ( flashApplication ( header.codeLength ) == FALSE ) {
		return FALSE ;
	}
	BSP_LED_Off ( LED_GREEN ) ;
	BSP_LED_Off ( LED_BLUE  ) ;

	return TRUE ;
}

// Wait for a specific char and return the string of what was received before that char
void waitForChar ( char * rxString, char nextChar ) {
	char *buf ;
	int length = 0 ;
	boolean found = FALSE ;

	while ( ! found ) {
		while ( usartDriver.getReceiveBuffer ( & buf , 1 ) != 1 ) HAL_Delay ( 1 ) ;
		if ( *buf == nextChar ) {
			found = TRUE ;
		}
		else {
			rxString [ length ++ ] = *buf ;
		}
		usartDriver.receive ( 1 ) ;
	}
	// make a string out of what was received before the expected char
	rxString [ length ] = _EOF ;
}
void usart_getHeaderMsg ( void ) {
	char rxString [ 20 ] ;

	waitForChar ( rxString, SOM 				) ;
	waitForChar ( rxString, SOFTWARE_REPROG_ACK ) ;
	waitForChar ( rxString, LENGTH_SEPARATOR 	) ;	header.codeLength 	= atol ( rxString ) ; 	// Convert the length
	waitForChar ( rxString, EOM 				) ;	header.codeChecksum = atol ( rxString ) ;	// Convert the checksum
	waitForChar ( rxString, 0x0A 				) ;
}

void usart_sendDownloadAck () {
	char * buf ;
	char ackMsg[] 		= { SOM , SOFTWARE_REPROG_ACK , EOM , _EOF } ;
	uint8_t ackMsgSize 	= strlen ( ackMsg ) ;

	while ( usartDriver.getSendBuffer ( &buf , ackMsgSize ) < ackMsgSize ) {
		HAL_Delay ( 1 ) ;
	}
	memcpy ( buf , ackMsg , ackMsgSize ) ;
	usartDriver.send ( ackMsgSize ) ;
}

boolean downloadApplication ( void ) {
	uint32_t length 	  = 0 ;
	uint32_t packetLength = 0 ;
	char *buf ;

	// Init the driver on usart 2
	usartDriver.init ( 2 ) ;

	// Tell the host we are ready for download
	usart_sendDownloadAck () ;

	// Wait for reprogramming message
	usart_getHeaderMsg () ;

	if ( header.codeLength > APPLICATION_SIZE_MAX ) {
		printf ( "Application too big for us !\n" ) ;
		BSP_LED_On ( LED_RED   ) ;
		return FALSE ;
	}

	ramBuffer = (uint8_t *) malloc ( header.codeLength ) ;

	if ( ramBuffer == NULL ) {
		printf ( "Application too big for us !\n" ) ;
		BSP_LED_On ( LED_RED   ) ;
		return FALSE ;
	}

	// Get the code block by block
	while ( length < header.codeLength ) {
		// Get a packet - a pointer on receive fifo buffer
		packetLength = usartDriver.getReceiveBuffer ( & buf , DOWNLOAD_BUFFER_SIZE ) ;

		// Copy the received packet in ram
		if ( packetLength ) {
			memcpy ( & ramBuffer [ length ] , buf , packetLength ) ;
			usartDriver.receive ( packetLength ) ;
			length += packetLength ;
		}
	}
	HAL_Delay( 0 ) ;

	//printf ( "About to check downloaded application\n" ) ;
	return checkApplication () ;
}
boolean checkApplication () {
	uint32_t result = computeCRC32 ( (uint32_t *)ramBuffer , header.codeLength ) ;
	if ( result == header.codeChecksum ) {
		return TRUE ;
	}
	return FALSE ;
}
boolean flashApplication ( uint32_t length ) {

	if ( FLASH_If_Erase ( length ) == FALSE ) {
		return FALSE ;
	}
	if ( FLASH_If_Erase_Verify ( length ) == FALSE ) {
		return FALSE ;
	}
	if ( FLASH_If_Write  ( (uint32_t *)ramBuffer , length / 4 ) == FALSE ) {
		return FALSE ;
	}
	if ( FLASH_If_Verify ( (uint32_t *)ramBuffer , length / 4 ) == FALSE ) {
		return FALSE ;
	}
	return TRUE ;
}

