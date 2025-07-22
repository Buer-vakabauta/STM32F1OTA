//模板工程
//
#include <Delay.h>
#include "OLED.h"
#include "ESP8266.h"
#include "uart.h"
#include <string.h>
#define APP_VECTOR_ADDR 0x08004000

//全局变量
uint8_t flag=0;
float v_angle=190;
float h_angle=70;

// 定义宏
void main_loop(void);
void init();

//初始化
void init()
{	OLED_Init();
    UART_Init(115200);
}
int main()
{	SCB->VTOR = APP_VECTOR_ADDR;//重定义中断向量
    init();
	main_loop();
}
void main_loop(void)
{
	esp_printf("app start");
    while (1){
		OLED_ShowString(1,1,"HELLO OTA!!!");
		if(strlen(uart_buffer)>0&&uart_buffer[0]=='A'){
			esp_printf("1");
		}
    }
}


