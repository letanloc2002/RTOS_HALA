#include "stm32f4xx.h"
#include "stm32f446xx.h"
#include "main.h"
#include "stm32f4xx_flash.h"
#include "uart.h"
#include <stdint.h>
#include <stdbool.h>
#define ADDR_TASK_1	0x08004000
#define ADDR_TASK_2	0x08005000
#define ADDR_TASK_3	0x08006000
volatile uint32_t address = 0;
void Flash_Erase(uint32_t flag_sector)
{
	FLASH_Unlock();
	while( FLASH_GetFlagStatus(FLASH_FLAG_BSY) == 1);
	FLASH_Erase_Sector(flag_sector,VoltageRange_3);
	while(FLASH_GetFlagStatus(FLASH_FLAG_BSY)==1);
	FLASH_Lock();
}

void Flash_WriteInt(uint32_t address, uint16_t value)
{
	FLASH_Unlock();
	while( FLASH_GetFlagStatus(FLASH_FLAG_BSY) == 1);
	FLASH_ProgramHalfWord(address,value);
	while(FLASH_GetFlagStatus(FLASH_FLAG_BSY)==1);
	FLASH_Lock();
}

void Flash_WriteNumByte(uint32_t address, uint8_t *data, int num)
{
	FLASH_Unlock();
	while( FLASH_GetFlagStatus(FLASH_FLAG_BSY) == 1);
	uint16_t *ptr = (uint16_t *)data;
	for(int i=0;i<(num+1)/2;i++)
	{
		FLASH_ProgramHalfWord(address+2*i,*ptr);
		while(FLASH_GetFlagStatus(FLASH_FLAG_BSY)==1);
		ptr++;
	}
	FLASH_Lock();
}
void Boot()
{
	HAL_RCC_DeInit();
	SCB->SHCSR &= ~(SCB_SHCSR_USGFAULTENA_Msk|SCB_SHCSR_MEMFAULTENA_Msk|SCB_SHCSR_BUSFAULTENA_Msk);
	__set_MSP(*(__IO uint32_t*)(address));//ghi giá tr? vào stackpointer, back up ctrinh phòng khi exception xay ra
	uint32_t JumpAddress=*(__IO uint32_t*)(address+4);//lay gia tri o nho t2 cua procces
	void (*reset_handler)(void) = (void*) JumpAddress;
	reset_handler();
}
uint8_t buf1[]="task1 is running\r\n";
uint8_t buf2[]="task2 is running\r\n";
uint8_t buf3[]="task3 is running\r\n";
uint8_t sendata[]="Select task!!!\r\n";
uint8_t sendata1[]="Loading Task 1!!!\r\n";
uint8_t sendata2[]="Loading Task 2!!!\r\n";
uint8_t sendata3[]="Loading Task 3!!!\r\n";
volatile uint8_t rxBuf[1];
 
void task1(void)
{
	USART1_SendData((uint8_t *)buf1,sizeof(buf1));
}
void task2(void)
{
	USART1_SendData((uint8_t *)buf2,sizeof(buf2));
}
void task3(void)
{
	USART1_SendData((uint8_t *)buf3,sizeof(buf3));
}

void USART1_IRQHandler(void)
{
	if(USART1->SR&(1<<5))
	{
		rxBuf[0]=(uint8_t)(USART1->DR & 0xFF);
			switch ((char)rxBuf[0])
    {
        case '1':
						USART1_SendData((uint8_t *)sendata1,sizeof(sendata1));
            address = ADDR_TASK_1; // Boot vào task1
					//Boot(ADDR_TASK_1);
            break;
        case '2':
						USART1_SendData((uint8_t *)sendata2,sizeof(sendata2));
            address = ADDR_TASK_2; // Boot vào task2
            break;
        case '3':
						USART1_SendData((uint8_t *)sendata3,sizeof(sendata3));
            address = ADDR_TASK_3; // Boot vào task3
            break;
        default:
            // X? lý tru?ng h?p khác (n?u c?n)
            break;
    }
	}
	Boot();
	USART1->SR 			&=~(1<<5);
	NVIC->ICPR[1]		|=(1<<5);
} 

int main()
{
	USART1_Init();
	USART1_ReceiveInT_Setup();
	USART1_SendData((uint8_t *)sendata,sizeof(sendata));
//	switch ((char)rxBuf[0])
//    {
//        case '1':
//						USART1_SendData((uint8_t *)sendata1,sizeof(sendata1));
//            Boot(ADDR_TASK_1); // Boot vào task1
//            break;
//        case '2':
//						USART1_SendData((uint8_t *)sendata1,sizeof(sendata1));
//            Boot(ADDR_TASK_2); // Boot vào task2
//            break;
//        case '3':
//						USART1_SendData((uint8_t *)sendata1,sizeof(sendata1));
//            Boot(ADDR_TASK_3); // Boot vào task3
//            break;
//        default:
//            // X? lý tru?ng h?p khác (n?u c?n)
//            break;
//    }
//	address = ADDR_TASK_3; // Boot vào task3
//	Boot();
	while(1)
	{	
	}
}