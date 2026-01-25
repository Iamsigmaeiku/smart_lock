#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>

// ILI9341 螢幕控制類（使用 Adafruit_ILI9341 庫）
// 接腳: CS->GPIO5, RST->GPIO22, DC->GPIO21, MOSI->GPIO23, SCK->GPIO18, MISO->GPIO19
// 觸控: T_CS->GPIO15, T_IRQ->GPIO2
class Screen {
public:
  // 螢幕初始化
  void init();
  
  // 觸控初始化
  void initTouch();
  
  // 顯示歡迎畫面
  void showWelcome();
  
  // 顯示等待指紋
  void showWaitingForFinger();
  
  // 顯示驗證成功
  void showSuccess();
  
  // 顯示驗證失敗
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

private:
  // UI佈局常數
  struct KeypadLayout {
    static const uint16_t START_X = 40;
    static const uint16_t START_Y = 80;
    static const uint16_t KEY_W = 70;
    static const uint16_t KEY_H = 50;
    static const uint16_t SPACING = 10;
  };
  
  // 繪製單個按鍵
  void drawKey(uint16_t x, uint16_t y, uint16_t w, uint16_t h, 
               String label, uint16_t color, bool pressed = false);
};

#endif
