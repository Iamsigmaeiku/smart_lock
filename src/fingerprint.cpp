#include "fingerprint.h"
#include "config.h"

void Fingerprint::init() {
  // TODO: 初始化 AS608 指紋傳感器
  // 使用 Serial2 (RX=16, TX=17)
  // 設定 baud rate (通常是 57600)
  Serial.println("初始化指紋傳感器...");
}

bool Fingerprint::detectFinger() {
  // TODO: 檢測是否有手指放置
  return false;
}

bool Fingerprint::verifyFinger() {
  // TODO: 驗證指紋是否匹配
  return false;
}

bool Fingerprint::enrollFinger(uint8_t id) {
  // TODO: 註冊新指紋
  return false;
}
