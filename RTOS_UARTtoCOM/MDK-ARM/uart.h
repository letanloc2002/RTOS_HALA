#ifndef __UART_H
#define __UART_H
#include <stdint.h>
void UART1_GPIO_Init(void);
void USART1_Init(void);
void USART1_SendData(uint8_t *pTxBuffer,uint32_t Len);
void USART1_ReceiveData(uint8_t *pRxBuffer,uint32_t Len);
void GPIOA_Pin5_Init(void);
void GPIOA_Pin5_Toggle(void);
//void Uart2_Send(int c);

void USART1_ReceiveInT_Setup(void);
#endif