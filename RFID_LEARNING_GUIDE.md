# RFID RC522 學習指南 - 從第一性原理出發

## 📖 目錄

1. [學習前準備](#學習前準備)
2. [第 0 階段：基礎知識](#第-0-階段基礎知識)
3. [第 1 階段：Datasheet 導讀](#第-1-階段datasheet-導讀)
4. [第 2 階段：實驗式學習](#第-2-階段實驗式學習)
5. [第 3 階段：深入原始碼](#第-3-階段深入原始碼)
6. [第 4 階段：實作專案](#第-4-階段實作專案)
7. [除錯指南](#除錯指南)
8. [參考資源](#參考資源)

---

## 學習前準備

### 硬體需求
- ✅ ESP32 開發板
- ✅ RC522 RFID 模組
- ✅ MIFARE Classic 1K 卡片（至少 2 張）
- ✅ 麵包板與杜邦線
- 🔧 示波器或邏輯分析儀（選配，用於觀察波形）

### 軟體需求
- ✅ PlatformIO（已安裝）
- ✅ Serial Monitor（觀察輸出）
- ✅ MFRC522 函式庫（已加入 platformio.ini）

### 接線檢查

| ESP32 Pin | RC522 Pin | 說明 |
|-----------|-----------|------|
| GPIO 23   | MOSI      | 主出從入 |
| GPIO 25   | MISO      | 主入從出 |
| GPIO 19   | SCK       | 時鐘 |
| GPIO 22   | SDA/CS    | 片選 |
| 3.3V      | VCC       | **注意：不是 5V！** |
| GND       | GND       | 接地 |
| -         | RST       | 重置（可選，本專案未使用） |
| -         | IRQ       | 中斷（可選，本專案未使用） |

⚠️ **重要提醒**：
- RC522 **只能用 3.3V**，使用 5V 會燒毀！
- SPI 線建議 < 10cm，過長會導致通訊不穩定
- 確保 GND 良好接地

---

## 第 0 階段：基礎知識

### 1. RFID 工作原理（第一性原理）

```
電磁感應定律（Faraday's Law）
↓
Reader 產生 13.56MHz 交變磁場
↓
卡片線圈感應產生電流（φ = B·A·cos(θ)）
↓
整流電路將交流轉直流供電給卡片晶片
↓
卡片晶片改變線圈負載阻抗
↓
Reader 偵測到阻抗變化解調出資料（Load Modulation）
```

**關鍵物理量**：
- 頻率：13.56 MHz（ISM 頻段）
- 工作距離：0-10 cm（取決於天線設計與功率）
- 能量傳輸：約 100 mW
- 調變方式：ASK（Amplitude Shift Keying，幅移鍵控）

### 2. SPI 通訊協議

```
時序圖：
     SCK  __|‾|_|‾|_|‾|_|‾|_|‾|_|‾|_|‾|_|‾|__
     CS   ‾‾|__________________________|‾‾‾‾
     MOSI --|  B7 B6 B5 B4 B3 B2 B1 B0 |----
     MISO --|  B7 B6 B5 B4 B3 B2 B1 B0 |----
```

**RC522 SPI 特性**：
- 模式：Mode 0（CPOL=0, CPHA=0）
- 時鐘：最高 10 MHz
- 位元順序：MSB first
- 讀取：地址 bit 7 = 1
- 寫入：地址 bit 7 = 0

**範例：讀取版本暫存器**
```cpp
// 版本暫存器地址 = 0x37
// 讀取地址 = (0x37 << 1) | 0x80 = 0xEE

digitalWrite(CS, LOW);
SPI.transfer(0xEE);      // 發送讀取命令
byte version = SPI.transfer(0x00);  // 讀取資料
digitalWrite(CS, HIGH);

// version 應該是 0x91 或 0x92
```

### 3. ISO 14443A 協議層次

```
應用層：MIFARE 命令（讀/寫/認證）
↓
傳輸層：SELECT, ANTICOLLISION, HALT
↓
鏈路層：REQA/WUPA, ATQA, SAK
↓
物理層：13.56MHz ASK 調變
```

---

## 第 1 階段：Datasheet 導讀

### 📄 下載 Datasheet

搜尋：**"MFRC522 datasheet PDF"** → NXP 官方文件（約 100 頁）

### 🎯 必讀章節（按順序）

#### 1. Section 10: Pin description（15 分鐘）

理解每個接腳的功能：

| Pin | 功能 | 說明 |
|-----|------|------|
| MOSI | SPI 輸入 | 從 MCU 接收資料 |
| MISO | SPI 輸出 | 發送資料到 MCU |
| SCK | 時鐘輸入 | SPI 同步時鐘 |
| NSS | 片選 | 低電位有效 |
| TX1/TX2 | 天線輸出 | 連接到天線線圈 |
| RX | 天線輸入 | 接收卡片回應 |

#### 2. Section 11.1: SPI interface（30 分鐘）

**Figure 21: SPI write sequence** - 寫入時序
```
地址格式：0AAAAAA0 (最高位 = 0 表示寫入)
資料格式：DDDDDDDD (8 bits)
```

**Figure 22: SPI read sequence** - 讀取時序
```
地址格式：1AAAAAA0 (最高位 = 1 表示讀取)
```

**實作練習**：
```cpp
// 寫入暫存器
void writeReg(byte addr, byte value) {
  digitalWrite(CS, LOW);
  SPI.transfer((addr << 1) & 0x7E);  // 寫入地址
  SPI.transfer(value);
  digitalWrite(CS, HIGH);
}

// 讀取暫存器
byte readReg(byte addr) {
  digitalWrite(CS, LOW);
  SPI.transfer(((addr << 1) & 0x7E) | 0x80);  // 讀取地址
  byte value = SPI.transfer(0x00);
  digitalWrite(CS, HIGH);
  return value;
}
```

#### 3. Section 8: Register description（1.5 小時，分次閱讀）

**關鍵暫存器速查表**：

| 地址 | 名稱 | 功能 | 重要位元 |
|------|------|------|----------|
| 0x01 | CommandReg | 執行命令 | [3:0] 命令碼 |
| 0x06 | ErrorReg | 錯誤狀態 | bit 0: 協議錯誤 |
| 0x09 | FIFODataReg | FIFO 資料 | 讀/寫 FIFO |
| 0x0A | FIFOLevelReg | FIFO 水位 | [6:0] 位元組數 |
| 0x0D | BitFramingReg | 位元框架 | [2:0] 最後位元數 |
| 0x14 | TxControlReg | 天線控制 | bit 1:0 開啟天線 |
| 0x26 | RFCfgReg | RF 增益 | [6:4] 接收增益 |
| 0x37 | VersionReg | 版本號 | 0x91/0x92 |

**學習技巧**：
1. 先看 Table（表格），不要逐字讀
2. 對照範例程式，看哪些暫存器被用到
3. 做實驗時再回來查細節

#### 4. Section 9.3: Command overview（1 小時）

**14 個核心命令**：

| 命令碼 | 名稱 | 功能 |
|--------|------|------|
| 0x00 | Idle | 閒置，取消當前命令 |
| 0x01 | Mem | 存取內部記憶體 |
| 0x0C | Transceive | 發送 FIFO 資料並接收回應 |
| 0x0E | MFAuthent | MIFARE 認證 |
| 0x0F | SoftReset | 軟體重置 |

**Transceive 命令流程**（最常用）：
```
1. 清空 FIFO (FIFOLevelReg = 0x80)
2. 寫入資料到 FIFO (FIFODataReg)
3. 執行 Transceive (CommandReg = 0x0C)
4. 等待中斷 (ComIrqReg)
5. 讀取回應 (FIFODataReg)
6. 檢查錯誤 (ErrorReg)
```

#### 5. Table 149: MFRC522 command（15 分鐘）

完整的命令參數表，實作時會常查閱。

---

## 第 2 階段：實驗式學習

### 實驗環境設定

修改 `src/main.cpp`，加入實驗模式：

```cpp
#include "rfid.h"

RFID rfidReader;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n========== RFID 學習實驗 ==========\n");
  
  rfidReader.init();
  
  // 等待 5 秒，方便觀察輸出
  delay(5000);
  
  // ========== 選擇要執行的實驗 ==========
  
  // 實驗 1: 讀取版本號
  rfidReader.experiment_ReadVersion();
  
  // 實驗 2: 天線控制
  // rfidReader.experiment_AntennaControl(false);  // 關閉
  // delay(3000);
  // rfidReader.experiment_AntennaControl(true);   // 開啟
  
  // 實驗 3: 手動 REQA
  // Serial.println("\n請靠近卡片...");
  // delay(2000);
  // rfidReader.experiment_ManualREQA();
  
  // 實驗 5: 印出所有暫存器
  // rfidReader.experiment_DumpRegisters();
  
  // 實驗 6: 卡片類型偵測
  // Serial.println("\n請靠近卡片...");
  // delay(2000);
  // rfidReader.experiment_CardTypeDetection();
}

void loop() {
  // 持續偵測卡片
  if (rfidReader.detectCard()) {
    Serial.println("\n✓ 偵測到卡片！");
    
    uint8_t uid[10];
    uint8_t len;
    if (rfidReader.readCardUID(uid, &len)) {
      Serial.print("UID: ");
      for (int i = 0; i < len; i++) {
        if (uid[i] < 0x10) Serial.print("0");
        Serial.print(uid[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
    
    delay(2000);  // 防止重複讀取
  }
  
  delay(100);
}
```

### 🧪 實驗 1：讀取版本號

**目標**：學習基本的 SPI 讀取操作

**步驟**：
1. 上傳程式
2. 打開 Serial Monitor (115200 baud)
3. 觀察輸出

**預期輸出**：
```
===== 實驗 1: 讀取版本號 =====
目標: 學習基本的 SPI 讀取操作
Datasheet: 8.1.8 VersionReg (address 0x37)

VersionReg (0x37) = 0x92
二進位: 10010010

版本對照表:
  0x91 = Version 1.0
  0x92 = Version 2.0
  0x00/0xFF = 讀取失敗（檢查接線）
```

**理解要點**：
- `PCD_ReadRegister()` 內部做了什麼？
- 為什麼版本號是固定的？
- 如果讀到 0x00 或 0xFF 代表什麼？

**進階練習**：
```cpp
// 手動讀取版本號（不用函式庫）
digitalWrite(RFID_CS, LOW);
SPI.transfer(((0x37 << 1) & 0x7E) | 0x80);  // 0xEE
byte ver = SPI.transfer(0x00);
digitalWrite(RFID_CS, HIGH);
Serial.println(ver, HEX);
```

### 🧪 實驗 2：天線控制

**目標**：學習位元操作與觀察實際效果

**步驟**：
1. 修改 `setup()` 啟用實驗 2
2. 準備一張卡片
3. 觀察關閉天線後是否還能讀卡

**預期輸出**：
```
===== 實驗 2: 天線控制 =====
TxControlReg 目前值: 0b10000011
✓ 天線已關閉 (bit 1:0 = 00)
⚠ 注意: 關閉天線後將無法讀取卡片

（3 秒後）

✓ 天線已開啟 (bit 1:0 = 11)
```

**理解要點**：
- `|=` 和 `&= ~` 的位元操作
- TxControlReg 的 bit 1 和 bit 0 控制什麼？
- 其他 bits（bit 7:2）的功能是什麼？（查 Datasheet 8.6.3）

**進階實驗**：
用示波器觀察 TX1/TX2 接腳的 13.56MHz 信號：
- 開啟天線：應該看到正弦波
- 關閉天線：信號消失

### 🧪 實驗 3：手動發送 REQA

**目標**：理解完整的命令收發流程

**步驟**：
1. 啟用實驗 3
2. 靠近卡片
3. 觀察每個步驟的輸出

**預期輸出**：
```
===== 實驗 3: 手動發送 REQA =====
[1] 清空 FIFO (寫入 FIFOLevelReg bit 7)
[2] 寫入 REQA (0x26) 到 FIFO
[3] 設定 BitFramingReg = 0x07 (7 bits)
[4] 執行 Transceive 命令 (0x0C)
[5] ErrorReg = 0b00000000
[6] FIFO 中有 2 bytes 回應
ATQA: 0x04 0x00
```

**理解要點**：
- 為什麼 REQA 是 7 bits 而不是 8 bits？
  - ISO 14443A 標準規定，短幀格式
  - 用於節省功耗，快速偵測卡片
  
- ATQA 的含義？（查 Datasheet Table 18）
  - 0x04 0x00 = MIFARE Classic 1K
  - 0x44 0x00 = MIFARE Ultralight
  
- ErrorReg 每個 bit 的意義？
  - bit 0: ProtocolErr（協議錯誤）
  - bit 1: ParityErr（奇偶校驗錯誤）
  - bit 2: CRCErr（CRC 錯誤）
  - bit 3: CollErr（碰撞錯誤）
  - bit 4: BufferOvfl（緩衝區溢位）
  - bit 5: TempErr（溫度錯誤）
  - bit 6: WrErr（寫入錯誤）

**進階練習**：
修改實驗，發送 WUPA (0x52) 而不是 REQA (0x26)：
```cpp
mfrc522->PCD_WriteRegister(mfrc522->FIFODataReg, 0x52);  // WUPA
```
觀察有什麼不同？（WUPA 可以喚醒處於 HALT 狀態的卡片）

### 🧪 實驗 5：印出所有暫存器

**目標**：檢查初始化狀態，學習除錯方法

**步驟**：
1. 啟用實驗 5
2. 對照 Datasheet 檢查每個暫存器的預期值

**預期輸出**：
```
===== 實驗 5: 印出所有暫存器 =====
Addr | Reg Name          | Value | Binary
-----|-------------------|-------|----------
0x01 | CommandReg        | 0x20  | 0b00100000
0x06 | ErrorReg          | 0x00  | 0b00000000
0x09 | FIFODataReg       | 0x00  | 0b00000000
0x0A | FIFOLevelReg      | 0x00  | 0b00000000
0x14 | TxControlReg      | 0x83  | 0b10000011
0x26 | RFCfgReg          | 0x70  | 0b01110000
0x37 | VersionReg        | 0x92  | 0b10010010
```

**關鍵檢查點**：
- ✅ TxControlReg bit 1:0 = 11（天線已開啟）
- ✅ RFCfgReg = 0x70（最大增益 48dB）
- ✅ VersionReg = 0x91 或 0x92（晶片正常）
- ✅ ErrorReg = 0x00（無錯誤）

### 🧪 實驗 6：卡片類型偵測

**目標**：理解 SAK 與卡片型號的對應關係

**步驟**：
1. 準備不同類型的卡片（Classic 1K, Ultralight, etc.）
2. 逐一測試
3. 記錄每張卡的 UID、SAK、類型

**預期輸出**：
```
===== 實驗 6: 卡片類型偵測 =====
卡片資訊:
----------
UID: AA BB CC DD
UID 長度: 4 bytes
SAK: 0x08 (0b00001000)
卡片類型: MIFARE Classic 1K
函式庫判斷: MIFARE 1KB
```

**建立卡片資料庫**：
| UID | SAK | 類型 | 用途 |
|-----|-----|------|------|
| AA BB CC DD | 0x08 | Classic 1K | 測試卡 #1 |
| 11 22 33 44 | 0x08 | Classic 1K | 測試卡 #2 |
| ... | ... | ... | ... |

---

## 第 3 階段：深入原始碼

### 閱讀 MFRC522.cpp 關鍵函式

#### 1. `PCD_Init()` - 初始化流程

位置：`~/.platformio/lib/.../MFRC522/src/MFRC522.cpp`

```cpp
void MFRC522::PCD_Init() {
  // 1. 軟體重置
  PCD_WriteRegister(CommandReg, PCD_SoftReset);
  
  // 2. 等待重置完成
  // ...
  
  // 3. 設定定時器
  PCD_WriteRegister(TModeReg, 0x80);
  PCD_WriteRegister(TPrescalerReg, 0xA9);
  PCD_WriteRegister(TReloadRegH, 0x03);
  PCD_WriteRegister(TReloadRegL, 0xE8);
  
  // 4. 設定調變
  PCD_WriteRegister(TxASKReg, 0x40);
  
  // 5. 設定接收增益
  PCD_WriteRegister(RFCfgReg, (0x07<<4));
  
  // 6. 開啟天線
  PCD_AntennaOn();
}
```

**學習任務**：
- [ ] 找出每個暫存器的值對應 Datasheet 哪個章節
- [ ] 計算定時器設定的實際超時時間
- [ ] 修改增益設定，觀察讀取距離變化

#### 2. `PICC_IsNewCardPresent()` - 卡片偵測

```cpp
bool MFRC522::PICC_IsNewCardPresent() {
  byte bufferATQA[2];
  byte bufferSize = sizeof(bufferATQA);
  
  // 發送 REQA 或 WUPA
  byte command = PICC_CMD_REQA;
  byte validBits = 7;  // REQA 只有 7 bits
  
  // 執行 Transceive
  StatusCode status = PCD_TransceiveData(
    &command, 1,
    bufferATQA, &bufferSize,
    &validBits
  );
  
  return (status == STATUS_OK || status == STATUS_COLLISION);
}
```

**學習任務**：
- [ ] 追蹤 `PCD_TransceiveData()` 的實作
- [ ] 理解為什麼 `STATUS_COLLISION` 也算成功
- [ ] 修改程式，同時支援 REQA 和 WUPA

#### 3. `PICC_Select()` - 防碰撞與選擇

這是最複雜的函式（約 100 行），實作了完整的防碰撞演算法。

**學習策略**：
1. 先閱讀註解理解整體流程
2. 逐步除錯，印出每個變數
3. 對照 ISO 14443-3 標準文件

**關鍵概念**：
- Cascade Level（級聯等級）：1/2/3
- Cascade Tag (0x88)：表示還有下一級
- NVB (Number of Valid Bits)：有效位元數
- BCC (Block Check Character)：校驗碼

---

## 第 4 階段：實作專案

### 項目 1：簡單門禁系統

**功能**：
- 註冊 3 張授權卡片
- 刷卡驗證，綠 LED 表示通過，紅 LED 表示拒絕
- Serial Monitor 顯示刷卡記錄

**硬體**：
- RC522 + ESP32
- 綠 LED (GPIO 25)
- 紅 LED (GPIO 26)

**程式碼骨架**：
```cpp
void loop() {
  if (rfidReader.detectCard()) {
    if (rfidReader.verifyCard()) {
      digitalWrite(GREEN_LED, HIGH);
      Serial.println("✓ 授權通過");
      delay(2000);
      digitalWrite(GREEN_LED, LOW);
    } else {
      digitalWrite(RED_LED, HIGH);
      Serial.println("❌ 拒絕存取");
      delay(1000);
      digitalWrite(RED_LED, LOW);
    }
  }
}
```

### 項目 2：卡片複製檢測器

**原理**：儲存卡片的完整資料（不只 UID），檢測是否有人複製卡片。

**進階功能**：
- 讀取 MIFARE Classic 的 Sector 0 資料
- 使用 MIFARE 認證（預設金鑰）
- 比對完整資料，不只是 UID

### 項目 3：低功耗刷卡系統

**目標**：ESP32 深度睡眠，RC522 卡片靠近時喚醒

**實作要點**：
- 使用 RC522 的 IRQ 接腳
- ESP32 GPIO 中斷喚醒
- 測量功耗差異

---

## 除錯指南

### 問題 1：讀取版本號得到 0x00 或 0xFF

**可能原因**：
- ❌ SPI 接線錯誤（MOSI/MISO 接反）
- ❌ 片選 (CS) 接腳設定錯誤
- ❌ 供電不足（確認 3.3V 電流 > 50mA）
- ❌ GND 未接好

**除錯步驟**：
1. 用三用電表量 VCC 是否為 3.3V
2. 檢查 CS 接腳是否與 `config.h` 一致
3. 對調 MOSI 和 MISO 試試
4. 使用示波器檢查 SCK 是否有時鐘信號

### 問題 2：無法偵測卡片

**可能原因**：
- ❌ 天線未開啟（檢查 TxControlReg）
- ❌ 卡片距離太遠（> 5cm）
- ❌ 天線設計不良（某些便宜模組）
- ❌ 附近有金屬物體干擾

**除錯步驟**：
1. 執行 `experiment_DumpRegisters()` 檢查 TxControlReg
2. 貼近模組（< 2cm）再試
3. 更換另一張卡片
4. 移除附近金屬物體

### 問題 3：讀到錯誤的 UID

**可能原因**：
- ❌ CRC 校驗錯誤（通訊不穩定）
- ❌ 多張卡片同時在場（碰撞）
- ❌ SPI 時鐘太快（降低頻率）

**除錯步驟**：
1. 檢查 ErrorReg 的 bit 0-3
2. 確保只有一張卡片
3. 在 `SPI.begin()` 後加上：
   ```cpp
   SPI.setClockDivider(SPI_CLOCK_DIV16);  // 降低時鐘
   ```

### 問題 4：第一次讀取成功，之後就失敗

**可能原因**：
- ❌ 忘記呼叫 `PICC_HaltA()`，卡片未休眠
- ❌ FIFO 未清空

**解決方法**：
在每次讀取後加上：
```cpp
mfrc522->PICC_HaltA();
mfrc522->PCD_StopCrypto1();
```

---

## 參考資源

### 官方文件
- 📘 [MFRC522 Datasheet](https://www.nxp.com/docs/en/data-sheet/MFRC522.pdf) - NXP 官方
- 📘 [ISO/IEC 14443-3](https://www.iso.org/standard/50942.html) - 國際標準（需付費）
- 📘 [MIFARE Classic 1K Datasheet](https://www.nxp.com/docs/en/data-sheet/MF1S50YYX_V1.pdf) - 卡片規格

### 函式庫與範例
- 💻 [MFRC522 Library](https://github.com/miguelbalboa/rfid) - GitHub 原始碼
- 💻 [ESP32 MFRC522 Examples](https://github.com/OttoWinter/ESP32-MFRC522) - ESP32 專用版本

### 視頻教學
- 🎥 [How RFID Works - Ben Eater](https://www.youtube.com/watch?v=vLweFKgAFW8) - 視覺化講解
- 🎥 [MFRC522 深入分析](https://www.youtube.com/results?search_query=MFRC522+tutorial) - YouTube 搜尋

### 進階閱讀
- 📖 [MIFARE Hacking](https://www.cs.ru.nl/~flaviog/publications/Mifare.Cryptanalysis.pdf) - 安全性分析
- 📖 [Proxmark3](https://github.com/RfidResearchGroup/proxmark3) - 專業 RFID 研究工具

### 除錯工具
- 🔧 [示波器教學](https://www.youtube.com/watch?v=DgYGRtkd9Vs) - 觀察 SPI 波形
- 🔧 [邏輯分析儀](https://sigrok.org/) - 開源軟體分析

---

## 學習檢查清單

### ✅ 基礎級（1-2 週）
- [ ] 能解釋 RFID 的電磁感應原理
- [ ] 能手動讀寫 RC522 暫存器
- [ ] 理解 SPI 通訊時序
- [ ] 能成功讀取卡片 UID
- [ ] 能實作簡單的門禁系統

### ✅ 進階級（3-4 週）
- [ ] 理解 ISO 14443A 完整流程
- [ ] 能手動實作 REQA/Anticollision/Select
- [ ] 理解防碰撞演算法
- [ ] 能讀寫 MIFARE Classic 的 Sector
- [ ] 能使用 MIFARE 認證機制

### ✅ 專家級（2-3 個月）
- [ ] 能從零實作 RC522 驅動（不用函式庫）
- [ ] 理解並實作 CRC_A 演算法
- [ ] 能分析 ISO 14443 協議的安全漏洞
- [ ] 能優化程式達到低功耗（< 1mA）
- [ ] 能設計自訂的 RFID 天線

---

## 下一步

1. **立即開始**：執行實驗 1（讀取版本號）
2. **每日目標**：完成 1-2 個實驗
3. **每週回顧**：整理學習筆記，總結遇到的問題
4. **實戰應用**：將學到的知識用在智慧門鎖專案中

**記住**：從第一性原理學習不是最快的，但是最扎實的。堅持下去！

---

*建立日期：2026-01-30*  
*版本：v1.0*  
*作者：AI Assistant*
