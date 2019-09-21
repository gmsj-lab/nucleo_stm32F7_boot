/*
 * io_redirection.cpp
 *
 *  Created on: 8 sept. 2019
 *      Author: gmsj
 */
#include <io_redirection.h>
#include <usart_driver.h>


extern UART_HandleTypeDef huart2 ;

#if 1
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART and Loop until the end of transmission */
	 HAL_UART_Transmit ( &huart2 , (uint8_t *) &ch, 1, 0xFFFF ) ;

  return ch ;
}
#else
PUTCHAR_PROTOTYPE
{
	char * buf ;

	while ( UsartDriver::usartDrivers [ USART_DRIVER_INDEX_FROM_NUMBER ( 2 ) ] ->getSendBuffer ( &buf , 1 ) < 1 ) {
		HAL_Delay ( 1 ) ;
	}

	* buf =  ch ;

	UsartDriver::usartDrivers [ USART_DRIVER_INDEX_FROM_NUMBER ( 2 ) ] ->send ( 1 ) ;

	return ch ;
}
#endif

