# 智慧門鎖系統 🔐

使用 ESP32 實現的多功能智慧門鎖系統，支援指紋、RFID 卡片、觸控密碼、人臉辨識等多種驗證方式，並可透過 WiFi 連線遠端控制。

## ✨ 特色功能

- 🖥️ **觸控螢幕介面**：直覺的圖形化選單，輕鬆選擇驗證方式
- 🔢 **數字密碼鍵盤**：3x4完整鍵盤，支援清除和確認操作
- 🔒 **防暴力破解**：連續錯誤3次自動鎖定30秒
- 💾 **密碼持久化**：使用EEPROM存儲，重啟不丟失
- 🎯 **多種驗證方式**：指紋、RFID、密碼、人臉四選一
- 🔧 **易於校準**：內建觸控校準工具和詳細指南
- 📱 **完整文檔**：包含實施文檔、快速開始和校準指南

## 硬體清單
- **MCU**: ESP32-S 開發板
- **指紋傳感器**: AS608 光學指紋模組
- **RFID 讀卡機**: RC522 (13.56MHz)
- **AI 視覺辨識**: HUSKYLENS Pro 哈士奇 AI 辨識鏡頭
- **顯示螢幕**: ILI9341 2.4" TFT LCD (240x320 直立模式) 帶 XPT2046 觸控
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
│   ├── huskylens.cpp   # HUSKYLENS 人臉辨識驅動
│   ├── screen.cpp      # ILI9341 螢幕顯示與觸控驅動 ✨
│   ├── password.cpp    # 密碼管理與驗證 ✨ NEW
│   ├── motor.cpp       # SG90 舵機控制
│   └── wifi_comm.cpp   # ESP32 WiFi 通訊
├── include/            # 標頭檔
│   ├── config.h        # 系統配置與接腳定義 ✨
│   ├── fingerprint.h   # 指紋傳感器介面
│   ├── rfid.h          # RFID 讀卡機介面
│   ├── huskylens.h     # HUSKYLENS 人臉辨識介面
│   ├── screen.h        # 螢幕與觸控控制介面 ✨
│   ├── password.h      # 密碼管理介面 ✨ NEW
│   ├── motor.h         # 舵機控制介面
│   └── wifi_comm.h     # WiFi 通訊介面
├── test/               # 測試程式 ✨ NEW
│   ├── touch_calibration_test.cpp  # 觸控校準測試
│   └── CALIBRATION_GUIDE.md        # 校準指南
├── TOUCHSCREEN_IMPLEMENTATION.md   # 觸控功能實施文檔 ✨ NEW
├── QUICK_START_PASSWORD.md         # 快速開始指南 ✨ NEW
└── lib/                # 第三方函式庫
```

> ✨ 標記表示最近更新或新增的檔案

## 系統流程
1. **系統初始化**: 啟動所有硬體模組（指紋、RFID、HUSKYLENS、螢幕、觸控、密碼、舵機、WiFi）
2. **主選單**: 觸控螢幕選擇驗證方式
   - 🖐️ 指紋辨識
   - 💳 RFID 卡片
   - 🔢 觸控密碼 ✨ NEW
   - 👤 人臉辨識
3. **身份驗證**: 根據選擇的方式進行驗證
   - 指紋：比對資料庫
   - RFID：比對卡片清單
   - 密碼：輸入4-8位數字密碼（預設: 1234）
   - 人臉：HUSKYLENS AI辨識
4. **開鎖動作**: 驗證成功後控制舵機開鎖
5. **自動上鎖**: 5 秒後自動上鎖並回到主選單
6. **遠端通知**: 可選擇透過 WiFi 發送開鎖記錄到伺服器

### 密碼功能特色 ✨
- ✅ 3x4數字鍵盤（0-9、清除、確認）
- ✅ 密碼以星號遮罩顯示
- ✅ 錯誤3次自動鎖定30秒
- ✅ 密碼持久化存儲（EEPROM）
- ✅ 支持4-8位數字密碼

## 開發環境
- **編輯器**: Cursor
- **構建工具**: PlatformIO
- **框架**: Arduino

## 快速開始 ⚡

### 1. 編譯與上傳
```bash
# 編譯專案
pio run

