#ifndef RFID_H
#define RFID_H

#include <Arduino.h>

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
  // TODO: 添加必要的成員變數
  // 例如: MFRC522 物件、已註冊卡片列表等
};

#endif
