#include "stm32f4xx.h"

#define BOOTLOADER_ADDRESS  0x1FFF0000  // �?a ch? Bootloader c?a STM32F4
#define MAIN_FLASH_ADDRESS  0x08000000  // �?a ch? chuong tr�nh ch�nh, bootloader, OS

#define UART USART1  // S? d?ng UART1 (PA9 = TX, PA10 = RX)

// Khai b�o con tr? h�m d? nh?y v�o chuong tr�nh kh�c
typedef void (*JumpToApp_t)(void);

void JumpToAddress(uint32_t address) {
    // T?t ng?t to�n b? tru?c khi nh?y
    __disable_irq();

    // Set l?i MSP theo chuong tr�nh d�ch
    __set_MSP(*(volatile uint32_t*) address);

    // L?y d?a ch? Reset_Handler() c?a chuong tr�nh d�ch
    JumpToApp_t JumpToApp = (JumpToApp_t)(*(volatile uint32_t*) (address + 4));

    // Nh?y v�o chuong tr�nh
    JumpToApp();
}

int UART_ReceiveCheck(void) {
    uint32_t timeout = 1000000;  // Th?i gian ch? UART (~10 gi�y)
    
    while (timeout--) {
        if (UART->SR & USART_SR_RXNE) {  // Ki?m tra n?u c� d? li?u t? UART
            return 1;  // �� nh?n du?c t�n hi?u t? UART
        }
    }
    return 0;  // H?t th?i gian ch? m� kh�ng c� t�n hi?u
}

void EnterBootloader(void) {
    // Nh?y v�o System Bootloader
    JumpToAddress(BOOTLOADER_ADDRESS);
}

void EnterMainFlash(void) {
    // Nh?y v�o chuong tr�nh ch�nh t? Flash
    JumpToAddress(MAIN_FLASH_ADDRESS);
}

int main(void) {
    // C?u h�nh UART (ch? c?n n?u kh�ng d�ng CubeMX)
    // UART_Init();

    // Nh?y v�o Bootloader
    EnterBootloader();

    // Ch? UART trong 10 gi�y
    if (UART_ReceiveCheck() == 0) {
        // N?u kh�ng c� t�n hi?u n?p, quay l?i Flash
        EnterMainFlash();
    }

    while (1);
}
