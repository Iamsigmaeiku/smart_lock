#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>

// ILI9341 螢幕控制類（使用 Adafruit_ILI9341 庫）
// 接腳: CS->GPIO5, RST->GPIO22, DC->GPIO21, MOSI->GPIO23, SCK->GPIO18, MISO->GPIO19
class Screen {
public:
  // 初始化
  void init();
  
  // 顯示歡迎畫面
  void showWelcome();
  
  // 顯示等待指紋
  void showWaitingForFinger();
  
  // 顯示驗證成功
  void showSuccess();
  
  // 顯示驗證失敗
  void showFailed();
};

#endif
