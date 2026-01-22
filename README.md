# 智慧門鎖系統

使用 ESP32、AS608 指紋傳感器、ILI9341 螢幕和 SG90 舵機實現的智慧門鎖系統。

## 硬體清單
- **MCU**: ESP32-S 開發板
- **指紋傳感器**: AS608
- **顯示螢幕**: ILI9341 (2.4" TFT)
- **舵機**: SG90 (控制門鎖)

## 專案結構
```
smart_lock/
├── platformio.ini      # PlatformIO 專案配置
├── src/                # 主程式碼
│   ├── main.cpp        # 主程式入口
│   ├── fingerprint.cpp # 指紋傳感器控制實作
│   ├── screen.cpp      # 螢幕控制實作
│   └── motor.cpp       # 舵機控制實作
├── include/            # 標頭檔
│   ├── fingerprint.h   # 指紋傳感器控制介面
│   ├── screen.h        # 螢幕控制介面
│   └── motor.h         # 舵機控制介面
└── lib/                # 自訂函式庫（選用）
```

## 系統流程
1. **初始化**: 啟動硬體模組
2. **等待指紋**: 顯示提示訊息，等待使用者放置指紋
3. **驗證**: 比對指紋資料庫
4. **開鎖**: 驗證成功後控制舵機開鎖
5. **自動上鎖**: 5秒後自動上鎖

## 開發環境
- **編輯器**: Cursor
- **構建工具**: PlatformIO
- **框架**: Arduino

## 編譯與上傳
```bash
# 編譯專案
pio run

# 編譯並上傳到 ESP32
pio run --target upload

# 查看串口輸出
pio device monitor
```

## 接腳定義
### AS608 指紋傳感器
- VCC -> 3.3V
- GND -> GND
- TX -> RX2 (GPIO 16)
- RX -> TX2 (GPIO 17)

### ILI9341 螢幕
- VCC -> 3.3V
- GND -> GND
- CS -> GPIO 5
- RESET -> GPIO 4
- DC -> GPIO 2
- MOSI -> GPIO 23
- SCK -> GPIO 18
- LED -> 3.3V

### SG90 舵機
- VCC -> 5V
- GND -> GND
- Signal -> GPIO 13

## 開發狀態
- [x] 專案架構建立
- [ ] AS608 驅動實作
- [ ] ILI9341 驅動實作
- [ ] SG90 控制實作
- [ ] 整合測試
