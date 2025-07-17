/*
 * modbus_h.h
 *
 *  Created on: 13 лип. 2018 р.
 *      Author: LEXX
 */

#ifndef MODBUS_H_H_
#define MODBUS_H_H_

//example main.h
//#define F_AHB  72000000ull
//#define F_APB1 36000000ull
//#define F_APB2 72000000ull
//
//#define MB_UART2
//#define MB_UART_NVIC_PRIORITY 5
//
//#define MB_NOT_USE_DEM//управление направлением передачи не используется
//
//#define MB_REGS_LEN 0x120
//end main.h

void MB_Init(uint32_t baudrate, uint16_t timeout, uint8_t addr);
void MB_Send_Data(uint8_t *buf, int len);
void WriteFloatToMBRegs(uint16_t* _MBRegs, uint16_t MBRegsN, float data);
float ReadFloatFromMBRegs(uint16_t* _MBRegs, uint16_t MBRegsN);
#endif /* MODBUS_H_H_ */
