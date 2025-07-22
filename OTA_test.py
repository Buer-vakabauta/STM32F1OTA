import serial
import time
import sys

def test_single_packet(port, baud):
    """测试单个数据包的发送和接收"""
    print(f"🔌 连接到 {port} @ {baud}")
    
    try:
        ser = serial.Serial(port, baud, timeout=1)
        time.sleep(1)
        
        print("🔮 步骤1: 发送魔术值触发OTA模式")
        magic = bytes([0x55, 0xAA, 0xDE, 0xAD, 0xBE, 0xEF])
        ser.write(magic)
        ser.flush()
        
        # 等待重启和OTA模式消息
        print("⏳ 等待设备重启并进入OTA模式...")
        time.sleep(4)
        
        # 读取并显示所有启动消息
        while ser.in_waiting:
            data = ser.read(ser.in_waiting)
            try:
                text = data.decode('utf-8', errors='ignore')
                print(f"📥 启动消息: {text.strip()}")
            except:
                pass
            time.sleep(0.1)
        
        print("\n📦 步骤2: 发送测试数据包")
        
        # 准备测试数据包
        test_data = b"HELLO WORLD TEST PACKET 123456"
        packet = bytes([0xA5, len(test_data)]) + test_data
        
        print(f"📤 测试包长度: {len(packet)} 字节")
        print(f"📤 起始字节: 0x{packet[0]:02X}")
        print(f"📤 长度字节: 0x{packet[1]:02X} ({packet[1]})")
        print(f"📤 数据: {test_data}")
        print(f"📤 完整包(hex): {packet.hex()}")
        
        # 清空接收缓冲区
        ser.flushInput()
        
        # 发送数据包
        print(f"\n🚀 发送数据包...")
        ser.write(packet)
        ser.flush()
        print(f"✅ 数据包已发送")
        
        # 监听响应
        print(f"\n👂 监听响应 (10秒)...")
        start_time = time.time()
        all_response = b''
        
        while time.time() - start_time < 10:
            if ser.in_waiting:
                data = ser.read(ser.in_waiting)
                all_response += data
                
                # 尝试解码为文本
                try:
                    text = data.decode('utf-8', errors='ignore')
                    if text.strip():
                        print(f"📥 文本响应: {text.strip()}")
                except:
                    pass
                
                # 显示原始十六进制
                print(f"📥 原始数据: {data.hex()}")
                
                # 检查ACK
                if 0x5A in data:
                    print(f"✅ 检测到ACK (0x5A)!")
                
            time.sleep(0.1)
        
        if not all_response:
            print(f"❌ 10秒内未收到任何响应")
        else:
            print(f"\n📊 总响应数据: {len(all_response)} 字节")
            print(f"📊 完整响应(hex): {all_response.hex()}")
        
        print(f"\n🔄 步骤3: 发送几个小的测试包")
        for i in range(3):
            small_data = f"PKT{i}".encode()
            small_packet = bytes([0xA5, len(small_data)]) + small_data
            
            print(f"\n📤 小包{i+1}: {small_packet.hex()}")
            ser.flushInput()
            ser.write(small_packet)
            ser.flush()
            
            # 等待响应
            start_time = time.time()
            while time.time() - start_time < 3:
                if ser.in_waiting:
                    response = ser.read(ser.in_waiting)
                    print(f"📥 小包{i+1}响应: {response.hex()}")
                    if 0x5A in response:
                        print(f"✅ 小包{i+1} ACK OK")
                        break
                time.sleep(0.05)
            else:
                print(f"❌ 小包{i+1} 无响应")
            
            time.sleep(0.5)
    
    except Exception as e:
        print(f"❌ 错误: {e}")
    finally:
        if 'ser' in locals():
            ser.close()
            print("🔌 串口已关闭")

def interactive_test(port, baud):
    """交互式测试"""
    print(f"🎮 交互式测试模式")
    
    try:
        ser = serial.Serial(port, baud, timeout=0.1)
        time.sleep(1)
        
        print("📝 输入命令:")
        print("  'magic' - 发送魔术值")
        print("  'test' - 发送测试包")
        print("  'hex:XXXX' - 发送十六进制数据")
        print("  'quit' - 退出")
        
        while True:
            # 读取并显示串口数据
            if ser.in_waiting:
                data = ser.read(ser.in_waiting)
                try:
                    text = data.decode('utf-8', errors='ignore')
                    if text.strip():
                        print(f"📥 设备: {text.strip()}")
                except:
                    pass
                print(f"📥 原始: {data.hex()}")
            
            # 处理用户输入
            try:
                cmd = input().strip().lower()
                
                if cmd == 'quit':
                    break
                elif cmd == 'magic':
                    magic = bytes([0x55, 0xAA, 0xDE, 0xAD, 0xBE, 0xEF])
                    ser.write(magic)
                    ser.flush()
                    print(f"📤 发送魔术值: {magic.hex()}")
                elif cmd == 'test':
                    test_data = b"INTERACTIVE_TEST"
                    packet = bytes([0xA5, len(test_data)]) + test_data
                    ser.write(packet)
                    ser.flush()
                    print(f"📤 发送测试包: {packet.hex()}")
                elif cmd.startswith('hex:'):
                    hex_str = cmd[4:]
                    try:
                        data = bytes.fromhex(hex_str)
                        ser.write(data)
                        ser.flush()
                        print(f"📤 发送十六进制: {data.hex()}")
                    except ValueError:
                        print("❌ 无效的十六进制格式")
                else:
                    print("❌ 未知命令")
            except EOFError:
                break
            except KeyboardInterrupt:
                break
    
    except Exception as e:
        print(f"❌ 错误: {e}")
    finally:
        if 'ser' in locals():
            ser.close()

def main():
    if len(sys.argv) < 3:
        print("用法: python packet_test.py <端口> <波特率> [模式]")
        print("模式: auto(默认) 或 interactive")
        print("例如: python packet_test.py COM5 9600")
        print("例如: python packet_test.py COM5 9600 interactive")
        sys.exit(1)
    
    port = sys.argv[1]
    baud = int(sys.argv[2])
    mode = sys.argv[3] if len(sys.argv) > 3 else "auto"
    
    print("🧪 STM32 OTA 数据包测试工具")
    print("=" * 50)
    
    if mode == "interactive":
        interactive_test(port, baud)
    else:
        test_single_packet(port, baud)

if __name__ == '__main__':
    main()