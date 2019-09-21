/*
 * usart_driver.cpp
 *
 *  Created on: 8 sept. 2019
 *      Author: gmsj
 */
#include "usart_driver.h"

extern UART_HandleTypeDef huart2 ;
extern UART_HandleTypeDef huart3 ;
extern UART_HandleTypeDef huart6 ;

 // mapping : usart2=1, usart3=2, usart6=0
#define USART_DRIVER_INDEX_FROM_NUMBER(num)		( ((num)-1) % 5 )
#define USART_DRIVER_HANDLE_FROM_NUMBER(num)	( ((num) == 2 ) ? &huart2 : ((num) == 3 ) ? &huart3 : &huart6 )
#define USART_DRIVER_INDEX_FROM_HANDLE(handle)	( ((handle)->Instance == huart2.Instance) ? 1 : ((handle)->Instance == huart3.Instance)  ? 2 : 0 )
#define USART_IRQ_FROM_NUMBER(num)				( ((num) == 2 ) ? USART2_IRQn : ((num) == 3 ) ? USART3_IRQn : USART6_IRQn )
#define DMA_IRQ_FROM_NUMBER(num)				( ((num) == 2 ) ? DMA1_Stream6_IRQn : ((num) == 3 ) ? DMA1_Stream3_IRQn : DMA2_Stream6_IRQn )


UsartDriver *UsartDriver::usartDrivers [ 3 ] = { 0,0,0 } ;

int UsartDriver::init ( long usart ) {

	tx.in				= 0 ;
	tx.out				= 0 ;
	tx.end				= TX_BUFFER_LAST ;
	tx.currentLength 	= 0 ;

	rx.in				= 0 ;
	rx.out				= 0 ;

	uart = USART_DRIVER_HANDLE_FROM_NUMBER(usart) ;
	usartDrivers [ USART_DRIVER_INDEX_FROM_HANDLE(uart) ] = this ;
	usart_number = usart ;

	usart_irq = USART_IRQ_FROM_NUMBER(usart) ;
	dma_irq   = DMA_IRQ_FROM_NUMBER(usart) ;

	return ( HAL_UART_Receive_DMA ( uart , rx.data , sizeof (rx.data) ) == HAL_OK ) ? 1 : 0 ;
}

//---------------------------------- TX -----------------------------------------------------------------
int UsartDriver::availableForSend ( void ) {
	int availableSize ;

	DISABLE_TX_IT ;

	uint16_t out = tx.out + tx.currentLength - __HAL_DMA_GET_COUNTER ( uart->hdmatx ) ;

	// -1 : "in" pointer should not reach "out" pointer (in==out means empty!)
	if ( ! out ) out -= 1 ;

	if ( tx.in < out ) {
		// available space is from in to current out
		availableSize = out - tx.in ;
	}
	else if ( (TX_BUFFER_LAST - tx.in ) > out ) {
		// available space is from in to last
		availableSize = TX_BUFFER_LAST - tx.in ;
	}
	else  {
		// available space from top to out is bigger (will need a roll over)
		availableSize = out ;
	}
	ENABLE_TX_IT ;

	return availableSize ;
}
int UsartDriver::getSendBuffer ( char ** buf , int minRequestedSize ) {
	int availableSize = 0 ;

	while ( availableSize < minRequestedSize ) {

		DISABLE_TX_IT ;

		uint16_t out = tx.out + tx.currentLength - __HAL_DMA_GET_COUNTER ( uart->hdmatx ) ;

		if ( tx.in < out ) {
			// free room from "in" to "out"
			availableSize = out - tx.in - 1 ;
		}
		else {
			// available space is from in to last
			availableSize = TX_BUFFER_LAST - tx.in ;
			if ( availableSize < minRequestedSize ) {
				// not enough free space at end of buffer, roll over if possible (do not catch outOffset!)
				// and show that transmission ends at roll over point
				if ( out != 0 ) {
					tx.end = tx.in ;
					tx.in = 0 ;
					availableSize = out - 1 ;
				}
			}
		}
		*buf = (char *) tx.data + tx.in ;

		ENABLE_TX_IT ;

		if ( availableSize < minRequestedSize ) {
			// wait to have some more room in tx buffer
			delay_us ( 1 ) ;
		}
	}
	return availableSize ;
}
inline void UsartDriver::launchDma ( void ) {

	// roll over "out" if it reach the end of used part of buffer
	if ( tx.out == tx.end ) {
		tx.out 	= 0 ;
		tx.end  = TX_BUFFER_LAST ;	// reset to last byte of buffer in case last tx stopped before
	}
	tx.currentLength = ( (tx.in >= tx.out) ? tx.in - tx.out : tx.end - tx.out ) ;
	// More work ?
	if ( tx.currentLength != 0 ) {
		if ( HAL_UART_Transmit_DMA ( uart , & tx.data [ tx.out ] , tx.currentLength ) != HAL_OK ) {
			for(;;); //ErrorHandler () ; //GM001
		}
	}
}
void UsartDriver::send ( int length ) {

	DISABLE_TX_IT ;

	tx.in += length ;

	// need to reload DMA only if stopped
	if ( tx.currentLength == 0 ) {
		launchDma () ;
	}
	ENABLE_TX_IT ;
}
void UsartDriver::txCallback ( void ) {
	// bizarre...le DMA s'arrete de temps en temps avant la fin (NTDF>0), sans erreur...on relance pour qu'il finisse
	uint16_t NTDR = __HAL_DMA_GET_COUNTER ( uart->hdmatx ) ;
	tx.out += tx.currentLength - NTDR ;
	if ( NTDR ) {
		// used to have a counter of this kind of strange phenomenon...
	}
	launchDma () ;
}

