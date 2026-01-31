#include "screen.h"
#include "config.h"

// ILI9341 沒有無參數建構子，所以必須用初始化列表建構
Screen::Screen()
  : tft(TFT_CS, TFT_DC, TFT_RST), touch(TOUCH_CS, TOUCH_IRQ) {
}

void Screen::init() {
  tft.begin();
  tft.setRotation(1);      // 依你的螢幕方向調整
  tft.fillScreen(0x0000);
  tft.setTextWrap(false);
}

void Screen::showWelcome() {
  tft.fillScreen(0x0000);

  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(60, 60);
  tft.print("Welcome");

  tft.setTextColor(0x07E0);
  tft.setTextSize(3);
  tft.setCursor(30, 100);
  tft.print("SMART LOCK");

  tft.setTextColor(0x8410);
  tft.setTextSize(2);
  tft.setCursor(40, 170);
  tft.print("Touch sensor");
}

void Screen::showWaitingForFinger() {
  tft.fillScreen(0x0000);

  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(20, 60);
  tft.print("Please verify");

  tft.setTextColor(0x07E0);
  tft.setTextSize(3);
  tft.setCursor(20, 110);
  tft.print("FINGER");

  tft.setTextColor(0x8410);
  tft.setTextSize(2);
  tft.setCursor(20, 180);
  tft.print("Touch sensor...");
}

void Screen::showWaitingForCard() {
  tft.fillScreen(0x0000);

  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(20, 60);
  tft.print("Please scan");

  tft.setTextColor(0x07E0);
  tft.setTextSize(3);
  tft.setCursor(20, 110);
  tft.print("RFID CARD");

  tft.setTextColor(0x8410);
  tft.setTextSize(2);
  tft.setCursor(20, 180);
  tft.print("Waiting...");
}

void Screen::initTouch() {
  touch.begin();
  touch.setRotation(1);  // 與螢幕方向一致
}

bool Screen::isTouched() {
  return touch.touched();
}

void Screen::getTouchPoint(int16_t &x, int16_t &y) {
  if (touch.touched()) {
    TS_Point p = touch.getPoint();
    
    // 映射觸控座標到螢幕像素（根據校準參數）
    x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
    
    // 限制範圍
    x = constrain(x, 0, tft.width() - 1);
    y = constrain(y, 0, tft.height() - 1);
  }
}

void Screen::printTouchDebug() {
  if (touch.touched()) {
    TS_Point p = touch.getPoint();
    Serial.printf("Raw: X=%d Y=%d Z=%d\n", p.x, p.y, p.z);
    
    int16_t x, y;
    getTouchPoint(x, y);
    Serial.printf("Mapped: X=%d Y=%d\n", x, y);
  }
}

void Screen::showMainMenu() {
  tft.fillScreen(0x0000);
  
  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(40, 10);
  tft.print("Select Method");
  
  // 繪製按鈕（簡化版）
  // 第一排
  tft.drawRect(10, 40, 140, 80, 0x07E0);
  tft.setCursor(40, 70);
  tft.print("FINGER");
  
  tft.drawRect(160, 40, 140, 80, 0x07E0);
  tft.setCursor(190, 70);
  tft.print("RFID");
  
  // 第二排
  tft.drawRect(10, 130, 140, 80, 0x07E0);
  tft.setCursor(30, 160);
  tft.print("PASSWORD");
  
  tft.drawRect(160, 130, 140, 80, 0x07E0);
  tft.setCursor(190, 160);
  tft.print("FACE");
  
  // 第三排
  tft.drawRect(10, 220, 140, 80, 0xFFE0);
  tft.setCursor(30, 250);
  tft.print("ENROLL");
  
  tft.drawRect(160, 220, 140, 80, 0x8410);
  tft.setCursor(180, 250);
  tft.print("SETTING");
}

void Screen::showPasswordInput() {
  tft.fillScreen(0x0000);
  
  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(60, 10);
  tft.print("Enter Password");
  
  // 密碼顯示區域
  tft.drawRect(10, 40, 300, 40, 0xFFFF);
  
  // 繪製數字鍵盤 (3x4)
  const int keyW = 100, keyH = 50;
  const int startX = 10, startY = 100;
  
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 3; col++) {
      int x = startX + col * keyW;
      int y = startY + row * keyH;
      
      tft.drawRect(x, y, keyW - 5, keyH - 5, 0x8410);
      
      // 數字標籤
      String label;
      if (row < 3) {
        label = String(row * 3 + col + 1);
      } else {
        if (col == 0) label = "#";  // 清除
        else if (col == 1) label = "0";
        else label = "*";  // 確認
      }
      
      tft.setCursor(x + 40, y + 15);
      tft.print(label);
    }
  }
}

void Screen::updatePasswordDisplay(String maskedPW) {
  // 清除密碼顯示區域
  tft.fillRect(12, 42, 296, 36, 0x0000);
  
  tft.setTextColor(0xFFFF);
  tft.setTextSize(3);
  tft.setCursor(20, 50);
  
  // 顯示星號
  for (int i = 0; i < maskedPW.length(); i++) {
    tft.print("*");
  }
}

int8_t Screen::getKeypadPress(int16_t x, int16_t y) {
  const int keyW = 100, keyH = 50;
  const int startX = 10, startY = 100;
  
  // 檢查是否在鍵盤區域內
  if (x < startX || y < startY) return -1;
  
  int col = (x - startX) / keyW;
  int row = (y - startY) / keyH;
  
  if (col < 0 || col > 2 || row < 0 || row > 3) return -1;
  
  // 計算按鍵值
  if (row < 3) {
    return row * 3 + col + 1;  // 1-9
  } else {
    if (col == 0) return 10;  // # (清除)
    else if (col == 1) return 0;
    else return 11;  // * (確認)
  }
}

void Screen::showCalibrationMarkers() {
  tft.fillScreen(0x0000);
  
  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(40, 100);
  tft.print("Touch Calibration");
  
  // 繪製標記點
  tft.fillCircle(10, 10, 5, 0xF800);
  tft.fillCircle(tft.width() - 10, 10, 5, 0xF800);
  tft.fillCircle(10, tft.height() - 10, 5, 0xF800);
  tft.fillCircle(tft.width() - 10, tft.height() - 10, 5, 0xF800);
}

void Screen::showSuccess() {
  tft.fillScreen(0x0000);

  tft.setTextColor(0x07E0);
  tft.setTextSize(3);
  tft.setCursor(40, 100);
  tft.print("SUCCESS");

  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(30, 160);
  tft.print("Door unlocked");
}

void Screen::showFailed() {
  tft.fillScreen(0x0000);

  tft.setTextColor(0xF800);   // 紅色（RGB565）
  tft.setTextSize(3);
  tft.setCursor(55, 100);
  tft.print("FAILED");

  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(20, 160);
  tft.print("Try again");
}
