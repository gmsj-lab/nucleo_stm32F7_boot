/*
 * downloadApplication.h
 *
 *  Created on: 8 sept. 2019
 *      Author: gmsj
 */

#ifndef REPROGRAMMING_DOWNLOAD_APPLICATION_H_
#define REPROGRAMMING_DOWNLOAD_APPLICATION_H_

#include "io_redirection.h"
#include <main.h>
#include <flash_if.h>

#ifdef __cplusplus
 extern "C" {
#endif

#define APPLICATION_SIZE_MAX	(FLASH_USER_END_ADDR-FLASH_USER_START_ADDR)
#define DOWNLOAD_BUFFER_SIZE	  1400
#define DOWNLOAD_HEADER_SIZE		 4

typedef struct _ReprogHeader {
	uint32_t codeLength ;
	uint32_t codeChecksum ;
} ReprogHeader ;

boolean	otar 							( void ) ;
boolean checkApplication 				( void ) ;
void 	usart_getHeaderMsg 				( void ) ;
void 	usart_sendDownloadAck 			( void ) ;
boolean downloadApplication 			( void ) ;
boolean checkApplication 				( void ) ;
boolean flashApplication 				( uint32_t length ) ;

#ifdef __cplusplus
}
#endif

#endif /* REPROGRAMMING_DOWNLOAD_APPLICATION_H_ */
