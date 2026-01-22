#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#include <Arduino.h>

// AS608 指紋傳感器控制類
class Fingerprint {
public:
  // 初始化
  void init();
  
  // 檢測指紋
  bool detectFinger();
  
  // 驗證指紋
  bool verifyFinger();
  
  // 註冊新指紋
  bool enrollFinger(uint8_t id);

private:
  // TODO: 添加必要的成員變數
};

#endif
