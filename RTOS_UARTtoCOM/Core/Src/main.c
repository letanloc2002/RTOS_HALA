/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f446xx.h"
#include "main.h"
#include "uart.h"
#include <stdint.h>
#include <stdbool.h>
uint8_t sendata[]="Select task!!!\r\n";
volatile uint8_t rxBuf[100]; 
void USART1_IRQHandler(void)
{
	if(USART1->SR&(1<<5))
	{
		rxBuf[0]=(uint8_t)(USART1->DR & 0xFF);
		//do sth
	}
	USART1->SR 			&=~(1<<5);
	NVIC->ICPR[1]		|=(1<<5);
} 
int main(void)
{
	USART1_Init();
	USART1_ReceiveInT_Setup();
	USART1_SendData((uint8_t *)sendata,sizeof(sendata));
  while (1)
  {

  }

}
