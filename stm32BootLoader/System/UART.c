//
// Created by Buer_vakabauta on 2024/10/30.
//
#include "UART.h"
//#include "cstring"
#include "string.h"

#define BUFFER_SIZE 100



void UART_Init(uint32_t baud_rate) {
    // 1. 启用 GPIOA 和 USART1 的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
;
    // 2. 配置 PA9（TX）和 PA10（RX）
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // TX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // RX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 3. 配置 USART 参数
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = baud_rate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);


    // 4. 使能 USART
    USART_Cmd(USART1, ENABLE);
}


void UART_SendChar(char c) {
    USART_SendData(USART1, (uint16_t)c);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); // 等待发送完成
}

void UART_SendString(const char *str) {
    while (*str) {
        UART_SendChar(*str++);
    }
	UART_SendChar('\n');
}

char UART_ReceiveChar(void) {
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET); // 等待数据接收
    return (char)USART_ReceiveData(USART1);
}



uint8_t UART_hasData(void) {
    return USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET;
}

