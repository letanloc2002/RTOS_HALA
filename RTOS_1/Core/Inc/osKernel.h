#include "stm32f446xx.h"


uint8_t osKernelAddThread(void (*task0)(void),void (*task1)(void), void (*task2)(void));
void osKernelInit(void);
void osKernelLaunch(uint32_t quanta);
