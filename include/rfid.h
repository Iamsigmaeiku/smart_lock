#ifndef RFID_H
#define RFID_H

#include <Arduino.h>
#include <MFRC522.h>      // MFRC522 RFID 讀卡器函式庫
#include <Preferences.h>  // ESP32 NVS 儲存（比 EEPROM 更好）

// RFID RC522 控制類
class RFID {
public:
  // 初始化
  void init();
  
  // 檢測卡片
  bool detectCard();
  
  // 讀取卡片 UID
  bool readCardUID(uint8_t* uid, uint8_t* uidLength);
  
  // 驗證卡片
  bool verifyCard();
  
  // 註冊新卡片
  bool enrollCard();

private:
  // MFRC522 硬體物件
  // 注意：物件會在 init() 中動態建立，因為需要從 config.h 讀取腳位
  MFRC522* mfrc522;  // RFID 讀卡器物件指標
  
  // ESP32 NVS 儲存（用於持久化註冊卡片）
  Preferences prefs;
  
  // 暫存最後讀取的卡片 UID
  uint8_t lastUID[10];      // UID 緩衝區（最大 10 bytes）
  uint8_t lastUIDLength;    // 實際 UID 長度（通常 4 或 7 bytes）
  
  // 系統常數
  static const uint8_t MAX_CARDS = 10;       // 最多註冊 10 張卡片
  static const uint8_t MAX_UID_LENGTH = 10;  // UID 最大長度
  
  // 輔助函式
  // 比對兩個 UID 是否相同
  bool compareUID(uint8_t* uid1, uint8_t* uid2, uint8_t len);
  
  // 檢查卡片是否已註冊
  bool isCardRegistered(uint8_t* uid, uint8_t len);
  
  // 儲存卡片到 NVS
  void saveCardToStorage(uint8_t* uid, uint8_t len);
};

#endif
