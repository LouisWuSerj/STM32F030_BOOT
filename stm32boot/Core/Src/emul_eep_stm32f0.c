/*
 * emul_eep.c
 *
 *  Created on: 18 июл. 2018 г.
 *      Author: Lab
 */
#include "stm32f0xx.h"
#include "emul_eep_stm32f0.h"

void Internal_Flash_Unlock(void)
{
	FLASH->KEYR = 0x45670123;
	FLASH->KEYR = 0xCDEF89AB;
};

void Internal_Flash_Lock(void)
{
	FLASH->CR |= FLASH_CR_LOCK;
};

//pageAddress - любой адрес, принадлежащий стираемой странице
void Internal_Flash_Erase(uint32_t pageAddress)
{
	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP)
	{
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR |= FLASH_CR_PER;
	FLASH->AR = pageAddress;
	FLASH->CR |= FLASH_CR_STRT;
	while (!(FLASH->SR & FLASH_SR_EOP));
	FLASH->SR = FLASH_SR_EOP;
	FLASH->CR &= ~FLASH_CR_PER;
}

//data - указатель на записываемые данные
//address - адрес во flash
//count - количество записываемых данных
void Internal_Flash_Write(uint16_t* data, uint32_t address, uint16_t count)
{
	uint16_t i;

	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP)
	{
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR |= FLASH_CR_PG;

	for (i = 0; i < count; i ++)
	{
		*(volatile uint16_t*)(address + i*2) = data[i];//2//4
		while (!(FLASH->SR & FLASH_SR_EOP));
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR &= ~(FLASH_CR_PG);
}

uint32_t Internal_Flash_Read(uint32_t address)
{
    return (*(__IO uint32_t*)address);
}

