#include <stm32f0xx_hal.h>
#include "main.h"
#include "modbus_l.h"
#include "modbus_h.h"

//#define MB_NOT_USE_DMATX

extern MBVar_t MBRegs;

#ifdef MB_UART1
	#define MB_DMA_TX DMA1_Channel4
	#define MB_UART USART1
	#define MB_UART_IRQ_HANDLER USART1_IRQHandler
	#define MB_UART_IRQn        USART1_IRQn
#endif
#ifdef MB_UART2
	#define MB_DMA_TX DMA1_Channel7
	#define MB_UART USART2
	#define MB_UART_IRQ_HANDLER USART2_IRQHandler
	#define MB_UART_IRQn        USART2_IRQn
#endif
#ifdef MB_UART3
	#define MB_DMA_TX DMA1_Channel2
	#define MB_UART USART3
	#define MB_UART_IRQ_HANDLER USART3_IRQHandler
	#define MB_UART_IRQn        USART3_IRQn
#endif

#ifdef MB_NOT_USE_DMATX
	volatile uint16_t tx_cnt = 0;
	volatile uint16_t tx_pkt_len = 0;
	uint8_t* pbuf;
#endif

void MB_UART_IRQ_HANDLER(void)
{
	if(MB_UART->ISR & USART_ISR_ORE)
	{

		MB_UART->ICR |= USART_ICR_ORECF;
	}
	if(MB_UART->ISR & USART_ISR_RXNE)
	{
		uint8_t Data = MB_UART->RDR;
		modbus_newchar(Data);
	}
    if(MB_UART->ISR & USART_ISR_RTOF)
    {
    	modbus_2ch_timeout();
        MB_UART->ICR |= USART_ICR_RTOCF;
    }

#ifdef MB_NOT_USE_DMATX
    if(MB_UART->ISR & USART_ISR_TC)
    {
    	if(tx_cnt < tx_pkt_len-1)
    	{
    		tx_cnt++;
    		MB_UART->TDR = *(pbuf + tx_cnt);
    	}
    	MB_UART->ICR |= USART_ICR_TCCF;
    }
#endif
};

void MB_Send_Data(uint8_t *buf, int len)
{
#ifdef MB_NOT_USE_DMATX
	tx_cnt = 0;
	pbuf = buf;
	tx_pkt_len = len;
	MB_UART->TDR = buf[tx_cnt];
#endif

#ifndef MB_NOT_USE_DMATX
	MB_DMA_TX->CCR &= ~DMA_CCR_EN;
	__disable_irq ();
	MB_DMA_TX->CMAR = (uint32_t)(buf);
	MB_DMA_TX->CNDTR = len;
	__enable_irq ();
	MB_DMA_TX->CCR |= DMA_CCR_EN;
#endif
};

void MB_Init(uint32_t baudrate, uint16_t timeout, uint8_t addr)
{
	modbus_sendData = &MB_Send_Data;
	mb_registers_buf = (uint16_t*)&MBRegs;
	modbus_init(addr);
	#ifdef MB_UART1
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	#endif
	#ifdef MB_UART2
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	#endif
	#ifdef MB_UART3
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	#endif

#ifndef MB_NOT_USE_DMATX
	RCC->AHBENR  |= RCC_AHBENR_DMA1EN;
#endif

#ifdef MB_UART1
	MB_UART->BRR  = F_APB2/baudrate;
#endif
#ifdef MB_UART2
	MB_UART->BRR  = F_APB1/baudrate;
#endif
#ifdef MB_UART3
	MB_UART->BRR  = F_APB2/baudrate;
#endif

	MB_UART->CR1 |= USART_CR1_TE | USART_CR1_RE;
	MB_UART->CR1 |= USART_CR1_RXNEIE;//��������� �� ������ ������

#ifndef MB_NOT_USE_DMATX
	MB_UART->CR3 |= USART_CR3_DMAT;//DMA enable transmitter
#endif

#ifdef MB_NOT_USE_DMATX
	MB_UART->CR1 |= USART_CR1_TCIE;
#endif

#ifndef MB_NOT_USE_DEM
	MB_UART->CR3 |= USART_CR3_DEM;//Driver enable mode //RS485TE
	MB_UART->CR1 |= USART_CR1_DEAT | USART_CR1_DEDT;
#endif

    MB_UART->CR1 |= USART_CR1_RTOIE;
    MB_UART->CR2 |= USART_CR2_RTOEN;
    MB_UART->RTOR = (timeout & USART_RTOR_RTO);//����-��� 2 ������� (�������� ������ ������� �������� ���� ��� + 8 ��� ������ + ���� ��� = 10 ���)
	MB_UART->CR1 |= USART_CR1_UE;
	NVIC_EnableIRQ(MB_UART_IRQn);
	NVIC_SetPriority(MB_UART_IRQn, MB_UART_NVIC_PRIORITY);

#ifndef MB_NOT_USE_DMATX
	SYSCFG->CFGR1 |= SYSCFG_CFGR1_USART1TX_DMA_RMP;
	MB_DMA_TX->CCR |= DMA_CCR_MINC | DMA_CCR_DIR;
	MB_DMA_TX->CPAR = (uint32_t)&(MB_UART->TDR);
#endif
};


void WriteFloatToMBRegs(uint16_t* _MBRegs, uint16_t MBRegsN, float data)
{
	uint32_t temp;
	*((float*)&temp) = *(&data);
	_MBRegs[MBRegsN+0] = temp & 0x0000FFFF;
	_MBRegs[MBRegsN+1] = (temp >> 16) & 0x0000FFFF;
};


float ReadFloatFromMBRegs(uint16_t* _MBRegs, uint16_t MBRegsN)
{
	uint32_t temp = 0;
	float var;
	temp = ((_MBRegs[MBRegsN+1] << 16) | (_MBRegs[MBRegsN+0]));
	var = *((float*)(&temp));
	return var;
};

