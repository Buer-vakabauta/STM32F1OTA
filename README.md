# STM32F1OTA
> 通过脚本实现上位机对stm32MCU的无线烧录更新

### 系统架构

![系统架构](https://github.com/Buer-vakabauta/STM32F1OTA/blob/main/assets/SystemStructure.png)

### 程序流程

![程序流程](https://github.com/Buer-vakabauta/STM32F1OTA/blob/main/assets/process.png)

### 所需配置

bootloader 默认为16kb,配置IROM1为0x8000000 大小为0x4000

APP程序不超过48kb,配置起始地址为0x8004000,大小为0xC000以及USER中设置After Build

'fromelf --bin -o ".\bin_file\@L.bin" "#L'

以在编译后生成bin二进制文件到bin_file用于后续烧录

以下代码详见stm32_app_test文件中的相关内容

配置USART中断接收'cmd:restart'命令后重启程序以实现远程自动烧录

另外需要重定义中断向量(不然无法使用中断),修改startup_stm32fxxx.s中的APP_START_ADDRESS为APP起始地址,并在main()函数开始时赋值SCB->VTOR = APP_VECTOR_ADDR;

APP_VECTOR_ADDR;宏定义为起始地址#define APP_VECTOR_ADDR 0x08004000

### 烧录脚本

代码注释中有详细的参数说明

需要设置的参数有--port 端口和--bin 二进制文件位置

波特率默认为115200

可自行修改其他配置.

单个包的大小不能超过255

测试脚本提供交互式命令用于查看测试传输过程中的问题



