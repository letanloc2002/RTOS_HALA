#include "uart.h"
#include <stdint.h>
#include "stm32f446xx.h"



void UART1_GPIO_Init(void)
{
	//enb clock gpioA
	RCC->AHB1ENR |= (1<<0);
	//GPIOA_MODER |= 0xFF;
		// config PA9
	//moder
	GPIOA->MODER |= (0b10<<18);
//	//otyper --> reset state
//	GPIOA_OTYPER &= ~(1<<2);
//	//ospeed
	GPIOA->OSPEEDR|= (3<<18);
//	// pupdr
//	GPIOA_PUPDR |= (1<<4)|(1<<6);
	// afrl
	GPIOA->AFR[1] |= (0b0111<<4);
	
		// config PA10
	//moder
	GPIOA->MODER |= (0b10<<20);
//	//otyper --> reset state
//	GPIOA_OTYPER &= ~(1<<3);
//	//ospeed
	GPIOA->OSPEEDR|= (3<<20);
//	// pupdr
//	GPIOA_PUPDR |= (1<<6);
	// afrl
	GPIOA->AFR[1] |= (0b0111<<8);
}
void USART1_Init(void)
{

	UART1_GPIO_Init();

	//enb clock usart2
	RCC->APB2ENR |= (1<<4);
	// clear all	
	USART1->CR1 = 0x00;  
	//enb rx interupt 
	USART1->CR1 		|= (1<<5);
	//enb usart2
	USART1->CR1 |= (1<<13);
	//bit M -8bits data
	USART1->CR1 &= ~(1<<12);	
	//baudrate	
	USART1->BRR |= (3<<0);
	USART1->BRR |= (104 << 4);
	//TX-RX mode
	USART1->CR1 |= (1<<3)|(1<<2);
	//parity
	USART1->CR1  &= ~(1<<10);
	//over8
	//USART2_CR1 |= (1<<15);
	/*
	set up baudrate - 9600bit/s, clock = 16MHz
	usart_div = 104,33333
	mantisa = 104
	fraction= 0,3333 x 8 = 2,6666 ~~ 3 
	*/

}
void USART1_ReceiveInT_Setup(void)
{

	NVIC->ISER[1] 	|= (1<<5);
	NVIC->ICPR[1]		|=(1<<5);
	__asm volatile("cpsie i");
}


void USART1_SendData(uint8_t *pTxBuffer,uint32_t Len)
{
	for(uint32_t count = 0;count < Len; count++)
	{
		while((USART1->SR & (1<<7)) == 0);
		USART1->DR =(uint8_t)(*pTxBuffer & (uint8_t)0xFF);
		pTxBuffer++;
	}
	while((USART1->SR &(1<<6)) == 0);
}

void USART1_ReceiveData(uint8_t *pRxBuffer,uint32_t Len)
{
	for(uint32_t count=0;count<Len;count++)
	{
		while((USART1->SR &(1<<5)) == 0);
		*pRxBuffer=(uint8_t)(USART1->DR&0xFF);
		pRxBuffer++;
	}
}
void GPIOA_Pin5_Init(void)
{
	RCC->AHB1ENR |= (1<<0);
	GPIOA->MODER |= (1<<10);
	GPIOA->OTYPER &= ~(1<<5);
	GPIOA->ODR 	&=	~(1<<5);
}

void GPIOA_Pin5_Toggle(void)
{
	GPIOA->ODR ^= (1<<5);
}
