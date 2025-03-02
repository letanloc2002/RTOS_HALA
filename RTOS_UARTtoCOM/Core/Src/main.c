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
#define RX_BUFFER_SIZE 100 // Kích thu?c buffer nh?n d? li?u

// Bi?n toàn c?c
char rxBuffer[RX_BUFFER_SIZE]; // Buffer d? luu d? li?u nh?n du?c
volatile uint8_t rxIndex = 0;  // Index d? theo dõi v? trí trong buffer
volatile uint8_t rxFlag = 0;   // C? báo hi?u dã nh?n d? chu?i
void USART1_IRQHandler(void)
{
	if (USART1->SR & USART_SR_RXNE) { // Ki?m tra xem có d? li?u m?i không
        char receivedChar = (char)(USART1->DR & 0xFF); // Ð?c ký t?

        // Luu ký t? vào buffer
        if (rxIndex < RX_BUFFER_SIZE - 1) {
            rxBuffer[rxIndex++] = receivedChar;
        }

        // Ki?m tra ký t? k?t thúc chu?i
        if (receivedChar == 'n' || receivedChar == '\r') {
            rxBuffer[rxIndex] = '\0'; // K?t thúc chu?i
            rxFlag = 1; // B?t c? báo hi?u dã nh?n d? chu?i
            rxIndex = 0; // Reset index
        }
    }
} 
int main(void)
{
	USART1_Init();
	USART1_ReceiveInT_Setup();
	USART1_SendData((uint8_t *)sendata,sizeof(sendata));
  while (1)
  {
		//rxIndex = 0;
  }

}
