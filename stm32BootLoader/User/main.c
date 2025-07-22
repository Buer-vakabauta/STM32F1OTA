#include "stm32f10x.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "ESP8266.h"
#include "UART.h"
#define APP_ADDR         0x08004000
#define FLASH_PAGE_SIZE  1024
#define OTA_MAGIC_FLAG   0x08003C00  // 最后一页地址，用于保存标志
#define OTA_MAGIC_VALUE  0xDEADBEEF
typedef void (*pFunction)(void);  // 函数指针类型定义
// 接收缓冲区
uint8_t recv_buf[256];

void Flash_EraseAppArea(void);
void Flash_Write(uint32_t addr, uint8_t* data, uint16_t len);
void JumpToApp(void);


volatile uint32_t systick_ms = 0;



void SysTick_Init(void) {
    SysTick_Config(SystemCoreClock / 1000);  // 1ms节拍
}

uint32_t SysTick_GetTick(void) {
    return systick_ms;
}

void main(void) {
	SysTick_Init();
    UART_Init(115200);
    esp_printf("Bootloader start...");
	
	 // 限时监听触发命令（1秒）
    uint32_t tick_start = SysTick_GetTick(); 
    uint8_t trigger_buf[6] = {0};
    uint8_t index = 0;

    esp_printf("Waiting for OTA trigger (1s)...");

    while ((SysTick_GetTick() - tick_start) < 1000) {
        if (UART_hasData()) {
            uint8_t ch = UART_ReceiveChar();
            trigger_buf[index++] = ch;
            if (index == 6) {
                if (memcmp(trigger_buf, (uint8_t[]){0x55, 0xAA, 0xDE, 0xAD, 0xBE, 0xEF}, 6) == 0) {
                    esp_printf("OTA trigger matched. Setting flag and restarting...");
                    FLASH_Unlock();
                    FLASH_ErasePage(OTA_MAGIC_FLAG);
                    FLASH_ProgramWord(OTA_MAGIC_FLAG, OTA_MAGIC_VALUE);
                    FLASH_Lock();
                    NVIC_SystemReset();  // 重启
                }
                index = 0;
            }
        }
    }
	
	
	
    uint32_t magic = *(volatile uint32_t*)OTA_MAGIC_FLAG;
    if (magic != OTA_MAGIC_VALUE) {
        esp_printf("No update flag. Jumping to App.");
		USART_Cmd(USART1, DISABLE);
		USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        JumpToApp();
    }

    esp_printf("OTA update mode.");

    Flash_EraseAppArea();

    uint32_t app_addr = APP_ADDR;
while (1) {
    if (UART_hasData()) {
        uint8_t start = UART_ReceiveChar();
        if (start != 0xA5) continue;
        
        // 等待长度字节
        while (!UART_hasData()) { /* 可以加超时 */ }
        uint8_t length = UART_ReceiveChar();
         if (length == 0xFF) {
            esp_printf("OTA complete. Jumping to App...");

            FLASH_Unlock();
            FLASH_ErasePage(OTA_MAGIC_FLAG);  // 清除升级标志，防止下次误进OTA
            FLASH_Lock();
            UART_SendChar(0x5A);
            JumpToApp();
        }
        if (length > sizeof(recv_buf)) continue;
        
        // 接收数据
        for (uint8_t i = 0; i < length; i++) {
            while (!UART_hasData()) { /* 可以加超时 */ }
            recv_buf[i] = UART_ReceiveChar();
        }
        
        Flash_Write(app_addr, recv_buf, length);
        app_addr += length;
        UART_SendChar(0x5A);  // 发送 ACK
    }
}

}

void JumpToApp(void) {
    uint32_t app_sp = *(__IO uint32_t*)APP_ADDR;
    uint32_t app_entry = *(__IO uint32_t*)(APP_ADDR + 4);
    pFunction app_func = (pFunction)app_entry;

    __disable_irq();
    __set_MSP(app_sp);
    app_func();
}

void Flash_EraseAppArea(void) {
    FLASH_Unlock();

    for (uint32_t addr = APP_ADDR; addr < 0x08010000; addr += FLASH_PAGE_SIZE) {
        FLASH_ErasePage(addr);
    }

    FLASH_ErasePage(OTA_MAGIC_FLAG);  // 清除升级标志
    FLASH_Lock();
}

void Flash_Write(uint32_t addr, uint8_t* data, uint16_t len) {
    FLASH_Unlock();
    for (uint16_t i = 0; i < len; i += 2) {
        uint16_t halfword = data[i];
        if (i + 1 < len) {
            halfword |= data[i + 1] << 8;
        } else {
            halfword |= 0xFF00;  // 填充偶数
        }
        FLASH_ProgramHalfWord(addr + i, halfword);
    }
    FLASH_Lock();
}
