#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include "stm32f10x.h"
#include "stddef.h"
#include "stdbool.h"

typedef enum {
    RING_FAIL = -1,
    RING_OK = 0,
} RINGBUFF_STT_t;

typedef struct {
    volatile uint32_t head;
    volatile uint32_t tail;  
    uint32_t size;          
    volatile uint8_t *ptrRingBuff;
} RINGBUFF_TypeDef;

RINGBUFF_STT_t RINGBUFF_Init(RINGBUFF_TypeDef *ringBuff, uint8_t *ptr_ringBuff, uint32_t max_size);
RINGBUFF_STT_t RINGBUFF_Put(RINGBUFF_TypeDef *ringBuff, uint8_t value);
RINGBUFF_STT_t RINGBUFF_Get(RINGBUFF_TypeDef *ringBuff, uint8_t *value);

bool RINGBUFF_IsEmpty(RINGBUFF_TypeDef *ringBuff);
bool RINGBUFF_IsFull(RINGBUFF_TypeDef *ringBuff);
#endif /* RING_BUFFER_H_*/


script = kich 

- project