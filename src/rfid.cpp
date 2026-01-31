#include "rfid.h"
#include "config.h"
#include <SPI.h>

/* ============================================================================
 * RFID RC522 完整實作 - 每個段落都標注 Datasheet 位置
 * 
 * 📖 閱讀建議：
 * 對照 MFRC522 Datasheet 閱讀，每個函式都有章節標註
 * Datasheet: https://www.nxp.com/docs/en/data-sheet/MFRC522.pdf
 * ============================================================================ */

void RFID::init() {
  Serial.println("\n========== RFID RC522 初始化 ==========");
  
  /* ========== 步驟 1: 初始化 SPI ==========
   * 📖 Datasheet: Section 11.1 SPI interface (p.68-69)
   * 
   * SPI 設定：
   * - Mode 0 (CPOL=0, CPHA=0)
   * - 最高 10 MHz 時鐘
   * - MSB first
   * 
   * 注意：與螢幕共用 SPI，可能已經初始化過
   */
  // SPI.begin(RFID_SCLK, RFID_MISO, RFID_MOSI, RFID_CS);
  Serial.println("[1/5] SPI 已初始化（與螢幕共用）");
  
  
  /* ========== 步驟 2: 建立 MFRC522 物件 ==========
   * 參數：CS pin, RST pin
   * RST pin 設為 UINT8_MAX 表示不使用硬體重置
   */
  mfrc522 = new MFRC522(RFID_CS, RFID_RST);
  Serial.println("[2/5] MFRC522 物件已建立");
  
  
  /* ========== 步驟 3: 初始化 RC522 晶片 ==========
   * 📖 Datasheet 內部執行：
   * 
   * 3.1 軟體重置 (Section 8.1.1 CommandReg, p.36)
   *     寫入 CommandReg = 0x0F (SoftReset command)
   *     等待晶片重置完成
   * 
   * 3.2 設定定時器 (Section 8.2.4-8.2.6, p.41-42)
   *     TModeReg = 0x80        預分頻器自動啟動
   *     TPrescalerReg = 0xA9   預分頻器值
   *     TReloadRegH = 0x03     重載值高位元組
   *     TReloadRegL = 0xE8     重載值低位元組
   *     → 超時時間約 25ms
   * 
   * 3.3 設定調變 (Section 8.6.2 TxASKReg, p.44)
   *     TxASKReg = 0x40        100% ASK 調變
   * 
   * 3.4 設定接收增益 (Section 8.6.9 RFCfgReg, p.48)
   *     RFCfgReg[6:4] = 0x07   最大增益 ~48dB
   *     → 影響讀取距離（0-10cm）
   * 
   * 3.5 開啟天線 (Section 8.6.3 TxControlReg, p.45)
   *     TxControlReg |= 0x03   設定 bit 1:0
   *     → Bit 1: Tx2RFEn (TX2 射頻輸出)
   *     → Bit 0: Tx1RFEn (TX1 射頻輸出)
   */
  mfrc522->PCD_Init();
  Serial.println("[3/5] RC522 晶片初始化完成");
  
  
  /* ========== 步驟 4: 驗證晶片版本 ==========
   * 📖 Datasheet: Section 8.1.8 VersionReg (address 0x37, p.40)
   * 
   * 讀取 VersionReg：
   * - 0x00 或 0xFF → SPI 通訊失敗（檢查接線）
   * - 0x91 → Version 1.0
   * - 0x92 → Version 2.0（常見）
   * 
   * SPI 讀取格式 (Section 11.1.2, p.68)：
   * - 地址 = ((0x37 << 1) & 0x7E) | 0x80 = 0xEE
   * - 發送: [0xEE] [0x00]
   * - 接收: [xx]   [version]
   */
  byte version = mfrc522->PCD_ReadRegister(MFRC522::VersionReg);
  Serial.print("[4/5] RC522 版本號: 0x");
  Serial.println(version, HEX);
  
  if (version == 0x00 || version == 0xFF) {
    Serial.println("❌ 錯誤：無法讀取版本號！");
    Serial.println("   請檢查：");
    Serial.println("   - SPI 接線 (MOSI/MISO/SCK/CS)");
    Serial.println("   - 3.3V 供電");
    Serial.println("   - CS 腳位設定 (config.h)");
  } else if (version == 0x91 || version == 0x92) {
    Serial.println("✓ RC522 晶片正常");
  } else {
    Serial.print("⚠ 未知版本號: 0x");
    Serial.println(version, HEX);
  }
  
  
  /* ========== 步驟 5: 初始化卡片儲存 ==========
   * 清空已註冊卡片列表
   */
  cardCount = 0;
  memset(registeredUIDs, 0, sizeof(registeredUIDs));
  memset(uidLengths, 0, sizeof(uidLengths));
  Serial.println("[5/5] 卡片儲存系統初始化完成");
  
  Serial.println("========== RFID 初始化完成 ==========\n");
}


