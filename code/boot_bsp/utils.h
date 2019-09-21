/*
 * util.h
 *
 *  Created on: 8 sept. 2019
 *      Author: gmsj
 */

#ifndef BOOT_BSP_UTILS_H_
#define BOOT_BSP_UTILS_H_

#include "stm32f7xx_nucleo_144.h"
#include "stm32f7xx_hal.h"
#include "core_cm7.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "stdbool.h"

#ifdef __cplusplus
  extern "C" {
#endif

// polebot_error :
typedef enum
{
	NO_ERROR = 0,
	MOTOR_CONFIG ,
	ACCELOROMETER_CONFIG ,
	BAROMETER_CONFIG ,
	GYROMETER_CONFIG ,
	MAGNETOMETER_CONFIG ,
	MAGNETOMETER_DATA_NOT_READY_ERROR ,
	TRACKER_CONFIG ,
	I2C_WRITE ,
	I2C_READ ,
	I2C_MULTI_READ ,
	ENCODER_ERROR ,
	SLIDING_WINDOW ,
	ADC_ERROR ,
	NAN_ERROR ,
	FLASH_ERROR ,
	MAIN_ERROR
} app_error_t ;

extern void app_error ( app_error_t error ) ;

#ifdef __cplusplus
  }
#endif

typedef void ( * Callback ) ( void ) ;

#ifndef PI
#define PI 3.14159265358979323846264338327
#endif

#ifndef UINT10_MAX
#define UINT10_MAX	1023
#endif


#ifndef UINT12_MAX
#define UINT12_MAX	4095
#endif

#ifndef INT16_MAX
#define INT16_MAX	0x7FFF
#endif
#ifndef UINT16_MAX
#define UINT16_MAX	0xFFFF
#endif
#define CONSTRAIN( x , min , max )					( ( (x) < (min) ) ? (min) : ( (x) > (max) ) ? (max) : (x) )
#ifndef MIN
	#define MIN(a,b) 								( ( (a) < (b) ) ? (a) : (b) )
#endif
#define MAX(a,b) 									( ( (a) > (b) ) ? (a) : (b) )
#define MAP( x, in_min, in_max, out_min, out_max) 	( ( (x) - (in_min) ) * ( (out_max) - (out_min) ) / ( (in_max) - (in_min) ) + (out_min) )
#define _MAP( x, in_min, in_max, out_min, out_max) 	( ( (double)(x) - (double)(in_min) ) * ( (double)(out_max) - (double)(out_min) ) / ( (double)(in_max) - (double)(in_min) ) + (double)(out_min) )
#define SIGN(x)										( ( (x) >= 0 ) ? 1 : -1 )
#define ABS(x)										( ( (x) >= 0 ) ? (x) : -(x) )

#define ToRad(x) 				( (x) * 0.017453292 )   // *pi/180  52
#define ToDeg(x) 				( (x) * 57.29577951  )  // *180/pi 31
#define G_FORCE_TO_DEGREES(x) 	( ToDeg ( asin(x) ) )    // sin(x)*180/pi

#define ROUND_2_INT(f) 			((int)((f) >= 0.0 ? (f + 0.5) : ((f) - 0.5)))
#define	CM_TO_METER(cm)			((double)(cm) / 100.0 )
#define	MM_TO_METER(mm)			((double)(mm) / 1000.0 )
//#define PI_FLOAT     			3.14159265f
//#define PIBY2_FLOAT  			1.5707963f
#define PI_FLOAT     			(PI)
#define PIBY2_FLOAT  			(2*PI)
double _fastAtan2 ( double y, double x ) ;

uint32_t 	micros 			( void ) ;
uint32_t 	millis 			( void ) ;

void delay_ms 				( uint32_t msTime ) ;
void delay_us 				( uint32_t nTime ) ;
void delay			 		( long delay ) ;
void delayMicroseconds 		( long delay ) ;

#endif /* BOOT_BSP_UTILS_H_ */
