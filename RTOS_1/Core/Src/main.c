#include <stdint.h>
#include "osKernel.h"
#include "uart.h"


uint32_t volatile i0,i1,i2,p0,p1;
uint32_t semaphore1,semaphore2;
extern void (*periodicTask)(void);
uint8_t run[] = "motor is running !!!\r\n";
uint8_t stop[] = "motor is stoping !!!\r\n";

void MotorRun()
{
	USART1_SendData(run,sizeof(run)/sizeof(run[0]));
}
void MotorStop()
{
	USART1_SendData(stop,sizeof(stop)/sizeof(stop[0]));
}
void Task0(){
	while(1)
	{
//		osSpinLock_Wait(&semaphore1);
		i0++;
//		MotorRun();
//		 osSemaphore_Give(&semaphore2);
	}
}
void Task1(){
	while(1)
	{
//		osSpinLock_Wait(&semaphore2);
		i1++;
//		MotorStop();
//		osSemaphore_Give(&semaphore1);
	}
}


void Task2(){
	while(1)
	{
		i2++;
	}
}
void PeriodicTask0(void){
	p0++;
}
void PeriodicTask1(void){
	p1++;
}
int main(){

	osKernelAddPeriodicThreads(PeriodicTask0,100);
	osKernelAddPeriodicThreads(PeriodicTask1,450);
	osKernelAddThread(Task0, Task1, Task2);
//	osSemaphore_Init(&semaphore1, 1);
//	osSemaphore_Init(&semaphore2, 0);
	osPeriodicTask_Init(periodic_events_exe,1000,1);
	USART1_Init();
	osKernelInit();
	while(1){
	
	}
}
