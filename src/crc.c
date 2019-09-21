/**
  ******************************************************************************
  * File Name          : CRC.c
  * Description        : This file provides code for the configuration
  *                      of the CRC instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "crc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

CRC_HandleTypeDef hcrc;

/* CRC init function */
void MX_CRC_Init(void)
{

  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_CRC_MspInit(CRC_HandleTypeDef* crcHandle)
{

  if(crcHandle->Instance==CRC)
  {
  /* USER CODE BEGIN CRC_MspInit 0 */

  /* USER CODE END CRC_MspInit 0 */
    /* CRC clock enable */
    __HAL_RCC_CRC_CLK_ENABLE();
  /* USER CODE BEGIN CRC_MspInit 1 */

  /* USER CODE END CRC_MspInit 1 */
  }
}

void HAL_CRC_MspDeInit(CRC_HandleTypeDef* crcHandle)
{

  if(crcHandle->Instance==CRC)
  {
  /* USER CODE BEGIN CRC_MspDeInit 0 */

  /* USER CODE END CRC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CRC_CLK_DISABLE();
  /* USER CODE BEGIN CRC_MspDeInit 1 */

  /* USER CODE END CRC_MspDeInit 1 */
  }
} 


/* USER CODE BEGIN 1 */
#ifdef SOFT_CRC

uint32_t crcTable [] = { // Nibble lookup table for 0x04C11DB7 polynomial
		0x00000000,0x04C11DB7,0x09823B6E,0x0D4326D9,0x130476DC,0x17C56B6B,0x1A864DB2,0x1E475005,
		0x2608EDB8,0x22C9F00F,0x2F8AD6D6,0x2B4BCB61,0x350C9B64,0x31CD86D3,0x3C8EA00A,0x384FBDBD } ;

uint32_t addToCrc ( uint32_t crc , uint32_t data ) {
	crc = crc ^ data ;
	for ( int i = 0 ; i < 8 ; i ++ ) {
		crc = ( crc << 4 ) ^ crcTable [ ( crc >> 28 ) & 0x0F ] ;
	}
	return crc ;
}
uint32_t computeCRC32 ( uint32_t* dataWard, uint32_t length ) {
	uint32_t crc = 0xFFFFFFFF ;
	uint8_t *data = (uint8_t*) dataWard ;
	if ( length % 4 != 0 ) {
		printf ("Bad CRC update size: %d " , (int)length ) ;
		return -1 ;
	}
	for ( uint32_t i = 0 ; i < length ; i += 4 ) {
//		uint32_t word =
//				( ( data [ i + 0 ] << 24 ) & 0xff000000 ) |
//				( ( data [ i + 1 ] << 16 ) & 0x00ff0000 ) |
//				( ( data [ i + 2 ] <<  8 ) & 0x0000ff00 ) |
//				( ( data [ i + 3 ] <<  0 ) & 0x000000ff ) ;
		uint32_t word = __REV( (uint32_t) data ) ;
		crc = addToCrc ( crc , word ) ;
	}
	return crc ;
}
#else
uint32_t computeCRC32 ( uint32_t* data, uint32_t count ) {
	return HAL_CRC_Calculate ( & hcrc , data , count ) ;
}
#endif
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
