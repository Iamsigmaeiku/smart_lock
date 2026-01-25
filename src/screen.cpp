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
  tft.begin();              // 執行完整初始化序列
  tft.setRotation(1);       // 橫向顯示（240寬x320高 -> 320寬x240高）
  tft.fillScreen(ILI9341_BLACK);
  Serial.println("初始化螢幕完成");
}

void Screen::initTouch() {
  touch.begin();
  touch.setRotation(1);     // 與螢幕方向一致
  Serial.println("初始化觸控完成");
}

void Screen::showWelcome() {
  tft.fillScreen(ILI9341_BLACK);
  
  // 藍色矩形標題區
  tft.fillRect(10, 40, 220, 60, ILI9341_BLUE);
  
  // 白色文字 "Smart Lock"
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(40, 55);
  tft.print("Smart Lock");
  
  // 綠色提示文字
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.setCursor(30, 150);
  tft.print("Touch sensor");
  tft.setCursor(50, 180);
  tft.print("to unlock");
  
  Serial.println("顯示：歡迎使用智慧門鎖");
}

void Screen::showWaitingForFinger() {
  tft.fillScreen(ILI9341_BLACK);
  
  // 黃色警示區
  tft.fillRect(20, 80, 200, 80, ILI9341_YELLOW);
  
  // 黑色文字
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(40, 100);
  tft.print("Please place");
  tft.setCursor(50, 130);
  tft.print("your finger");
  
  Serial.println("顯示：請放置指紋");
}

void Screen::showSuccess() {
  tft.fillScreen(ILI9341_BLACK);
  
  // 綠色成功區
  tft.fillRect(20, 80, 200, 100, ILI9341_GREEN);
  
  // 白色文字
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(60, 100);
  tft.print("Access");
  tft.setCursor(55, 135);
  tft.print("Granted!");
  
  Serial.println("顯示：驗證成功");
}

void Screen::showFailed() {
  tft.fillScreen(ILI9341_BLACK);
  
  // 紅色失敗區
  tft.fillRect(20, 80, 200, 100, ILI9341_RED);
  
  // 白色文字
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(60, 100);
  tft.print("Access");
  tft.setCursor(65, 135);
  tft.print("Denied!");
  
  Serial.println("顯示：驗證失敗");
}

// ============================================
// 觸控功能實現
// ============================================

bool Screen::isTouched() {
  return touch.touched();
}

void Screen::getTouchPoint(int16_t &x, int16_t &y) {
  if (!touch.touched()) {
    x = -1;
    y = -1;
    return;
  }
  
  TS_Point p = touch.getPoint();
  
  // 映射觸控座標到螢幕像素（根據rotation=1）
  x = map(p.x, TS_MINX, TS_MAXX, 0, 320);
  y = map(p.y, TS_MINY, TS_MAXY, 0, 240);
  
  // 限制範圍
  x = constrain(x, 0, 319);
  y = constrain(y, 0, 239);
}

void Screen::printTouchDebug() {
  if (touch.touched()) {
    TS_Point p = touch.getPoint();
    int16_t x, y;
    getTouchPoint(x, y);
    
    Serial.printf("觸控 - 原始: X=%d, Y=%d, P=%d | 映射: X=%d, Y=%d\n", 
                  p.x, p.y, p.z, x, y);
  }
}

// ============================================
// UI功能實現
// ============================================

void Screen::showMainMenu() {
  tft.fillScreen(ILI9341_BLACK);
  
  // 標題
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(70, 10);
  tft.print("Select Method");
  
  // 四個按鈕
  uint16_t btnW = 140;
  uint16_t btnH = 80;
  uint16_t spacing = 10;
  
  // 指紋按鈕（左上）
  drawKey(10, 40, btnW, btnH, "Finger", ILI9341_BLUE);
  
  // RFID按鈕（右上）
  drawKey(10 + btnW + spacing, 40, btnW, btnH, "RFID", ILI9341_GREEN);
  
  // 密碼按鈕（左下）
  drawKey(10, 40 + btnH + spacing, btnW, btnH, "Password", ILI9341_ORANGE);
  
  // 人臉按鈕（右下）
  drawKey(10 + btnW + spacing, 40 + btnH + spacing, btnW, btnH, "Face", ILI9341_PURPLE);
  
  Serial.println("顯示：主選單");
}

