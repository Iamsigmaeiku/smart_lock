#include "rfid.h"
#include "config.h"

void RFID::init() {
  // TODO: 初始化 RC522 RFID 模組
  // 需要設定 SPI 通訊和接腳
  Serial.println("初始化 RFID 模組...");
}

bool RFID::detectCard() {
  // TODO: 檢測是否有卡片靠近
  return false;
}

bool RFID::readCardUID(uint8_t* uid, uint8_t* uidLength) {
  // TODO: 讀取卡片的 UID
  return false;
}

bool RFID::verifyCard() {
  // TODO: 驗證卡片是否已註冊
  return false;
}

bool RFID::enrollCard() {
  // TODO: 註冊新卡片
  return false;
}
