#include "rfid.h"
#include "config.h"
#include <SPI.h>

// ============================================
// 初始化 RFID 模組
// ============================================
void RFID::init() {
  Serial.println("=== 初始化 RFID RC522 模組 ===");
  
  // 1. 初始化 SPI（螢幕可能已初始化，但重複呼叫是安全的）
  //    使用預設腳位：SCLK=18, MISO=19, MOSI=23【與螢幕共用，請確認接線】
  //    透過不同 CS 腳位區分設備：TFT_CS=5, RFID_CS=4
  SPI.begin();
  Serial.println("  [√] SPI 已初始化（與螢幕共用）");
  
  // 2. 建立 MFRC522 物件
  //    RFID_CS = 4    // Chip Select/SDA 腳位【請根據實際接線修改 config.h】
  //    RFID_RST = 27  // Reset 腳位【請根據實際接線修改 config.h】
  mfrc522 = new MFRC522(RFID_CS, RFID_RST);
  Serial.printf("  [√] MFRC522 物件已建立（CS=%d, RST=%d）\n", RFID_CS, RFID_RST);
  
  // 3. 初始化 MFRC522 晶片
  mfrc522->PCD_Init();
  delay(50);  // 給 MFRC522 一點時間初始化
  Serial.println("  [√] MFRC522 晶片已初始化");
  
  // 4. 讀取韌體版本以確認連線正常（應為 0x91 或 0x92）
  byte version = mfrc522->PCD_ReadRegister(MFRC522::VersionReg);
  Serial.printf("  [√] MFRC522 韌體版本: 0x%02X", version);
  
  if (version == 0x00 || version == 0xFF) {
    Serial.println(" ⚠️ 警告：版本異常，請檢查接線！");
    Serial.println("      - 確認 3.3V 和 GND 連接");
    Serial.println("      - 確認 SPI 腳位（MOSI/MISO/SCLK）");
    Serial.println("      - 確認 CS 和 RST 腳位設定");
  } else {
    Serial.println(" ✓ 正常");
  }
  
  // 5. 初始化 Preferences（ESP32 NVS 儲存）
  //    namespace = "rfid"，用於儲存註冊卡片
  prefs.begin("rfid", false);  // false = 讀寫模式
  
  // 讀取已註冊卡片數量
  uint8_t cardCount = prefs.getUChar("count", 0);
  Serial.printf("  [√] NVS 儲存已初始化，已註冊 %d 張卡片\n", cardCount);
  
  // 6. 提高天線增益以增強讀取距離（建議設定）
  mfrc522->PCD_SetAntennaGain(mfrc522->RxGain_max);
  Serial.println("  [√] 天線增益已設為最大（增強讀取距離）");
  
  // 7. 初始化暫存變數
  lastUIDLength = 0;
  memset(lastUID, 0, sizeof(lastUID));
  
  Serial.println("=== RFID 模組初始化完成！===\n");
}

// ============================================
// 檢測是否有卡片靠近
// ============================================
bool RFID::detectCard() {
  // 檢測是否有新卡片靠近
  if (!mfrc522->PICC_IsNewCardPresent()) {
    return false;  // 沒有卡片
  }
  
  // 嘗試讀取卡片序號
  if (!mfrc522->PICC_ReadCardSerial()) {
    return false;  // 讀取失敗
  }
  
  // 讀取成功，儲存 UID 到暫存區
  lastUIDLength = mfrc522->uid.size;
  memcpy(lastUID, mfrc522->uid.uidByte, lastUIDLength);
  
  // 印出 UID 供除錯使用
  Serial.print("偵測到卡片 UID: ");
  for (uint8_t i = 0; i < lastUIDLength; i++) {
    Serial.printf("%02X ", lastUID[i]);
  }
  Serial.println();
  
  // 停止當前卡片的通訊（準備讀取下一張）
  mfrc522->PICC_HaltA();
  mfrc522->PCD_StopCrypto1();
  
  return true;
}