void Screen::showPasswordInput() {
  tft.fillScreen(ILI9341_BLACK);
  
  // 標題
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(80, 10);
  tft.print("Enter PIN");
  
  // 密碼顯示區域（白色邊框）
  tft.drawRect(10, 35, 300, 30, ILI9341_WHITE);
  
  // 繪製數字鍵盤 3x4
  const uint16_t keyW = KeypadLayout::KEY_W;
  const uint16_t keyH = KeypadLayout::KEY_H;
  const uint16_t startX = KeypadLayout::START_X;
  const uint16_t startY = KeypadLayout::START_Y;
  const uint16_t spacing = KeypadLayout::SPACING;
  
  String keys[4][3] = {
    {"1", "2", "3"},
    {"4", "5", "6"},
    {"7", "8", "9"},
    {"#", "0", "*"}
  };
  
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 3; col++) {
      uint16_t x = startX + col * (keyW + spacing);
      uint16_t y = startY + row * (keyH + spacing);
      drawKey(x, y, keyW, keyH, keys[row][col], ILI9341_DARKGREY);
    }
  }
  
  // 說明文字
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(40, 70);
  tft.print("# = Clear  * = Enter");
  
  Serial.println("顯示：密碼輸入介面");
}

void Screen::updatePasswordDisplay(String maskedPW) {
  // 清除密碼顯示區域
  tft.fillRect(12, 37, 296, 26, ILI9341_BLACK);
  
  // 顯示遮罩密碼（圓點）
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(20, 40);
  
  for (int i = 0; i < maskedPW.length(); i++) {
    tft.print("*");  // 用星號代替圓點
  }
}

int8_t Screen::getKeypadPress(int16_t x, int16_t y) {
  const uint16_t keyW = KeypadLayout::KEY_W;
  const uint16_t keyH = KeypadLayout::KEY_H;
  const uint16_t startX = KeypadLayout::START_X;
  const uint16_t startY = KeypadLayout::START_Y;
  const uint16_t spacing = KeypadLayout::SPACING;
  
  // 檢查是否在鍵盤區域內
  if (x < startX || y < startY) {
    return -1;
  }
  
  // 計算按鍵位置
  int col = (x - startX) / (keyW + spacing);
  int row = (y - startY) / (keyH + spacing);
  
  if (col < 0 || col > 2 || row < 0 || row > 3) {
    return -1;
  }
  
  // 檢查是否真的在按鍵內（不是在間隙）
  uint16_t keyX = startX + col * (keyW + spacing);
  uint16_t keyY = startY + row * (keyH + spacing);
  
  if (x > keyX + keyW || y > keyY + keyH) {
    return -1;  // 點在間隙中
  }
  
  // 數字鍵盤佈局
  // [1][2][3]  -> 1,2,3
  // [4][5][6]  -> 4,5,6
  // [7][8][9]  -> 7,8,9
  // [#][0][*]  -> 10,0,11
  
  if (row == 0) return col + 1;      // 1,2,3
  if (row == 1) return col + 4;      // 4,5,6
  if (row == 2) return col + 7;      // 7,8,9
  if (row == 3) {
    if (col == 0) return 10;         // # (清除)
    if (col == 1) return 0;          // 0
    if (col == 2) return 11;         // * (確認)
  }
  
  return -1;
}

void Screen::showCalibrationMarkers() {
  tft.fillScreen(ILI9341_BLACK);
  
  // 四角標記
  uint16_t markerSize = 20;
  
  // 左上
  tft.fillCircle(10, 10, markerSize, ILI9341_RED);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.setCursor(35, 5);
  tft.print("(10,10)");
  
  // 右上
  tft.fillCircle(310, 10, markerSize, ILI9341_GREEN);
  tft.setCursor(250, 5);
  tft.print("(310,10)");
  
  // 左下
  tft.fillCircle(10, 230, markerSize, ILI9341_BLUE);
  tft.setCursor(35, 225);
  tft.print("(10,230)");
  
  // 右下
  tft.fillCircle(310, 230, markerSize, ILI9341_YELLOW);
  tft.setCursor(240, 225);
  tft.print("(310,230)");
  
  // 中心
  tft.fillCircle(160, 120, markerSize, ILI9341_WHITE);
  tft.setCursor(130, 145);
  tft.print("(160,120)");
  
  Serial.println("顯示：校準標記");
}

// ============================================
// 私有輔助函數
// ============================================

void Screen::drawKey(uint16_t x, uint16_t y, uint16_t w, uint16_t h, 
                      String label, uint16_t color, bool pressed) {
  // 繪製按鍵背景
  if (pressed) {
    tft.fillRect(x, y, w, h, color);
    tft.drawRect(x, y, w, h, ILI9341_WHITE);
  } else {
    tft.fillRect(x + 2, y + 2, w - 4, h - 4, color);
    tft.drawRect(x, y, w, h, ILI9341_WHITE);
  }
  
  // 繪製文字（居中）
  tft.setTextColor(ILI9341_WHITE);
  
  // 根據文字長度調整大小和位置
  if (label.length() > 4) {
    tft.setTextSize(2);
    tft.setCursor(x + w/2 - label.length() * 6, y + h/2 - 8);
  } else {
    tft.setTextSize(3);
    tft.setCursor(x + w/2 - label.length() * 9, y + h/2 - 12);
  }
  
  tft.print(label);
}
