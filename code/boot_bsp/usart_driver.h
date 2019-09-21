/*
 * usart_driver.h
 *
 *  Created on: 8 sept. 2019
 *      Author: gilles
 */

#ifndef BOOT_BSP_USART_DRIVER_H_
#define BOOT_BSP_USART_DRIVER_H_

#include <main.h>
#include <string.h>
#include <stm32f7xx_nucleo_144.h>
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"
#include "utils.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define USE_TX_DMA

//#define DISABLE_TX_IT   HAL_NVIC_DisableIRQ  (USART2_IRQn);HAL_NVIC_DisableIRQ  (DMA1_Stream6_IRQn)
//#define ENABLE_TX_IT    HAL_NVIC_EnableIRQ   (USART2_IRQn);HAL_NVIC_EnableIRQ   (DMA1_Stream6_IRQn)

#define DISABLE_TX_IT   HAL_NVIC_DisableIRQ  (usart_irq);HAL_NVIC_DisableIRQ  (dma_irq)
#define ENABLE_TX_IT    HAL_NVIC_EnableIRQ   (usart_irq);HAL_NVIC_EnableIRQ   (dma_irq)
#define TX_BUFFER_SIZE	4000
#define TX_BUFFER_LAST	(TX_BUFFER_SIZE-1)
#define RX_BUFFER_SIZE	2000

class TxBuffer {
	public:
					uint16_t  in ;
		volatile	uint16_t  out ;
					uint16_t  end ;
		volatile	uint16_t  currentLength ;
		uint8_t   	data [ TX_BUFFER_SIZE ] ;
} ;

class RxBuffer {
	public:
		uint16_t in ;
		uint16_t out ;
		uint8_t  data 		[ RX_BUFFER_SIZE ] ;
} ;

class UsartDriver {

	private:
		UART_HandleTypeDef *uart ;
		TxBuffer 			tx ;
		RxBuffer 			rx ;

		IRQn_Type 			usart_irq ;
		IRQn_Type 			dma_irq ;

		void launchDma ( void ) ;

	public:
		UsartDriver () {
			uart 		 =  0 ;
			usart_number = -1 ;
			usart_irq 	 = USART2_IRQn ;
			dma_irq 	 = DMA1_Stream6_IRQn ;
		}
		virtual ~UsartDriver () {}

		static UsartDriver *usartDrivers [ 3 ] ;
		int  usart_number ;
		int  init 				( long usart ) ;
	// TX
		int  availableForSend 	( void ) ;
		void send       		( char * data , int length )	{}				// TODO
	// TX ZERO COPY
		int  getSendBuffer 		( char ** buf , int minRequestedSize ) ;
		void send 				( int length ) ;
		void txCallback			( void ) ;
	// RX
		void flushRx			( void ) ;
		int  available			( void ) ;
		int  receive 		 	( char * data , int length ) ;
		// RX OPTIMIZED
		int  getReceiveBuffer 	( char ** data , int length ) ;
		void receive 			( int length ) ;
} ;


#ifdef __cplusplus
}
#endif

#endif /* BOOT_BSP_USART_DRIVER_H_ */