# 編譯並上傳到 ESP32
pio run -t upload

# 查看串口輸出
pio device monitor
```

### 2. 首次使用
1. 啟動後會看到歡迎畫面
2. 進入主選單，有4個選項
3. 點擊「Password」試試密碼解鎖
4. 預設密碼: **1234**
5. 輸入後按 `*` 確認

### 3. 觸控校準（如果觸摸不準）
```bash
# 使用校準測試程式
cp test/touch_calibration_test.cpp src/main.cpp
pio run -t upload
# 按照螢幕提示觸摸標記點
# 根據序列埠輸出調整 config.h 中的校準參數
```

詳細說明見：`QUICK_START_PASSWORD.md`

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
| VCC | 5V | AS608 工作電壓 5V（接 ESP32 VIN 或外部 5V）|
| GND | GND | 接地 |
| TX | GPIO 16 (RX2) | 模組發送 → MCU 接收 |
| RX | GPIO 17 (TX2) | 模組接收 ← MCU 發送 |

### ILI9341 螢幕 + XPT2046 觸控 (SPI)
| 接腳 | ESP32 | 說明 |
|------|-------|------|
| **螢幕部分** |||
| VCC | 3.3V | 電源（有些模組需要 5V，請查閱規格書）|
| GND | GND | 接地 |
| CS | GPIO 5 | 螢幕片選 |
| RESET | GPIO 22 | Reset |
| DC/RS | GPIO 21 | Data/Command |
| MOSI | GPIO 23 | Master Out Slave In（與觸控共用）|
| SCK/CLK | GPIO 18 | Clock（與觸控共用）|
| MISO | GPIO 19 | Master In Slave Out（與觸控共用）|
| LED | 3.3V | 背光（建議串接電阻，或直接接 3.3V）|
| **觸控部分** ✨ |||
| T_CS | GPIO 15 | 觸控片選 |
| T_IRQ | GPIO 2 | 觸控中斷（可選）|
| T_DIN | GPIO 23 | 與螢幕 MOSI 共用 |
| T_DO | GPIO 19 | 與螢幕 MISO 共用 |
| T_CLK | GPIO 18 | 與螢幕 SCK 共用 |

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

### HUSKYLENS Pro AI 辨識鏡頭
| 接腳 | ESP32 | 說明 |
|------|-------|------|
| VCC | 5V | HUSKYLENS 支援 3.3V-5V 電源 |
| GND | GND | 必須與 ESP32 共地 |
| SDA | GPIO 21 | I2C 資料線（建議使用 I2C 模式）|
| SCL | GPIO 22 | I2C 時鐘線 |

> **通訊模式選擇**:
> - **建議使用 I2C 模式**（預設地址 0x32），因為 UART2 已被指紋模組佔用
> - 如果要用 UART 模式，可使用 UART1 或軟體串口，並在 HUSKYLENS 設定中切換模式
> - I2C 腳位與螢幕的 DC/RST 腳位相同號碼但功能不同，不會衝突

## 功能模組

### ✅ 已完成
- [x] 專案架構建立
- [x] 接腳配置規劃
- [x] 程式骨架與介面定義
- [x] 狀態機流程設計
- [x] **ILI9341 螢幕顯示與 UI** ✨
- [x] **XPT2046 觸控螢幕支持** ✨
- [x] **觸控數字鍵盤介面** ✨
- [x] **密碼管理系統**（驗證、存儲、修改）✨
- [x] **主選單介面**（4種驗證方式選擇）✨
- [x] **防暴力破解機制**（錯誤3次鎖定30秒）✨
- [x] **觸控校準工具** ✨

### 🚧 待實作（由你自己完成）
- [ ] AS608 指紋傳感器通訊協定
- [ ] RC522 RFID 讀寫功能
- [ ] HUSKYLENS 人臉辨識功能
- [ ] SG90 舵機角度控制優化
- [ ] ESP32 WiFi 連線與遠端通訊
- [ ] 使用者資料庫（指紋 ID、卡片 UID、人臉 ID）
- [ ] 密碼修改介面（長按進入設定模式）
- [ ] 解鎖日誌記錄
- [ ] 系統整合測試

## 學習建議

這是一個新手友善的專案架構，已經實作了觸控密碼系統作為範例：

### 已實作的參考範例 ✨
1. **觸控密碼系統**：完整可用的參考實作
   - 學習如何整合硬體（螢幕、觸控）
   - 學習如何設計UI介面（鍵盤、選單）
   - 學習如何管理持久化數據（EEPROM）
   - 學習如何實現防暴力破解

### 建議實作順序
1. **先試用密碼功能**：了解系統運作流程
2. **從 SG90 舵機開始**：最簡單，先讓馬達動起來
3. **接著做指紋或 RFID**：參考密碼系統的架構
4. **然後做 HUSKYLENS**：最直覺，可以看到辨識結果
5. **最後整合 WiFi**：進階功能，遠端控制

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

# 或在 VSCode/Cursor 中重新載入視窗
# Cmd+Shift+P → "Reload Window"
```

