/*
 * io_redirection.h
 *
 *  Created on: 8 sept. 2019
 *      Author: gmsj
 */

#ifndef BOOT_BSP_IO_REDIRECTION_H_
#define BOOT_BSP_IO_REDIRECTION_H_


#ifdef __cplusplus
 extern "C" {
#endif

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar ( int ch )
#else
#define PUTCHAR_PROTOTYPE int fputc ( int ch , FILE *f )
#endif /* __GNUC__ */

#ifdef __cplusplus
}
#endif

#endif /* BOOT_BSP_IO_REDIRECTION_H_ */

