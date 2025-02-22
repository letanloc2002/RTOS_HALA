#include "osKernel.h"
#include <stdint.h>

//#file osKernel.c
//Khai bao so luong thread 
#define NUM_OF_THREADS		3
//TCB Struct
struct tcb{
	uint32_t *stackPt;
	struct tcb* nextPt;
	
};
typedef struct tcb tcbType;
//TCB for each thread
tcbType tcbs[NUM_OF_THREADS];
//CurrentPt for deciding which thread is running.
tcbType *currentPt;
//#file osKernel.c
//Khai bao kich co Stack cho moi thread 
#define STACK_SIZE 			100
//Khoi tao vung nho Stack
uint32_t TCB_STACK[NUM_OF_THREADS][STACK_SIZE];

//File osKernel.c
//Clock freq for SysTick	
#define BUS_FREQ 			72000000
//Init some value for SysTick Reg
#define CTRL_ENABLE 		(1<<0)
#define CTRL_TICKINT 		(1<<1)
#define CTRL_CLCKSRC 		(1<<2)
#define CTRL_COUNTFLAG 		(1<<16)
#define SYSTICK_RST 		0
//Set value for SysTick (to calculate quanta time)
uint32_t MILLIS_PRESCALER;

//File osKernel.c
/*Ex
1s -> Bus_Freq 
1000ms-> Bus_Freq
1ms -> Bus_Freq/1000
*/


void osKernelLaunch(uint32_t quanta){
	/*Reset the SysTick*/
	SysTick->CTRL = SYSTICK_RST;
	/*Clear Systick current value register*/
	SysTick->VAL = 0;
	/*Load the quanta*/
	SysTick->LOAD = (quanta*MILLIS_PRESCALER-1);
	/*Set Systick interupt to lowest priority*/
	NVIC_SetPriority(SysTick_IRQn, 15);
	/*Enable Systick, select Internal clock*/
	SysTick->CTRL |= CTRL_CLCKSRC|CTRL_ENABLE;
	/*Enable Systick interupt*/
	SysTick->CTRL |= CTRL_TICKINT;
	//os launch scheduler
	osSchedulerLaunch();
}
void osKernelInit(void){
	//1ms
	MILLIS_PRESCALER = (BUS_FREQ/1000);
	osKernelLaunch(10);
}

void osKernelStackInit(int i){
	tcbs[i].stackPt = &TCB_STACK[i][STACK_SIZE-16];
	/*Set bit21 (T-Bit) in PSR to 1 to operate it*/
	/*Thumb Mode*/
	TCB_STACK[i][STACK_SIZE-1] = 1<<24;	/*PSR*/
	/*Skip the PC*/
	//
	/*Dumming Stack Content*/
	TCB_STACK[i][STACK_SIZE-3] = 0xAAAAAAAA;/*R14 (LR)*/
	TCB_STACK[i][STACK_SIZE-4] = 0xAAAAAAAA;/*R12*/
	TCB_STACK[i][STACK_SIZE-5] = 0xAAAAAAAA;/*R3*/
	TCB_STACK[i][STACK_SIZE-6] = 0xAAAAAAAA;/*R2*/
	TCB_STACK[i][STACK_SIZE-7] = 0xAAAAAAAA;/*R1*/
	TCB_STACK[i][STACK_SIZE-8] = 0xAAAAAAAA;/*R0*/
	
	TCB_STACK[i][STACK_SIZE-9] = 0xAAAAAAAA; /*R11*/
	TCB_STACK[i][STACK_SIZE-10] = 0xAAAAAAAA;/*R10*/
	TCB_STACK[i][STACK_SIZE-11] = 0xAAAAAAAA;/*R9*/
	TCB_STACK[i][STACK_SIZE-12] = 0xAAAAAAAA;/*R8*/
	TCB_STACK[i][STACK_SIZE-13] = 0xAAAAAAAA;/*R7*/
	TCB_STACK[i][STACK_SIZE-14] = 0xAAAAAAAA;/*R6*/
	TCB_STACK[i][STACK_SIZE-15] = 0xAAAAAAAA;/*R5*/
	TCB_STACK[i][STACK_SIZE-16] = 0xAAAAAAAA;/*R4*/
}
uint8_t osKernelAddThread(void (*task0)(void),void (*task1)(void), void (*task2)(void)){
	/*Disable global Interupt*/
	__disable_irq();
	tcbs[0].nextPt = &tcbs[1];
	tcbs[1].nextPt = &tcbs[2];
	tcbs[2].nextPt = &tcbs[0];
	
	osKernelStackInit(0);
	TCB_STACK[0][STACK_SIZE-2] = (uint32_t)(task0);
	
	osKernelStackInit(1);
	TCB_STACK[1][STACK_SIZE-2] = (uint32_t)(task1);

	osKernelStackInit(2);
	TCB_STACK[2][STACK_SIZE-2] = (uint32_t)(task2);
	
	currentPt = &tcbs[0];
	
	__enable_irq();
	
	return 1;
}
