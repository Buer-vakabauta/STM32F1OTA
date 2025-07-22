//
// Created by Buer_vakabauta on 2024/10/27.
//
//AT+CWJAP_DEF="Buer_vakabautas","66666666"
//#define single_esp8266 //单esp使用透传模式
//#define esp_now//双esp使用esp_now协议通信
#include "UART.h"
#define BUFFER_MAX_SIZE 64
#include <stdio.h>
#include <stdarg.h>
#include "ESP8266.h"
void esp_printf(const char* format,...){
    char buffer[BUFFER_MAX_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, BUFFER_MAX_SIZE, format, args);
    va_end(args);
    UART_SendString(buffer);
}
