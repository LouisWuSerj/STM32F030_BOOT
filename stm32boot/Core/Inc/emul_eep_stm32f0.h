/*
 * emul_eep_stm32f0.h
 *
 *  Created on: 18 èþë. 2018 ã.
 *      Author: Lab
 */

#ifndef EMUL_EEP_F0_H_
#define EMUL_EEP_F0_H_

void Internal_Flash_Unlock(void);
void Internal_Flash_Lock(void);
void Internal_Flash_Erase(uint32_t pageAddress);
void Internal_Flash_Write(uint16_t* data, uint32_t address, uint16_t count);
uint32_t Internal_Flash_Read(uint32_t address);
void Internal_Flash_Read_Settings(uint16_t *data, uint16_t struct_len, uint32_t StartAddr);

#endif /* EMUL_EEP_F0_H_ */