bool RFID::detectCard() {
  /* ========== ISO 14443A 卡片偵測流程 ==========
   * 📖 Datasheet: Section 9.3.3 REQA/WUPA command (p.10)
   * 📖 ISO 14443-3 標準
   * 
   * 
   * 步驟 1: 發送 REQA (Request Type A)
   * ----------------------------------------
   * 命令格式：
   * - 命令碼: 0x26 (REQA) 或 0x52 (WUPA)
   * - 長度: 7 bits（短幀格式）
   * 
   * 為什麼是 7 bits？
   * - ISO 14443-3 規定短幀用 7 bits
   * - 節省功耗，快速偵測卡片
   * 
   * 📖 Datasheet: Section 8.1.2 BitFramingReg (address 0x0D, p.37)
   * - TxLastBits[2:0] = 7（最後一個位元組傳輸 7 bits）
   * 
   * 
   * 步驟 2: 接收 ATQA (Answer To Request)
   * ----------------------------------------
   * 📖 Datasheet: Table 18 ATQA response (p.10)
   * 
   * ATQA 格式（2 bytes）：
   * - Byte 1 (低位元組): 位元幀防碰撞資訊
   * - Byte 2 (高位元組): 卡片類型
   * 
   * 常見卡片的 ATQA：
   * - 0x04 0x00 → MIFARE Classic 1K
   * - 0x02 0x00 → MIFARE Classic 4K
   * - 0x44 0x00 → MIFARE Ultralight
   * - 0x03 0x00 → MIFARE DESFire
   * 
   * 
   * 底層實作（函式庫內部）：
   * ----------------------------------------
   * 1. 清空 FIFO (Section 8.1.5 FIFOLevelReg, p.39)
   *    寫入 FIFOLevelReg = 0x80（bit 7 = FlushBuffer）
   * 
   * 2. 寫入 REQA 到 FIFO (Section 8.1.4 FIFODataReg, p.38)
   *    寫入 FIFODataReg = 0x26
   * 
   * 3. 設定位元框架
   *    寫入 BitFramingReg = 0x07（7 bits）
   * 
   * 4. 執行 Transceive 命令 (Section 9.3.4, p.65)
   *    寫入 CommandReg = 0x0C
   * 
   * 5. 等待中斷 (Section 8.1.3 ComIrqReg, p.37)
   *    輪詢 ComIrqReg，等待 RxIRq 或 IdleIRq
   * 
   * 6. 檢查錯誤 (Section 8.1.6 ErrorReg, p.39)
   *    讀取 ErrorReg，檢查通訊錯誤
   * 
   * 7. 讀取 ATQA (FIFODataReg)
   *    從 FIFO 讀取 2 bytes
   */
  
  if (!mfrc522->PICC_IsNewCardPresent()) {
    return false;  // 沒有新卡片
  }
  
  
  /* ========== 步驟 3: 防碰撞與選擇 ==========
   * 📖 Datasheet: Section 9.3.4 Anticollision and Select (p.11-12)
   * 📖 ISO 14443-3: Anticollision and select
   * 
   * 
   * 為什麼需要防碰撞？
   * ----------------------------------------
   * - 多張卡同時在場時，RF 信號會碰撞
   * - 需要用演算法依序識別每張卡
   * 
   * 
   * 防碰撞演算法（Cascade Level）：
   * ----------------------------------------
   * Level 1 (CL1): 使用 0x93 命令
   *   - ANTICOLLISION: 0x93 0x20
   *   - SELECT:        0x93 0x70 [4 bytes UID] [BCC]
   * 
   * Level 2 (CL2): 使用 0x95 命令（如果需要）
   *   - ANTICOLLISION: 0x95 0x20
   *   - SELECT:        0x95 0x70 [4 bytes UID] [BCC]
   * 
   * Level 3 (CL3): 使用 0x97 命令（如果需要）
   *   - ANTICOLLISION: 0x97 0x20
   *   - SELECT:        0x97 0x70 [4 bytes UID] [BCC]
   * 
   * 
   * UID 長度判斷：
   * ----------------------------------------
   * 📖 Datasheet: Table 19 SAK response (p.11)
   * 
   * SAK (Select Acknowledge) 1 byte：
   * - Bit 3 = 0 → UID 完整
   * - Bit 3 = 1 → 需要下一級 Cascade（UID 未完整）
   * 
   * UID 長度：
   * - 4 bytes:  單級（CL1）
   * - 7 bytes:  雙級（CL1 + CL2）
   * - 10 bytes: 三級（CL1 + CL2 + CL3）
   * 
   * 
   * Cascade Tag (CT)：
   * ----------------------------------------
   * 當 UID 需要多級時，第一個位元組為 0x88（Cascade Tag）
   * 
   * 範例（7 bytes UID = AA BB CC DD EE FF GG）：
   * - CL1: 0x88 AA BB CC [BCC]
   * - CL2: DD EE FF GG [BCC]
   * 
   * 
   * BCC (Block Check Character)：
   * ----------------------------------------
   * 校驗碼 = UID 各位元組的 XOR
   * 例如: BCC = 0x88 ^ AA ^ BB ^ CC
   * 
   * 
   * 底層實作：
   * ----------------------------------------
   * PICC_ReadCardSerial() 內部會：
   * 1. 呼叫 PICC_Select(&uid)
   * 2. 執行 Anticollision（可能多級）
   * 3. 每級執行 SELECT 並檢查 SAK
   * 4. 組合完整的 UID
   * 5. 儲存到 mfrc522->uid 結構
   */
  
  if (!mfrc522->PICC_ReadCardSerial()) {
    return false;  // 無法讀取序列號
  }
  
  return true;  // 成功偵測卡片
}


