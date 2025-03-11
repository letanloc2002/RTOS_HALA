/**************************************************************************
 * @file    ring_buffer.c
 * @brief   Thuc hien cac thao tac tren bo dem vong (ring buffer)
 * @details T?p tin n�y ch?a c�c h�m d? kh?i t?o, th�m, l?y gi� tr?,
 *          v� ki?m tra tr?ng th�i c?a m?t bo dem vong. Bo dem vong
 *          l� m?t c?u tr�c d? li?u h?u �ch trong l?p tr�nh nh�ng v�
 *          c�c ?ng d?ng y�u c?u qu?n l� b? nh? hi?u qu?.
 *          C�c ch?c nang bao g?m:
 *          - Khoi tao bo dem vong
 *          - Them gi� tr? v�o bo dem
 *          - Lay gi� tr? t? bo dem
 *          - Kiem tra xem bo dem co rong hay day
 *
 * @author  Nguyen Van Thuong
 * @date    04/11/2024
 **************************************************************************/

#include "ring_buffer.h"

/**************************************************************************
 * @brief   Khoi tao bo dem vong
 * @details Ham nay khoi tao bo dem vong voi con tro va kich thuoc duoc
 *          chi dinh. Neu tham so khong hop le, ham se tra ve RING_FAIL.
 * @param   ringBuff     Con tro den bo dem vong can khoi tao
 * @param   ptr_ringBuff Con tro den mang dung bo dem
 * @param   max_size     Kich thuoc toi da cua bo dem
 * @return  RINGBUFF_STT_t Tra ve RING_OK neu thanh cong, nguoc lai RING_FAIL
 **************************************************************************/
RINGBUFF_STT_t RINGBUFF_Init(RINGBUFF_TypeDef *ringBuff, uint8_t *ptr_ringBuff, uint32_t max_size)
{
    if (ringBuff == NULL || ptr_ringBuff == NULL || max_size < 2)
    {
        return RING_FAIL; // Tham so khong hop le
    }

    ringBuff->ptrRingBuff = ptr_ringBuff;
    ringBuff->head = 0;
    ringBuff->tail = 0;
    ringBuff->size = max_size;

    return RING_OK; // Khoi tao thanh cong
}

/**************************************************************************
 * @brief   Them gia tri vao bo dem vong
 * @details Ham nay them mot gia tri vao bo dem vong. Neu bo dem day,
 *          ham se tra ve RING_FAIL.
 * @param   ringBuff Con tro den bo dem vong
 * @param   value    Gia tri can them vao bo dem
 * @return  RINGBUFF_STT_t Tra ve RING_OK neu thanh cong, nguoc lai RING_FAIL
 **************************************************************************/
RINGBUFF_STT_t RINGBUFF_Put(RINGBUFF_TypeDef *ringBuff, uint8_t value)
{
    uint32_t nextHead = (ringBuff->head + 1) % ringBuff->size;

    if (nextHead == ringBuff->tail)
    {
        return RING_FAIL; // Bo dem day
    }

    ringBuff->ptrRingBuff[ringBuff->head] = value;
    ringBuff->head = nextHead;

    return RING_OK; // Them thanh cong
}

/**************************************************************************
 * @brief   Lay gia tri tu bo dem vong
 * @details Ham nay lay mot gia tri tu bo dem vong. Neu bo dem rong,
 *          ham se tra ve RING_FAIL.
 * @param   ringBuff Con tro den bo dem vong
 * @param   value    Con tro den gia tri lay ra
 * @return  RINGBUFF_STT_t Tra ve RING_OK neu thanh cong, nguoc lai RING_FAIL
 **************************************************************************/
RINGBUFF_STT_t RINGBUFF_Get(RINGBUFF_TypeDef *ringBuff, uint8_t *value)
{
    if (ringBuff->head == ringBuff->tail)
    {
        return RING_FAIL; // Bo dem rong
    }

    *value = ringBuff->ptrRingBuff[ringBuff->tail];
    ringBuff->tail = (ringBuff->tail + 1) % ringBuff->size;

    return RING_OK; // Lay thanh cong
}

/**************************************************************************
 * @brief   Kiem tra bo dem co rong khong
 * @details Ham nay tra ve true neu bo dem rong, nguoc lai tra ve false.
 * @param   ringBuff Con tro den bo dem vong
 * @return  bool Tra ve true neu rong, false neu khong
 **************************************************************************/
bool RINGBUFF_IsEmpty(RINGBUFF_TypeDef *ringBuff)
{
    return (ringBuff->head == ringBuff->tail); // Kiem tra tinh trang rong
}

/**************************************************************************
 * @brief   Kiem tra bo dem co day khong
 * @details Ham nay tra ve true neu bo dem day, nguoc lai tra ve false.
 * @param   ringBuff Con tro den bo dem vong
 * @return  bool Tra ve true neu day, false neu khong
 **************************************************************************/
bool RINGBUFF_IsFull(RINGBUFF_TypeDef *ringBuff)
{
    uint32_t nextHead = (ringBuff->head + 1) % ringBuff->size;
    return (nextHead == ringBuff->tail); // Kiem tra tinh trang day
}
