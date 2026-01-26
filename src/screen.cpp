#include "screen.h"
#include "config.h"
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>

// 創建 Adafruit_ILI9341 對象
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// 創建 XPT2046 觸控對象
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

void Screen::init() {
  // TODO: 初始化螢幕
}

void Screen::initTouch() {
  // TODO: 初始化觸控
}

void Screen::showWelcome() {
  // TODO: 顯示歡迎畫面
}

void Screen::showWaitingForFinger() {
  // TODO: 顯示等待指紋
}

void Screen::showSuccess() {
  // TODO: 顯示成功畫面
}

void Screen::showFailed() {
  // TODO: 顯示失敗畫面
}

// ============================================
// 觸控功能實現
// ============================================

bool Screen::isTouched() {
  // TODO: 檢查是否被觸控
  return false;
}

void Screen::getTouchPoint(int16_t &x, int16_t &y) {
  // TODO: 取得觸控座標
  x = -1;
  y = -1;
}

void Screen::printTouchDebug() {
  // TODO: 印出觸控除錯資訊
}

// ============================================
// UI功能實現
// ============================================

void Screen::showMainMenu() {
  // TODO: 顯示主選單
}

void Screen::showPasswordInput() {
  // TODO: 顯示密碼輸入介面
}

void Screen::updatePasswordDisplay(String maskedPW) {
  // TODO: 更新密碼顯示
}

int8_t Screen::getKeypadPress(int16_t x, int16_t y) {
  // TODO: 判斷按了哪個鍵
  return -1;
}

void Screen::showCalibrationMarkers() {
  // TODO: 顯示校準標記
}

// ============================================
// 私有輔助函數
// ============================================

void Screen::drawKey(uint16_t x, uint16_t y, uint16_t w, uint16_t h, 
                      String label, uint16_t color, bool pressed) {
  // TODO: 繪製按鍵
}