bool RFID::readCardUID(uint8_t* uid, uint8_t* uidLength) {
  /* ========== 讀取並複製 UID ==========
   * 
   * 函式庫結構：
   * ----------------------------------------
   * mfrc522->uid.size      → UID 長度（4/7/10）
   * mfrc522->uid.uidByte[] → UID 位元組陣列
   * mfrc522->uid.sak       → SAK (Select Acknowledge)
   * 
   * 📖 Datasheet: Table 18 ATQA and Table 19 SAK (p.10-11)
   */
  
  // 先檢測卡片
  if (!detectCard()) {
    return false;
  }
  
  // 複製 UID 到輸出緩衝區
  *uidLength = mfrc522->uid.size;
  for (uint8_t i = 0; i < *uidLength; i++) {
    uid[i] = mfrc522->uid.uidByte[i];
  }
  
  // 除錯輸出
  Serial.print("讀取到 UID (");
  Serial.print(*uidLength);
  Serial.print(" bytes): ");
  printUID(uid, *uidLength);
  
  
  /* ========== 卡片休眠 (HALT) ==========
   * 📖 Datasheet: Section 9.3.5 HALT command (p.12)
   * 
   * HALT 命令：
   * - 命令碼: 0x50 0x00
   * - 功能: 讓卡片進入休眠狀態
   * - 目的:
   *   1. 省電（卡片被動式，省的是 Reader 的功耗）
   *   2. 避免重複讀取同一張卡
   *   3. 下次需要用 WUPA (0x52) 才能喚醒
   * 
   * 底層實作：
   * 1. 寫入 FIFO: [0x50, 0x00]
   * 2. 計算 CRC_A (Section 8.7 CRC calculation, p.49)
   * 3. 執行 Transceive 命令
   * 4. 不期待回應（卡片進入休眠）
   */
  mfrc522->PICC_HaltA();
  
  // 停止加密通訊（如果有用 MIFARE 認證的話）
  mfrc522->PCD_StopCrypto1();
  
  return true;
}


bool RFID::verifyCard() {
  /* ========== 卡片驗證流程 ==========
   * 
   * 1. 讀取當前卡片 UID
   * 2. 與已註冊列表逐一比對
   * 3. 找到匹配則驗證成功
   */
  
  uint8_t uid[MAX_UID_LENGTH];
  uint8_t uidLength;
  
  // 讀取當前卡片
  if (!readCardUID(uid, &uidLength)) {
    Serial.println("❌ 無法讀取卡片");
    return false;
  }
  
  // 與已註冊卡片比對
  Serial.print("正在驗證... 已註冊: ");
  Serial.print(cardCount);
  Serial.println(" 張");
  
  for (uint8_t i = 0; i < cardCount; i++) {
    // 先檢查長度
    if (uidLength != uidLengths[i]) {
      continue;
    }
    
    // 逐位元組比對
    if (compareUID(uid, registeredUIDs[i], uidLength)) {
      Serial.print("✓ 驗證成功！匹配卡片 #");
      Serial.println(i + 1);
      return true;
    }
  }
  
  Serial.println("❌ 驗證失敗：此卡片未註冊");
  return false;
}


