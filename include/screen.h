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
  void initTouch();

  void showWelcome();
  void showWaitingForFinger();
  void showSuccess();
  void showFailed();
  
  // 觸控相關
  bool isTouched();                                   // 檢測是否被觸摸
  void getTouchPoint(int16_t &x, int16_t &y);         // 獲取觸摸座標（已映射到螢幕像素）
  void printTouchDebug();                             // 調試用：印出觸摸座標
  
  // 密碼輸入UI
  void showMainMenu();                                // 主選單（選擇驗證方式）
  void showPasswordInput();                           // 顯示密碼輸入介面
  void updatePasswordDisplay(String maskedPW);        // 更新密碼顯示
  int8_t getKeypadPress(int16_t x, int16_t y);        // 判斷按了哪個鍵（返回0-9或特殊鍵）
  
  // 校準測試
  void showCalibrationMarkers();                      // 顯示校準標記點
  
  // 輔助函數
  bool isButtonPressed(int16_t x, int16_t y, int16_t btnX, int16_t btnY, 
                       int16_t btnW, int16_t btnH);   // 判斷是否按中按鈕
  void drawButton(int16_t x, int16_t y, int16_t w, int16_t h, 
                  uint16_t color, const char* label, uint8_t textSize);

private:
  Adafruit_ILI9341 tft;
  XPT2046_Touchscreen ts;
};

#endif
