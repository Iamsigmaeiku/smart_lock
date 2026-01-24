# 智慧門鎖系統

使用 ESP32 實現的多功能智慧門鎖系統，支援指紋、RFID 卡片驗證，並可透過 WiFi 連線遠端控制。

## 硬體清單
- **MCU**: ESP32-S 開發板
- **指紋傳感器**: AS608 光學指紋模組
- **RFID 讀卡機**: RC522 (13.56MHz)
- **顯示螢幕**: ILI9341 (2.4" TFT LCD)
- **舵機**: SG90 (控制門鎖機構)

## 專案結構
```
smart_lock/
├── platformio.ini      # PlatformIO 專案配置
├── README.md           # 專案說明文件
├── src/                # 主程式碼
│   ├── main.cpp        # 主程式入口與狀態機
│   ├── fingerprint.cpp # AS608 指紋傳感器驅動
│   ├── rfid.cpp        # RC522 RFID 讀卡機驅動
│   ├── screen.cpp      # ILI9341 螢幕顯示驅動
│   ├── motor.cpp       # SG90 舵機控制
│   └── wifi_comm.cpp   # ESP32 WiFi 通訊
├── include/            # 標頭檔
│   ├── config.h        # 系統配置與接腳定義
│   ├── fingerprint.h   # 指紋傳感器介面
│   ├── rfid.h          # RFID 讀卡機介面
│   ├── screen.h        # 螢幕控制介面
│   ├── motor.h         # 舵機控制介面
│   └── wifi_comm.h     # WiFi 通訊介面
└── lib/                # 第三方函式庫
```

## 系統流程
1. **系統初始化**: 啟動所有硬體模組（指紋、RFID、螢幕、舵機、WiFi）
2. **等待輸入**: 顯示提示訊息，等待使用者驗證
   - 指紋感應
   - RFID 卡片感應
3. **身份驗證**: 比對指紋資料庫或 RFID 卡片清單
4. **開鎖動作**: 驗證成功後控制舵機開鎖
5. **自動上鎖**: 5 秒後自動上鎖並回到等待狀態
6. **遠端通知**: 可選擇透過 WiFi 發送開鎖記錄到伺服器

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

> **注意**: 所有接腳定義集中在 `include/config.h`，方便統一管理

### SG90 舵機
| 接腳 | ESP32 | 說明 |
|------|-------|------|
| 紅色 (VCC) | 5V | ⚠️ 建議接 ESP32 VIN 或外部 5V 電源 |
| 棕色 (GND) | GND | 必須與 ESP32 共地 |
| 橘色 (Signal) | GPIO 13 | PWM 訊號 |

### AS608 指紋傳感器
| 接腳 | ESP32 | 說明 |
|------|-------|------|
| VCC | 3.3V | AS608 核心電壓 3.3V |
| GND | GND | 接地 |
| TX | GPIO 16 (RX2) | 模組發送 → MCU 接收 |
| RX | GPIO 17 (TX2) | 模組接收 ← MCU 發送 |

### ILI9341 螢幕 (SPI)
| 接腳 | ESP32 | 說明 |
|------|-------|------|
| VCC | 3.3V | 電源（有些模組需要 5V，請查閱規格書）|
| GND | GND | 接地 |
| CS | GPIO 5 | Chip Select |
| RESET | GPIO 22 | Reset |
| DC/RS | GPIO 21 | Data/Command |
| MOSI | GPIO 23 | Master Out Slave In |
| SCK/CLK | GPIO 18 | Clock |
| MISO | GPIO 19 | Master In Slave Out（選用）|
| LED | 3.3V | 背光（建議串接電阻，或直接接 3.3V）|

### RFID RC522 (SPI - 與螢幕共用部分接腳)
| 接腳 | ESP32 | 說明 |
|------|-------|------|
| VCC | 3.3V | ⚠️ 嚴禁接 5V，否則會燒毀 RC522 |
| GND | GND | 必須與 ESP32 共地 |
| RST | GPIO 27 | 重置腳。選用 27 是因為它通常在開發板上離 SPI 介面較近，方便走線 |
| IRQ | 不接 | 中斷腳。在輪詢 (Polling) 模式下通常不需要，懸空即可 |
| MISO | GPIO 19 | Master In Slave Out (與螢幕並聯) |
| MOSI | GPIO 23 | Master Out Slave In (與螢幕並聯) |
| SCK | GPIO 18 | Clock (與螢幕並聯) |
| SDA (SS) | GPIO 4 | Chip Select。這是關鍵。螢幕用 GPIO 5，所以我們分配 GPIO 4 給 RFID |

## 功能模組

### ✅ 已完成
- [x] 專案架構建立
- [x] 接腳配置規劃
- [x] 程式骨架與介面定義
- [x] 狀態機流程設計

### 🚧 待實作（由你自己完成）
- [ ] AS608 指紋傳感器通訊協定
- [ ] RC522 RFID 讀寫功能
- [ ] ILI9341 螢幕顯示與 UI
- [ ] SG90 舵機角度控制優化
- [ ] ESP32 WiFi 連線與遠端通訊
- [ ] 使用者資料庫（指紋 ID、卡片 UID）
- [ ] 系統整合測試

## 學習建議

這是一個新手友善的專案架構，所有模組都只提供骨架，讓你自己實作：

1. **從 SG90 舵機開始**：最簡單，先讓馬達動起來
2. **接著做螢幕顯示**：視覺化回饋，方便除錯
3. **然後做指紋或 RFID**：二選一先實作
4. **最後整合 WiFi**：進階功能

## 除錯技巧

### 編譯專案
```bash
pio run
```
- 紅線不一定是錯誤，實際編譯才準確
- 看懂編譯器的錯誤訊息很重要

### 重建 IDE 索引
如果出現誤報紅線：
```bash
# 刪除快取
rm -rf .cache

# 或在 VSCode 中重新載入視窗
# Cmd+Shift+P → "Reload Window"
```

## 參考資料

建議查閱以下資料來實作各模組：

- **AS608**: 查詢通訊協定手冊
- **RC522**: MFRC522 函式庫文件
- **ILI9341**: Adafruit_ILI9341 或 TFT_eSPI 函式庫
- **ESP32 WiFi**: WiFi.h 函式庫（ESP32 內建）
- **ESP32Servo**: 已安裝，參考範例程式
