# STM32F1OTA
> 通过脚本实现上位机对stm32MCU的无线烧录更新

### 系统架构

![系统架构](https://github.com/Buer-vakabauta/STM32F1OTA/blob/main/assets/SystemStructure.png)

### 程序流程

![程序流程](https://github.com/Buer-vakabauta/STM32F1OTA/blob/main/assets/process.png)

### 项目配置

1.bootloader 程序默认为16kb,配置IROM1为0x8000000 大小为0x4000

2.APP程序不超过48kb,配置起始地址为0x8004000,大小为0xC000

3.APP程序USER中设置After Build执行'fromelf --bin -o ".\bin_file\@L.bin" "#L"'以在编译后生成bin二进制文件到根目录下的bin_file文件夹用于后续烧录

>  以下代码详见stm32_app_test文件中的相关内容

4.配置USART中断接收'cmd:restart'命令后重启程序以实现远程自动烧录

5.另外需要重定义中断向量(不然无法使用中断),修改startup_stm32fxxx.s中的APP_START_ADDRESS为APP起始地址,并在main()函数开始时赋值SCB->VTOR = APP_VECTOR_ADDR;详见对应文件

6.APP_VECTOR_ADDR;宏定义为起始地址#define APP_VECTOR_ADDR 0x08004000可自定义分配大小(但是注意需要同步修改魔术值的存放地址)

7.esp8266使用Arduino烧录代码实现ESP-NOW透传(每次启动会自动发送MAC地址)

### 烧录脚本

参考代码:

```
python3 OTA_send --port COM5 --baud 115200 --bin ./stm32-app-test/bin_file/projet.bin
```

需要设置的参数有--port 端口和--bin 二进制文件位置

波特率默认为115200

在代码中可修改包大小等其他参数

单个包的大小不能超过255

测试脚本'OTA_test'提供交互式命令用于查看测试传输过程中的问题

###　问题及解决

１.进入OTA模式后发送包接收不到ACK

```
脚本收到OTA信息后做适当延时再发送包(默认延时3s可以更长)
```

2.发送包过程中收不到ACK

```
重新发送或者减小包的体积
```

3.端口无法打开

```
检测端口是否被占用
```



