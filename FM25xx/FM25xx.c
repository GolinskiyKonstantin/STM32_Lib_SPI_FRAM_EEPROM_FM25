/*

  ******************************************************************************
  * @file 			( фаил ):   FM25xx.c
  * @brief 		( описание ):  	
  ******************************************************************************
  * @attention 	( внимание ):	author: Golinskiy Konstantin	e-mail: golinskiy.konstantin@gmail.com
  ******************************************************************************
  
*/

/* Includes ----------------------------------------------------------*/
#include "FM25xx.h"

#define FM25XX_SPI_TIMEOUT						100

extern SPI_HandleTypeDef FM25XX_SPI;


//------------------------------------------------
static void FM25xx_CS_Enable(void)
{
	#ifdef CS_PORT
		HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
	#endif
}

static void FM25xx_CS_Disable(void)
{
	#ifdef CS_PORT
		HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
	#endif
}

static void FM25xx_SPI_Read(uint8_t* data, uint16_t size)
{
	HAL_SPI_Receive(&FM25XX_SPI, data, size, FM25XX_SPI_TIMEOUT );
}

static void FM25xx_SPI_Write(uint8_t* data, uint16_t size)
{
	HAL_SPI_Transmit(&FM25XX_SPI, data, size, FM25XX_SPI_TIMEOUT );
}

static void FM25xx_WriteEnable(void)
{
    FM25xx_CS_Enable();
	
		uint8_t reg = FM25XX_WREN;
		FM25xx_SPI_Write(&reg, 1); //Write Enable OpCode
	
    FM25xx_CS_Disable();
}

static void FM25xx_WriteDisable(void)
{
    FM25xx_CS_Enable();

		uint8_t reg = FM25XX_WRDI;
		FM25xx_SPI_Write(&reg, 1); //Write Disable OpCode
	
    FM25xx_CS_Disable();
}

static uint8_t FM25xx_ReadState(void)
{
  FM25xx_CS_Enable();

	uint8_t reg = FM25XX_RDSR;
	FM25xx_SPI_Write(&reg, 1);

	uint8_t output = 0;
	FM25xx_SPI_Read(&output, 1);
	
  FM25xx_CS_Disable();
  
  return output;
}

static void FM25xx_WriteState(uint8_t data)
{
  FM25xx_WriteEnable();
  
  FM25xx_CS_Enable();

	uint8_t reg = FM25XX_WRSR;
	FM25xx_SPI_Write(&reg, 1);
	FM25xx_SPI_Write(&data, 1);
	
  FM25xx_CS_Disable();
	
	FM25xx_WriteDisable();
}
//-------------------------------------------------



void FM25xx_ProtectedAll(void)
{
  FM25xx_WriteState(FM25XX_PROTECT);
}

void FM25xx_UnProtectedAll(void)
{
  FM25xx_WriteState(FM25XX_UNPROTECT);
}

uint16_t FM25xx_ReadData(uint16_t address, uint8_t *read_ptr, uint16_t size)
{	
		if( (address + size) < FM25XX_MAX_MEM_ADDRESS ){
			FM25xx_CS_Enable();

			uint8_t reg = FM25XX_READ;
			FM25xx_SPI_Write(&reg, 1);
			
	#if (defined FM25_1_KBITS) || (defined FM25_2_KBITS)
		  reg = (address);							
			FM25xx_SPI_Write(&reg, 1);
	#else	
			reg = (address>>8);							//high byte
			FM25xx_SPI_Write(&reg, 1);
			reg = (address&0x00FF);					//low byte
			FM25xx_SPI_Write(&reg, 1);
	#endif
			
			FM25xx_SPI_Read(read_ptr, size);
			
			FM25xx_CS_Disable();
			
			return address + size;
		}
		else{
			return 0;
		}
}

uint16_t FM25xx_WriteData(uint16_t address, uint8_t *write_ptr, uint16_t size)
{
		if( (address + size) < FM25XX_MAX_MEM_ADDRESS ){
			FM25xx_WriteEnable();
			FM25xx_CS_Enable();
		
			uint8_t reg = FM25XX_WRITE;
			FM25xx_SPI_Write(&reg, 1);
			
	#if (defined FM25_1_KBITS) || (defined FM25_2_KBITS)
		  reg = (address);							
			FM25xx_SPI_Write(&reg, 1);
	#else	
			reg = (address>>8);							//high byte
			FM25xx_SPI_Write(&reg, 1);
			reg = (address&0x00FF);					//low byte
			FM25xx_SPI_Write(&reg, 1);
	#endif
			
			FM25xx_SPI_Write( write_ptr, size );
			
			FM25xx_CS_Disable();
			FM25xx_WriteDisable();
			
			return address + size;
		}
		else{
			return 0;
		}
}

void FM25xx_EraseChip(void)
{
	uint8_t data_ptr = 0xFF;
	uint16_t address = 0;
	uint16_t size = FM25XX_MAX_MEM_ADDRESS;
	
	FM25xx_WriteEnable();
	FM25xx_CS_Enable();

	uint8_t reg = FM25XX_WRITE;
	FM25xx_SPI_Write(&reg, 1);
	
	#if (defined FM25_1_KBITS) || (defined FM25_2_KBITS)
		  reg = (address);							
			FM25xx_SPI_Write(&reg, 1);
	#else	
			reg = (address>>8);							//high byte
			FM25xx_SPI_Write(&reg, 1);
			reg = (address&0x00FF);					//low byte
			FM25xx_SPI_Write(&reg, 1);
	#endif
	
	while (size--){
		FM25xx_SPI_Write(&data_ptr, 1); 
	}
	
	FM25xx_CS_Disable();
	FM25xx_WriteDisable();
}

void FM25xx_PinWP_Protected(pin_protected_t status)
{
	#ifdef WP_PORT
		// если на LOW (GND) то блокируется на запись
		if( FM25_PIN_PROTECTED == status ){
			HAL_GPIO_WritePin(WP_GPIO_Port, WP_Pin, GPIO_PIN_RESET );	
		}
		else if( FM25_PIN_UNPROTECTED == status ){
			HAL_GPIO_WritePin(WP_GPIO_Port, WP_Pin, GPIO_PIN_SET );	
		}
	#endif
}

void FM25xx_PinHOLD_Protected(pin_protected_t status)
{
	// если на LOW (GND) то блокируется и чтение, запись передача или чтение команд ( полностью связь с чипом )
	#ifdef HOLD_PORT
		FM25xx_CS_Enable();
		
		if( FM25_PIN_PROTECTED == status ){
			HAL_GPIO_WritePin(HOLD_GPIO_Port, HOLD_Pin, GPIO_PIN_RESET );	
		}
		else if( FM25_PIN_UNPROTECTED == status ){
			HAL_GPIO_WritePin(HOLD_GPIO_Port, HOLD_Pin, GPIO_PIN_SET );	
		}
		
		FM25xx_CS_Disable();
	#endif
}

//----------------------------------------------------------------------------------

/************************ (C) COPYRIGHT GKP *****END OF FILE****/
