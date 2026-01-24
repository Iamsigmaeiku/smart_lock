#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>

// ILI9341 螢幕控制類
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

private:
  void sendCommand(uint8_t cmd);
  void sendData(uint8_t data);
  void setAddress(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
  void fillScreen(uint16_t color);
  void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
  void drawText(uint16_t x, uint16_t y, const char* text, uint16_t color, uint8_t size);
};

#endif
