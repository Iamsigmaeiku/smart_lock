#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
#include "config.h"

class Screen {
public:
  Screen();
  void init();
  void initTouch();                                   // 初始化觸控（XPT2046）

  void showWelcome();
  void showMainMenu();                                // 主選單（選擇驗證方式）
  void showPasswordInput();                           // 顯示密碼輸入介面
  void updatePasswordDisplay(String maskedPW);        // 更新密碼顯示
  void showWaitingForFinger();
  void showSuccess();
  void showFailed();
  
  // 觸控相關
  bool isTouched();                                   // 檢測是否被觸摸
  void getTouchPoint(int16_t &x, int16_t &y);         // 獲取觸摸座標（已映射到螢幕像素）
  void printTouchDebug();                             // 調試用：印出觸摸座標
  int8_t getKeypadPress(int16_t x, int16_t y);        // 判斷按了哪個鍵（返回0-9或特殊鍵）
  
  // 校準測試
  void showCalibrationMarkers();                      // 顯示校準標記點

private:
  Adafruit_ILI9341 tft;
  XPT2046_Touchscreen ts;                            // 觸控物件
  
  // UI 輔助函數
  void drawButton(int16_t x, int16_t y, int16_t w, int16_t h, 
                  uint16_t color, const char* label, uint8_t textSize = 2);
};

#endif
