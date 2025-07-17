/*
 * crc32.c
 *
 *  Created on: 26 но€б. 2021 г.
 *      Author: Lab
 */
#include "main.h"
#include "crc32.h"
extern CRC_HandleTypeDef hcrc;
extern uint32_t g_pfnVectors[];

#define BUFFER_SIZE	*((uint32_t *)(0x08004000 + 7*4))/4   /*(g_pfnVectors[7]/4)*/

__IO uint32_t uwCRCValue = 0;
__IO uint32_t myCRCValue1 = 0xFFFFFFFF;

const uint32_t *myDataBuffer;

uint32_t Crc32(uint32_t Crc, uint32_t Data)
{
  uint8_t index;
  Crc = Crc ^ Data;
  for(index=0; index<32; index++)
  {
    if (Crc & 0x80000000)
      Crc = (Crc << 1) ^ 0x04C11DB7; // Polynomial used in STM32
    else
      Crc = (Crc << 1);
  }
  return(Crc);
}


uint8_t crc32_calc(void)
{
	  uint16_t myDataBufferCount = BUFFER_SIZE;
	  myDataBuffer = (uint32_t*) 0x08004000;

	  if(BUFFER_SIZE > 48*1024)//max firmware size validate
	  {
		  return 0;
	  }

	  uwCRCValue = HAL_CRC_Accumulate(&hcrc, (uint32_t *)0x08004000, BUFFER_SIZE);
	  while (myDataBufferCount--)
	  {
	    myCRCValue1 = Crc32(myCRCValue1, *myDataBuffer++);
	  }

	  if (uwCRCValue == *(uint32_t *)(0x08004000 + BUFFER_SIZE*4)) //if (uwCRCValue == *(uint32_t *)(0x08004000 + g_pfnVectors[7]))
	  {
		  return 1;
	  }
	  else
	  {
		  return 0;
	  }

};
