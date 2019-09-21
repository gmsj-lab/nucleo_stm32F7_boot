
#include <flash_if.h>

/* Private define ------------------------------------------------------------*/
/* Base address of the Flash sectors */

/* Private variables ---------------------------------------------------------*/
uint32_t FirstSector = 0, NbOfSectors = 0;
uint32_t Address = 0, SECTORError = 0;
__IO uint32_t data32 = 0 , MemoryProgramStatus = 0;

/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;

uint32_t GetSector ( uint32_t Address )
{
	uint32_t sector = 0;

	if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
	{
		sector = FLASH_SECTOR_0;
	}
	else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
	{
		sector = FLASH_SECTOR_1;
	}
	else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
	{
		sector = FLASH_SECTOR_2;
	}
	else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
	{
		sector = FLASH_SECTOR_3;
	}
	else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
	{
		sector = FLASH_SECTOR_4;
	}
	else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
	{
		sector = FLASH_SECTOR_5;
	}
	else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
	{
		sector = FLASH_SECTOR_6;
	}
	else /* (Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_7) */
	{
		sector = FLASH_SECTOR_7;
	}
	return sector;
}
boolean FLASH_If_Erase ( uint32_t length ) {
	uint32_t EndAddress = FLASH_USER_START_ADDR + length ;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock () ;

	/* Get the 1st sector to erase */
	FirstSector = GetSector(FLASH_USER_START_ADDR);
	/* Get the number of sector to erase from 1st sector*/
	NbOfSectors = GetSector(EndAddress) - FirstSector + 1;
	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector        = FirstSector;
	EraseInitStruct.NbSectors     = NbOfSectors;

	/* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
	     you have to make sure that these data are rewritten before they are accessed during code
	     execution. If this cannot be done safely, it is recommended to flush the caches by setting the
	     DCRST and ICRST bits in the FLASH_CR register. */
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
	{
		HAL_FLASH_Lock () ;
		return FALSE ;
	}
	HAL_FLASH_Lock () ;
	HAL_Delay( 0 ) ;
	return TRUE ;
}
boolean FLASH_If_Erase_Verify ( uint32_t length ) {
	uint32_t EndAddress = FLASH_USER_START_ADDR + length ;
	for ( uint32_t *flashAdress = (uint32_t *)FLASH_USER_START_ADDR ; flashAdress < (uint32_t *)EndAddress ; flashAdress ++ ) {
		if ( *flashAdress != 0xFFFFFFFF ) {
			return FALSE ;
		}
	}
	HAL_Delay( 0 ) ;
	return TRUE ;
}

boolean FLASH_If_Verify ( uint32_t * source , uint32_t length ) {
	uint32_t *flashAdress = (uint32_t *)FLASH_USER_START_ADDR ;
	for ( uint32_t i = 0 ; i < length ; i ++  ) {

		if ( * (uint32_t*) ( flashAdress + i ) != * (uint32_t*) ( source + i ) ) {
			return FALSE ;
		}
	}
	HAL_Delay( 0 ) ;
	return TRUE ;
}
/* Public functions ---------------------------------------------------------*/
/**
 * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
 * @note   After writing data buffer, the flash content is checked.
 * @param  destination: start address for target location
 * @param  p_source: pointer on buffer with data to write
 * @param  length: length of data buffer (unit is 32-bit word)
 * @retval uint32_t 0: Data successfully written to Flash memory
 *         1: Error occurred while writing data in Flash memory
 *         2: Written Data in flash memory is different from expected one
 */
boolean FLASH_If_Write ( uint32_t *source , uint32_t length )
{
	uint32_t destination = FLASH_USER_START_ADDR ;
	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock () ;

	for ( uint32_t i = 0 ; (i < length) && (destination <= (FLASH_USER_END_ADDR-4)) ; i++ )
	{
		/* Device voltage range supposed to be [2.7V to 3.6V], the operation will be done by word */
		if (HAL_FLASH_Program ( FLASH_TYPEPROGRAM_WORD, destination, *(uint32_t*)(source+i) ) == HAL_OK)
		{
			/* Check the written value */
			if (*(uint32_t*)destination != *(uint32_t*)(source+i))
			{
				/* Flash content doesn't match SRAM content */
				return ( FALSE ) ;
			}
			/* Increment FLASH destination address */
			destination += 4 ;
		}
		else
		{
			/* Error occurred while writing data in Flash memory */
			return ( FALSE ) ;
		}
	}

	/* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock () ;
	HAL_Delay( 0 ) ;
	return ( TRUE ) ;
}

