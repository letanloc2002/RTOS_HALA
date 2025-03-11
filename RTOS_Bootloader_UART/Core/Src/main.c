#include "stm32f4xx.h"

#define BOOTLOADER_ADDRESS  0x1FFF0000  // Ð?a ch? Bootloader c?a STM32F4
#define MAIN_FLASH_ADDRESS  0x08000000  // Ð?a ch? chuong trình chính, bootloader, OS

#define UART USART1  // S? d?ng UART1 (PA9 = TX, PA10 = RX)

// Khai báo con tr? hàm d? nh?y vào chuong trình khác
typedef void (*JumpToApp_t)(void);

void JumpToAddress(uint32_t address) {
    // T?t ng?t toàn b? tru?c khi nh?y
    __disable_irq();

    // Set l?i MSP theo chuong trình dích
    __set_MSP(*(volatile uint32_t*) address);

    // L?y d?a ch? Reset_Handler() c?a chuong trình dích
    JumpToApp_t JumpToApp = (JumpToApp_t)(*(volatile uint32_t*) (address + 4));

    // Nh?y vào chuong trình
    JumpToApp();
}

int UART_ReceiveCheck(void) {
    uint32_t timeout = 1000000;  // Th?i gian ch? UART (~10 giây)
    
    while (timeout--) {
        if (UART->SR & USART_SR_RXNE) {  // Ki?m tra n?u có d? li?u t? UART
            return 1;  // Ðã nh?n du?c tín hi?u t? UART
        }
    }
    return 0;  // H?t th?i gian ch? mà không có tín hi?u
}

void EnterBootloader(void) {
    // Nh?y vào System Bootloader
    JumpToAddress(BOOTLOADER_ADDRESS);
}

void EnterMainFlash(void) {
    // Nh?y vào chuong trình chính t? Flash
    JumpToAddress(MAIN_FLASH_ADDRESS);
}

int main(void) {
    // C?u hình UART (ch? c?n n?u không dùng CubeMX)
    // UART_Init();

    // Nh?y vào Bootloader
    EnterBootloader();

    // Ch? UART trong 10 giây
    if (UART_ReceiveCheck() == 0) {
        // N?u không có tín hi?u n?p, quay l?i Flash
        EnterMainFlash();
    }

    while (1);
}
