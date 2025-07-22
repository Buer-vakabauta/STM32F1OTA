#include "stm32f10x.h"
#define APP_ADDR         0x08004000
#define FLASH_PAGE_SIZE  1024
#define OTA_MAGIC_FLAG   0x08003C00  // 最后一页地址，用于保存标志
#define OTA_MAGIC_VALUE  0xDEADBEEF
#include "flash_opts.h"
