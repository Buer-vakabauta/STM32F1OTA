#include <ESP8266WiFi.h>
#include <espnow.h>

// 对方ESP8266的MAC地址 (需要替换为实际的MAC地址)
uint8_t peerMAC[] = {0xF4, 0xCF, 0xA2, 0xE9, 0x6A, 0x06}; // 替换为对方MAC
//0xC4, 0xD8, 0xD5, 0x0B, 0x7F, 0x4D
//0xF4, 0xCF, 0xA2, 0xE9, 0x6A, 0x06
// 数据结构 - ESP8266 ESP-NOW最大支持约250字节
typedef struct struct_message {
    uint8_t data[240];  // 二进制数据缓冲区
    uint8_t length;     // 实际数据长度
    uint16_t messageId; // 消息ID
} struct_message;

struct_message myData;
struct_message receivedData;
uint16_t messageCounter = 0;

// 串口缓冲区
uint8_t serialBuffer[240];
int bufferIndex = 0;

// ESP-NOW发送回调函数 (ESP8266版本)
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
    if (sendStatus != 0) {
        Serial.println("E1:no device find."); // 发送失败
    }
}

// ESP-NOW接收回调函数 (ESP8266版本)
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    
    // 将接收到的二进制数据原样通过串口发送给电脑
    Serial.write(receivedData.data, receivedData.length);
    Serial.flush(); // 确保数据立即发送
}

void setup() {
    Serial.begin(115200); // 提高波特率以支持更快的数据传输
    Serial.setTimeout(10); // 减少串口超时时间
    
    // 设置WiFi为Station模式
    WiFi.mode(WIFI_STA);
    
    // 打印本机MAC地址
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
    
    // 初始化ESP-NOW
    if (esp_now_init() != 0) {
        Serial.println("E2:esp-now init failed.");
        return;
    }
    
    // 设置ESP-NOW角色
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    
    // 注册发送和接收回调函数
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
    
    // 添加对方为对等设备 (ESP8266版本)
    if (esp_now_add_peer(peerMAC, ESP_NOW_ROLE_COMBO, 1, NULL, 0) != 0) {
        Serial.println("E3:Failed to add peer");
        return;
    }
    
    Serial.println("OK");
}

void loop() {
    static unsigned long lastDataTime = 0;
    
    // 检查是否有来自电脑串口的数据
    if (Serial.available()) {
        // 读取所有可用数据
        while (Serial.available() && bufferIndex < 240) {
            serialBuffer[bufferIndex] = Serial.read();
            bufferIndex++;
            lastDataTime = millis(); // 更新最后接收数据的时间
        }
        
        // 如果缓冲区满了，立即发送
        if (bufferIndex >= 240) {
            sendBufferedData();
        }
    }
    
    // 如果有数据在缓冲区中，并且超过一定时间没有新数据，则发送
    if (bufferIndex > 0 && (millis() - lastDataTime > 2)) {
        sendBufferedData();
    }
    
    // 短暂延时避免占用过多CPU
    delayMicroseconds(100);
}

void sendBufferedData() {
    if (bufferIndex == 0) return;
    
    // 准备发送的数据
    memcpy(myData.data, serialBuffer, bufferIndex);
    myData.length = bufferIndex;
    myData.messageId = messageCounter++;
    
    // 发送二进制数据 (ESP8266版本)
    int result = esp_now_send(peerMAC, (uint8_t *) &myData, sizeof(myData));
    
    if (result != 0) {
        Serial.println("E4:Send failed");
    }
    
    // 清空缓冲区
    bufferIndex = 0;
}