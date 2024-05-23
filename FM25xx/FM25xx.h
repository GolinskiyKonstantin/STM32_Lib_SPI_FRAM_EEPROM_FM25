/*
	
  ******************************************************************************
  * @file 			( фаил ):   FM25xx.h
  * @brief 		( описание ):  	
  ******************************************************************************
  * @attention 	( внимание ):	author: Golinskiy Konstantin	e-mail: golinskiy.konstantin@gmail.com
  ******************************************************************************
  
*/

#ifndef _FM25XX_H
#define _FM25XX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------*/

#include "main.h"

/*

HOLD - 	Удержание: вывод /HOLD используется, когда главный процессор должен прервать операцию с памятью.
				для другой задачи. Когда /HOLD имеет низкий уровень, текущая операция приостанавливается. Устройство
				игнорирует любые переходы по SCK или /CS. Все переходы на /HOLD должны происходить во время
				SCK низкий.
				
WP - 		Защита от записи: этот активный контакт с низким уровнем предотвращает операции записи в регистр состояния. Этот
				имеет решающее значение, поскольку другие функции защиты от записи контролируются через статус
				регистр.

SO -		MISO
SI -		MOSI

				Скорость работы - интерфейсная шина (SPI). Предназначен для работы на скоростях до 16 МГц.
				
				режим работы SPI:   SPI Mode 0: CPOL=0, CPHA=0  и  SPI Mode 3: CPOL=1, CPHA=1
				( у меня заработало при CPOL=High, CPHA=2Edge )
				
				Напряжение питания Voltage Operation 3.0-3.6V
				( обязятельно к микрухе к питанию конденсатор ( без него чтение и запись прерывается ) )

				данная память не имеет страниц и не нужно ждать конца прожига чипа, 
				писать можно хоть с начала и до конца одним разом без ожидания после записи

*/

//####### SETUP ##############################################################################################
		
		// указываем порт SPI
		#define FM25XX_SPI				hspi1

		//--  Cесли используем порт WP для блокировки памяти тогда раскомментировать ------------
		// если на порту VCC то память заблоктрована для записи, если GND то разблокирована ( можно записывать )
		// читать можно в любом случае		
		#define WP_PORT		
		//----------------------------------------------------------------------------------------
		#ifdef WP_PORT
			#if !defined (WP_GPIO_Port)
				#define WP_GPIO_Port    GPIOA
				#define WP_Pin			GPIO_PIN_14
			#endif
		#endif
		//---------------------------------------------------------------------------------------
		// если используем CS то разкоментировать если нет то закоментировать.
		#define CS_PORT
		//----------------------------------------------------------------------------------------
		#ifdef CS_PORT
			#if !defined (CS_GPIO_Port)
				#define CS_GPIO_Port    GPIOA
				#define CS_Pin					GPIO_PIN_15
			#endif
		#endif
		//---------------------------------------------------------------------------------------
		// если используем HOLD то разкоментировать если нет то закоментировать.
		#define HOLD_PORT
		//----------------------------------------------------------------------------------------
		#ifdef HOLD_PORT
			#if !defined (HOLD_GPIO_Port)
				#define HOLD_GPIO_Port    GPIOA
				#define HOLD_Pin					GPIO_PIN_16
			#endif
		#endif
		//---------------------------------------------------------------------------------------
		
		// указываем тип памяти ( нужное разкоментировать )
		//#define FM25_1_KBITS				// -> 1K bit 		(Organized as 128 x 8 bits) 	Ferroelectric Nonvolatile RAM	
		//#define FM25_2_KBITS				// -> 2K bit 		(Organized as 256 x 8 bits) 	Ferroelectric Nonvolatile RAM
		//#define FM25_4_KBITS				// -> 4K bit 		(Organized as 512 x 8 bits) 	Ferroelectric Nonvolatile RAM
		//#define FM25_8_KBITS				// -> 8K bit 		(Organized as 1024 x 8 bits) 	Ferroelectric Nonvolatile RAM
		//#define FM25_16_KBITS				// -> 16K bit 	(Organized as 2048 x 8 bits) 	Ferroelectric Nonvolatile RAM
		//#define FM25_32_KBITS				// -> 32K bit 	(Organized as 4096 x 8 bits) 	Ferroelectric Nonvolatile RAM
		#define FM25_64_KBITS				// -> 64K bit 	(Organized as 8192 x 8 bits) 	Ferroelectric Nonvolatile RAM
		//#define FM25_128_KBITS			// -> 128K bit 	(Organized as 16384 x 8 bits) Ferroelectric Nonvolatile RAM
		//#define FM25_256_KBITS			// -> 256K bit 	(Organized as 32768 x 8 bits) Ferroelectric Nonvolatile RAM
		//#define FM25_512_KBITS			// -> 512K bit 	(Organized as 65536 x 8 bits) Ferroelectric Nonvolatile RAM
		
		// если память больше чем 512K bit то там по документации нужно отправлять три байта с адресом ( легко можно добавить этот функционал )
		
//############################################################################################################


//==========================================
// Op-code Commands
#define FM25XX_WREN				0x06
#define FM25XX_WRDI				0x04
#define FM25XX_RDSR				0x05
#define FM25XX_WRSR				0x01
#define FM25XX_READ				0x03
#define FM25XX_WRITE			0x02

#define FM25XX_PROTECT 		0x0C
#define FM25XX_UNPROTECT	0x00
//==========================================

// тип памяти
#if defined(FM25_1_KBITS)
    #define FM25XX_MAX_MEM_ADDRESS         128
#elif defined(FM25_2_KBITS)
    #define FM25XX_MAX_MEM_ADDRESS         256
#elif defined(FM25_4_KBITS)
    #define FM25XX_MAX_MEM_ADDRESS         512
#elif defined(FM25_8_KBITS)
    #define FM25XX_MAX_MEM_ADDRESS         1024
#elif defined(FM25_16_KBITS)
    #define FM25XX_MAX_MEM_ADDRESS         2048
#elif defined(FM25_32_KBITS)
    #define FM25XX_MAX_MEM_ADDRESS         4096
#elif defined(FM25_64_KBITS)
    #define FM25XX_MAX_MEM_ADDRESS         8192
#elif defined(FM25_128_KBITS)
    #define FM25XX_MAX_MEM_ADDRESS         16384
#elif defined(FM25_256_KBITS)
    #define FM25XX_MAX_MEM_ADDRESS         32768
#elif defined(FM25_512_KBITS)
    #define FM25XX_MAX_MEM_ADDRESS         65536
#endif


typedef enum{
	FM25_PIN_PROTECTED,
	FM25_PIN_UNPROTECTED
}pin_protected_t;

/* Functions prototypes ----------------------------------------------*/

void 			FM25xx_ProtectedAll(void);
void 			FM25xx_UnProtectedAll(void);
uint16_t 	FM25xx_ReadData(uint16_t address, uint8_t *read_ptr, uint16_t size);
uint16_t 	FM25xx_WriteData(uint16_t address, uint8_t *write_ptr, uint16_t size);
void 			FM25xx_EraseChip(void);
void 			FM25xx_PinWP_Protected(pin_protected_t status);
void 			FM25xx_PinHOLD_Protected(pin_protected_t status);


//------------------------------------------------------------------------------



#ifdef __cplusplus
}
#endif

#endif	/*	_FM25XX_H */

/************************ (C) COPYRIGHT GKP *****END OF FILE****/