bool RFID::enrollCard() {
  /* ========== 卡片註冊流程 ==========
   * 
   * 1. 檢查是否已滿（最多 10 張）
   * 2. 讀取卡片 UID
   * 3. 檢查是否重複註冊
   * 4. 儲存到陣列
   * 
   * ⚠️ 注意：
   * 這裡只儲存在 RAM，重開機會遺失
   * 實際專案應儲存到：
   * - EEPROM (Preferences)
   * - Flash (SPIFFS/LittleFS)
   * - SD 卡
   * - 雲端資料庫
   */
  
  // 檢查容量
  if (cardCount >= MAX_CARDS) {
    Serial.println("❌ 卡片數量已達上限 (10 張)");
    return false;
  }
  
  uint8_t uid[MAX_UID_LENGTH];
  uint8_t uidLength;
  
  // 讀取卡片
  if (!readCardUID(uid, &uidLength)) {
    Serial.println("❌ 無法讀取卡片");
    return false;
  }
  
  // 檢查重複
  for (uint8_t i = 0; i < cardCount; i++) {
    if (uidLength == uidLengths[i] && 
        compareUID(uid, registeredUIDs[i], uidLength)) {
      Serial.println("⚠ 此卡片已經註冊過了");
      return false;
    }
  }
  
  // 儲存新卡片
  uidLengths[cardCount] = uidLength;
  memcpy(registeredUIDs[cardCount], uid, uidLength);
  cardCount++;
  
  Serial.print("✓ 註冊成功！目前已註冊 ");
  Serial.print(cardCount);
  Serial.println(" 張卡片");
  Serial.print("   UID: ");
  printUID(uid, uidLength);
  
  return true;
}


/* ============================================================================
 * 輔助函式
 * ============================================================================ */

bool RFID::compareUID(const uint8_t* uid1, const uint8_t* uid2, uint8_t length) {
  /* 逐位元組比對兩個 UID */
  for (uint8_t i = 0; i < length; i++) {
    if (uid1[i] != uid2[i]) {
      return false;
    }
  }
  return true;
}


void RFID::printUID(const uint8_t* uid, uint8_t length) {
  /* 印出 UID（十六進位格式）
   * 格式: AA:BB:CC:DD
   */
  for (uint8_t i = 0; i < length; i++) {
    if (uid[i] < 0x10) Serial.print("0");  // 補零
    Serial.print(uid[i], HEX);
    if (i < length - 1) Serial.print(":");
  }
  Serial.println();
}


/* ============================================================================
 * 📚 學習筆記
 * ============================================================================
 * 
 * 【核心概念】
 * ----------------------------------------
 * 1. SPI 通訊：MCU 與 RC522 的橋樑
 * 2. ISO 14443A：Reader 與卡片的通訊協議
 * 3. Anticollision：多卡防碰撞演算法
 * 4. UID：卡片的唯一識別碼（但可被複製！）
 * 
 * 
 * 【完整流程圖】
 * ----------------------------------------
 * MCU → SPI → RC522 → RF (13.56MHz) → Card
 *  ↑                                      ↓
 *  └──────────── RF 回應 ←────────────────┘
 * 
 * 
 * 【關鍵暫存器】
 * ----------------------------------------
 * CommandReg (0x01):    執行命令
 * FIFODataReg (0x09):   資料收發
 * ErrorReg (0x06):      錯誤狀態
 * TxControlReg (0x14):  天線控制
 * VersionReg (0x37):    晶片版本
 * 
 * 
 * 【除錯技巧】
 * ----------------------------------------
 * 1. 版本號讀不到 (0x00/0xFF)
 *    → 檢查 SPI 接線
 * 
 * 2. 偵測不到卡片
 *    → 檢查天線是否開啟 (TxControlReg bit 1:0 = 11)
 *    → 卡片距離太遠 (< 5cm)
 * 
 * 3. UID 讀取錯誤
 *    → 檢查 ErrorReg (碰撞/CRC 錯誤)
 *    → 確保只有一張卡
 * 
 * 4. 重複讀取同一張卡
 *    → 記得呼叫 PICC_HaltA()
 * 
 * 
 * 【進階學習】
 * ----------------------------------------
 * 1. MIFARE Classic 認證與讀寫
 *    → Section 10.3 MIFARE authentication (p.58)
 * 
 * 2. 低功耗模式
 *    → Section 8.1.1 Soft power-down (p.36)
 * 
 * 3. 中斷模式（非輪詢）
 *    → Section 8.1.3 ComIrqReg (p.37)
 * 
 * 4. 多卡同時處理
 *    → 研究完整的 Anticollision 演算法
 * 
 * ============================================================================ */