// ============================================
// 讀取最後一次檢測到的卡片 UID
// ============================================
bool RFID::readCardUID(uint8_t* uid, uint8_t* uidLength) {
  if (lastUIDLength == 0) {
    return false;  // 沒有讀取到卡片
  }
  
  // 將最後讀取的 UID 複製到傳入的緩衝區
  memcpy(uid, lastUID, lastUIDLength);
  *uidLength = lastUIDLength;
  
  return true;
}

// ============================================
// 驗證卡片是否已註冊
// ============================================
bool RFID::verifyCard() {
  if (lastUIDLength == 0) {
    Serial.println("錯誤：沒有讀取到卡片");
    return false;
  }
  
  // 檢查暫存的 UID 是否已註冊
  bool registered = isCardRegistered(lastUID, lastUIDLength);
  
  if (registered) {
    Serial.println("✓ 卡片驗證成功！");
  } else {
    Serial.println("✗ 卡片驗證失敗：此卡片未註冊");
  }
  
  return registered;
}

// ============================================
// 註冊新卡片
// ============================================
bool RFID::enrollCard() {
  if (lastUIDLength == 0) {
    Serial.println("錯誤：沒有讀取到卡片");
    return false;  // 沒有讀取到卡片
  }
  
  // 檢查是否已註冊
  if (isCardRegistered(lastUID, lastUIDLength)) {
    Serial.println("此卡片已註冊過，無需重複註冊");
    return false;
  }
  
  // 檢查是否已達最大註冊數量
  uint8_t cardCount = prefs.getUChar("count", 0);
  if (cardCount >= MAX_CARDS) {
    Serial.printf("錯誤：已達最大註冊數量（%d 張）\n", MAX_CARDS);
    return false;
  }
  
  // 儲存到 NVS
  saveCardToStorage(lastUID, lastUIDLength);
  Serial.println("✓ 卡片註冊成功！");
  
  return true;
}

// ============================================
// 輔助函式：比對兩個 UID 是否相同
// ============================================
bool RFID::compareUID(uint8_t* uid1, uint8_t* uid2, uint8_t len) {
  for (uint8_t i = 0; i < len; i++) {
    if (uid1[i] != uid2[i]) {
      return false;
    }
  }
  return true;
}

// ============================================
// 輔助函式：檢查卡片是否已註冊
// ============================================
bool RFID::isCardRegistered(uint8_t* uid, uint8_t len) {
  uint8_t cardCount = prefs.getUChar("count", 0);
  
  // 逐一比對已註冊的卡片
  for (uint8_t i = 0; i < cardCount; i++) {
    char key[16];
    sprintf(key, "card_%d", i);
    
    // 從 NVS 讀取卡片 UID
    uint8_t storedUID[MAX_UID_LENGTH];
    size_t storedLen = prefs.getBytes(key, storedUID, MAX_UID_LENGTH);
    
    // 比對長度和內容
    if (storedLen == len && compareUID(uid, storedUID, len)) {
      return true;  // 找到匹配的卡片
    }
  }
  
  return false;  // 沒有找到匹配的卡片
}

// ============================================
// 輔助函式：儲存卡片到 NVS
// ============================================
void RFID::saveCardToStorage(uint8_t* uid, uint8_t len) {
  uint8_t cardCount = prefs.getUChar("count", 0);
  
  // 建立新卡片的 key（例如 "card_0", "card_1", ...）
  char key[16];
  sprintf(key, "card_%d", cardCount);
  
  // 儲存 UID 到 NVS
  prefs.putBytes(key, uid, len);
  
  // 更新卡片數量
  prefs.putUChar("count", cardCount + 1);
  
  Serial.printf("已儲存卡片 %d，UID: ", cardCount);
  for (uint8_t i = 0; i < len; i++) {
    Serial.printf("%02X ", uid[i]);
  }
  Serial.println();
}
