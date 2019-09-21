/*
 * util.cpp
 *
 *  Created on: 8 sept. 2019
 *      Author: gilles
 */


#include "utils.h"
#include "stm32f7xx_hal.h"

uint32_t lastTime = 0 ;

uint32_t micros ( void )
{
	uint32_t countMax = (uint32_t)( SysTick->LOAD & SysTick_LOAD_RELOAD_Msk ) ;
	__disable_irq();
	uint32_t _micros = 1000 * ( ( countMax - (uint32_t)( SysTick->VAL & SysTick_VAL_CURRENT_Msk ) ) / (double) countMax ) ;
	uint32_t time = HAL_GetTick () * 1000 + _micros ;
	__enable_irq();
	return time ;
}


uint32_t millis ( void )
{
	return HAL_GetTick () ;
}

double _fastAtan2 ( double y, double x ) {
  static double atan ;
  static double z ;
  if ( x == 0.0f ) {
    if ( y > 0.0f ) return PIBY2_FLOAT ;
    if ( y == 0.0f ) return 0.0f ;
    return -PIBY2_FLOAT ;
  }
  z = y / x ;
  if ( fabs ( z ) < 1.0f )
  {
    atan = z/(1.0f + 0.28f*z*z);
    if ( x < 0.0f )
    {
      if ( y < 0.0f ) return atan - PI_FLOAT;
      return atan + PI_FLOAT;
    }
  }
  else
  {
    atan = PIBY2_FLOAT - z/(z*z + 0.28f);
    if ( y < 0.0f ) return atan - PI_FLOAT;
  }
  return atan;
}

void delay_ms ( uint32_t delay )
{
	HAL_Delay ( delay ) ;
}

void delay_us ( uint32_t delay )
{
	uint32_t start = micros () ;
	while ( micros () - start < delay ) ;
}
void delay ( long delay ) {
	delay_ms ( delay );
}
void delayMicroseconds ( long delay ) {
	delay_us ( delay ) ;
}