//---------------------------------- RX -----------------------------------------------------------------
int  UsartDriver::available	( void ) {
	char *data ;
	return getReceiveBuffer ( &data , RX_BUFFER_SIZE )  ;
}
int UsartDriver::receive ( char * data , int length )  {
	int bufLength ;
	// update the in offset to take into account what's have been received by dma
	rx.in = RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER ( uart->hdmarx ) ;

	// Roll-over ? performed only when in has moved (out should always stay behind in)
	if ( ( rx.out == RX_BUFFER_SIZE ) && ( rx.out != rx.in ) ) {
		rx.out = 0 ;
	}
	if ( rx.out <= rx.in ) {
		bufLength = MIN( length , rx.in - rx.out ) ;
	}
	else {
		// Give only what's from out pointer to end of buffer
		bufLength = MIN( length , RX_BUFFER_SIZE - rx.out ) ;
	}
	memcpy ( data , (char *) rx.data + rx.out , bufLength ) ;
	rx.out += bufLength ;
	return bufLength ;
}
int UsartDriver::getReceiveBuffer ( char ** data , int length ) {
	int bufLength ;
	// update the in offset to take into account what's have been received by dma
	rx.in = RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER ( uart->hdmarx ) ;

	// Roll-over ? performed only when in has moved (out should always stay behind in)
	if ( ( rx.out == RX_BUFFER_SIZE ) && ( rx.out != rx.in ) ) {
		rx.out = 0 ;
	}

	if ( rx.out <= rx.in ) {
		*data = (char *) rx.data + rx.out ;
		bufLength = rx.in - rx.out ;
	}
	else {
		// Give only what's from out pointer to end of buffer
		*data = (char *) rx.data + rx.out ;
		bufLength = RX_BUFFER_SIZE - rx.out ;
	}
	// only give to user the requested length if it is less than buffer length
	return ( bufLength < length ) ? bufLength : length ;
}
void UsartDriver::receive ( int length ) {
	rx.out += length ;
}
void UsartDriver::flushRx ( void ) {
	__HAL_UART_FLUSH_DRREGISTER( uart ) ;
}
void HAL_UART_TxCpltCallback	( UART_HandleTypeDef *huart ) {
	UsartDriver *driver = UsartDriver::usartDrivers [ USART_DRIVER_INDEX_FROM_HANDLE(huart) ] ;
	if ( driver->usart_number != -1 ) {
		driver->txCallback () ;
	}
}
void HAL_UART_RxCpltCallback	( UART_HandleTypeDef *huart ) {
}
void HAL_UART_ErrorCallback		( UART_HandleTypeDef *huart ) {
	BSP_LED_On(LED_RED);
	for(;;); //ErrorHandler () ;  // GM001
}


