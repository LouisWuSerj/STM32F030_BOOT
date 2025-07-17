#ifndef _MODBUS_L_H_
#define _MODBUS_L_H_ 1

#include "inttypes.h"
//#include "main.h"


typedef void reset_modbus_timer_t(void);
typedef void modbus_senddata_t(uint8_t *buf, int len);
typedef void modbus_needReg_t(uint16_t Addr);

extern reset_modbus_timer_t *modbus_resetTimer;
extern modbus_senddata_t *modbus_sendData;
extern modbus_needReg_t *modbus_NeedReg;

extern uint16_t *mb_registers_buf;


extern uint16_t modbus_err_alienpkt;
extern uint16_t modbus_err_longpkt;
extern uint16_t modbus_err_badpkt;
extern uint16_t modbus_err_badcrc;
extern uint16_t modbus_cnt_chars;
extern uint16_t modbus_cnt_okchars;
extern uint16_t modbus_cnt_fn3;
extern uint16_t modbus_cnt_fn4;
extern uint16_t modbus_cnt_if;
extern uint16_t modbus_cnt_ex;
extern uint16_t modbus_LastChar;
extern uint16_t modbus_rxcrc;



/** modbus_CRC16 - calculate ModBus CRC16 buffer checksum
 * \param buf buffer pointer
 * \param Len buffer size
 * \return CRC
 */
uint16_t modbus_CRC16( uint8_t *buf, uint16_t Len );



/** ������ ������� ������������� ��� ��������� ��������� ����� �� master-���������� 
 * � ���������� modbus-���������.
 * \param Data - �������� ���� ������
 */
void modbus_newchar(uint8_t Data);

/** ������ ������� _������_ ���� ������� ����� ���������� 
 * ������ � ������� �������, ������� ������������ �������� ���� ��������.
 * �� ����� ������ ������ ������� ������������ ��������� modbus-���������.
 */
int modbus_2ch_timeout(void);

/** ��������� ������������� ���������.
 * \param Addr modbus-����� ������ ����.
 */
void modbus_init(uint8_t Addr);




#endif 