### 觸控除錯 ✨
如果觸摸無反應或位置不準：

1. **檢查接線**
   ```bash
   # 查看序列埠輸出
   pio device monitor
   # 應該看到 "初始化觸控完成"
   ```

2. **測試原始觸控數據**
   ```cpp
   // 在 loop() 中臨時添加
   display.printTouchDebug();
   ```

3. **執行校準程式**
   ```bash
   # 詳見 test/CALIBRATION_GUIDE.md
   cp test/touch_calibration_test.cpp src/main.cpp
   pio run -t upload
   ```

4. **常見問題**
   - 觸摸無反應 → 檢查 T_CS (GPIO15) 接線
   - 位置偏移 → 調整 config.h 校準參數
   - 重複觸發 → 增加防抖延遲

## 參考資料

### 已使用的函式庫 ✨
- **Adafruit_ILI9341**: 螢幕驅動（已整合）
- **XPT2046_Touchscreen**: 觸控驅動（已整合）
- **Preferences**: ESP32 EEPROM存儲（已整合）

### 待實作模組可參考
- **AS608**: 查詢通訊協定手冊
- **RC522**: MFRC522 函式庫文件
- **HUSKYLENS**: HUSKYLENS 函式庫（在 PlatformIO 安裝 `huskylens` 函式庫）
- **ESP32 WiFi**: WiFi.h 函式庫（ESP32 內建）
- **ESP32Servo**: 已安裝，參考範例程式

### 專案文檔
- 📄 **TOUCHSCREEN_IMPLEMENTATION.md**: 觸控功能完整技術文檔
- 📄 **QUICK_START_PASSWORD.md**: 快速開始指南
- 📄 **test/CALIBRATION_GUIDE.md**: 觸控校準指南

## 編譯狀態

✅ **最新編譯成功** (2026-01-25)
- RAM使用: 13.7% (44,772 / 327,680 bytes)
- Flash使用: 58.9% (772,397 / 1,310,720 bytes)

## 更新日誌

### v1.1.0 (2026-01-25) ✨
- ✅ 新增觸控螢幕支持（XPT2046驅動）
- ✅ 新增密碼驗證系統（4-8位數字）
- ✅ 新增主選單UI（4個按鈕選擇驗證方式）
- ✅ 新增3x4數字鍵盤介面
- ✅ 新增密碼持久化存儲（EEPROM）
- ✅ 新增防暴力破解機制（錯誤3次鎖定30秒）
- ✅ 新增觸控校準工具和指南
- ✅ 更新狀態機，整合密碼輸入流程
- ✅ 添加完整技術文檔和快速開始指南

### v1.0.0 (初始版本)
- ✅ 專案架構建立
- ✅ 基礎硬體模組框架
- ✅ 接腳配置規劃
- ✅ 狀態機設計

## 授權

本專案為教育用途，歡迎學習和修改。

## 貢獻

歡迎提交 Issue 和 Pull Request！

---

**開發環境**: Cursor + PlatformIO  
**最後更新**: 2026-01-25  
**狀態**: ✅ 可編譯運行
