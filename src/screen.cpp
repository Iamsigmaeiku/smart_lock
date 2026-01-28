#include "screen.h"
#include "config.h"
#include <Arduino.h>

// 顏色定義 (RGB565)
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define YELLOW  0xFFE0
#define CYAN    0x07FF
#define ORANGE  0xFD20
#define GRAY    0x8410

<<<<<<< Updated upstream
=======
// 腳位定義
#define TFT_CS   5
#define TFT_RST  22
#define TFT_DC   21

>>>>>>> Stashed changes
// 建構子
Screen::Screen()
  : tft(TFT_CS, TFT_DC, TFT_RST),
    ts(TOUCH_CS) {
}

// ========================================
// 初始化函數
// ========================================

void Screen::init() {
  tft.begin();
  tft.setRotation(0);  // 0=直立 (240x320)
  tft.fillScreen(BLACK);
  tft.setTextWrap(false);
  Serial.println("ILI9341 初始化完成 (240x320)");
}

void Screen::initTouch() {
  ts.begin();
  ts.setRotation(0);  // 與螢幕同步
  Serial.println("XPT2046 觸控初始化完成");
}

// ========================================
// UI 輔助函數
// ========================================

void Screen::drawButton(int16_t x, int16_t y, int16_t w, int16_t h, 
                        uint16_t color, const char* label, uint8_t textSize) {
  // 繪製按鈕背景
  tft.fillRoundRect(x, y, w, h, 8, color);
  
  // 繪製按鈕邊框
  tft.drawRoundRect(x, y, w, h, 8, WHITE);
  
  // 計算文字居中位置
  tft.setTextSize(textSize);
  int16_t textWidth = strlen(label) * 6 * textSize;
  int16_t textHeight = 8 * textSize;
  int16_t textX = x + (w - textWidth) / 2;
  int16_t textY = y + (h - textHeight) / 2;
  
  // 繪製文字
  tft.setCursor(textX, textY);
  tft.setTextColor(WHITE);
  tft.print(label);
}

// ========================================
// 顯示函數
// ========================================

void Screen::showWelcome() {
  // 1. 清空螢幕
  tft.fillScreen(BLACK);
  
  // 2. 繪製藍色標題區 (80-140)
  tft.fillRoundRect(20, 80, 200, 60, 10, BLUE);
  
  // 3. 顯示 "Smart Lock" 文字
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.setCursor(30, 100);
  tft.print("Smart Lock");
  
  // 4. 顯示綠色提示文字
  tft.setTextSize(2);
  tft.setTextColor(GREEN);
  tft.setCursor(30, 200);
  tft.print("Touch sensor");
  tft.setCursor(50, 230);
  tft.print("to unlock");
  
  Serial.println("顯示：歡迎畫面");
}

void Screen::showMainMenu() {
  // 1. 清空螢幕
  tft.fillScreen(BLACK);
  
  // 2. 標題
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(50, 20);
  tft.print("Smart Lock");
  
  // 3. 第一排按鈕
  drawButton(10, 60, 110, 60, RED, "Finger", 2);
  drawButton(130, 60, 110, 60, ORANGE, "RFID", 2);
  
  // 4. 第二排按鈕
  drawButton(10, 140, 110, 60, GREEN, "Password", 1);
  drawButton(130, 140, 110, 60, CYAN, "Face", 2);
  
  // 5. 底部註冊按鈕
  drawButton(10, 240, 220, 40, GRAY, "Enroll Card", 2);
  
  Serial.println("顯示：主選單");
}

void Screen::showPasswordInput() {
  // 1. 清空螢幕
  tft.fillScreen(BLACK);
  
  // 2. 標題
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 10);
  tft.print("Enter Password");
  
  // 3. 密碼顯示區（底部線）
  tft.drawLine(20, 55, 220, 55, WHITE);
  
  // 4. 繪製 3x4 數字鍵盤
  const char* keys[4][3] = {
    {"1", "2", "3"},
    {"4", "5", "6"},
    {"7", "8", "9"},
    {"*", "0", "#"}
  };
  
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 3; col++) {
      int16_t x = 10 + col * 75;
      int16_t y = 80 + row * 60;
      
      // 特殊鍵用不同顏色
      uint16_t color = BLUE;
      if (row == 3 && col == 0) color = GREEN;   // * 確認鍵
      if (row == 3 && col == 2) color = RED;     // # 清除鍵
      
      drawButton(x, y, 70, 50, color, keys[row][col], 3);
    }
  }
  
  Serial.println("顯示：密碼輸入介面");
}

void Screen::updatePasswordDisplay(String maskedPW) {
  // 清除舊的密碼顯示區（不清空整個螢幕）
  tft.fillRect(20, 30, 200, 20, BLACK);
  
  // 顯示新的遮罩密碼
  tft.setTextSize(3);
  tft.setTextColor(YELLOW);
  tft.setCursor(30, 30);
  
  // 顯示星號
  for (unsigned int i = 0; i < maskedPW.length(); i++) {
    tft.print("*");
  }
  
  Serial.printf("更新密碼顯示：%d 位\n", maskedPW.length());
<<<<<<< Updated upstream
}

int8_t Screen::getKeypadPress(int16_t x, int16_t y) {
  // 判斷 (x,y) 落在哪個按鈕
  // 返回 0-9 或 10(#清除) 或 11(*確認) 或 -1(無效)
  
  // 檢查是否在鍵盤區域內
  if (y < 80 || y > 310 || x < 10 || x > 235) {
    return -1;
  }
  
  // 計算按下的按鈕
  int col = (x - 10) / 75;  // 0, 1, 2
  int row = (y - 80) / 60;  // 0, 1, 2, 3
  
  // 邊界檢查
  if (col < 0 || col > 2 || row < 0 || row > 3) {
    return -1;
  }
  
  // 根據行列返回按鍵值
  if (row == 0) return col + 1;      // 1, 2, 3
  if (row == 1) return col + 4;      // 4, 5, 6
  if (row == 2) return col + 7;      // 7, 8, 9
  if (row == 3) {
    if (col == 0) return 11;         // * (確認)
    if (col == 1) return 0;          // 0
    if (col == 2) return 10;         // # (清除)
  }
  
  return -1;
=======
>>>>>>> Stashed changes
}

void Screen::showWaitingForFinger() {
  // 1. 清空螢幕
  tft.fillScreen(BLACK);
  
  // 2. 黃色警示區
  tft.fillRoundRect(20, 100, 200, 120, 10, YELLOW);
  
  // 3. 提示文字（黑色）
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.setCursor(35, 130);
  tft.print("Please place");
  tft.setCursor(45, 160);
  tft.print("your finger");
  
  Serial.println("顯示：等待指紋");
<<<<<<< Updated upstream
}

void Screen::showWaitingForCard() {
  // 1. 清空螢幕
  tft.fillScreen(BLACK);
  
  // 2. 青色警示區
  tft.fillRoundRect(20, 100, 200, 120, 10, CYAN);
  
  // 3. 提示文字（黑色）
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.setCursor(30, 120);
  tft.print("Enroll Card");
  tft.setCursor(35, 160);
  tft.print("Place card");
  tft.setCursor(50, 190);
  tft.print("now...");
  
  Serial.println("顯示：等待RFID卡片");
=======
>>>>>>> Stashed changes
}

void Screen::showSuccess() {
  // 1. 清空螢幕
  tft.fillScreen(BLACK);
  
  // 2. 綠色成功區
  tft.fillRoundRect(20, 100, 200, 120, 10, GREEN);
  
  // 3. 成功文字（白色）
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.setCursor(50, 130);
  tft.print("Access");
  tft.setCursor(45, 165);
  tft.print("Granted!");
  
  Serial.println("顯示：驗證成功");
}

// ============================================
// 顯示失敗畫面
// ============================================
void Screen::showFailed() {
  // 1. 清空螢幕
  tft.fillScreen(BLACK);
  
  // 2. 紅色失敗區
  tft.fillRoundRect(20, 100, 200, 120, 10, RED);
  
  // 3. 失敗文字（白色）
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.setCursor(50, 130);
  tft.print("Access");
  tft.setCursor(55, 165);
  tft.print("Denied!");
  
  Serial.println("顯示：驗證失敗");
}

// ========================================
// 觸控函數
// ========================================

bool Screen::isTouched() {
  return ts.touched();
}

void Screen::getTouchPoint(int16_t &x, int16_t &y) {
  if (!ts.touched()) {
    x = -1;
    y = -1;
    return;
  }
  
  TS_Point p = ts.getPoint();
  
  // 映射到螢幕座標 (考慮旋轉和校準)
  // rotation=0 (直立模式): 240寬 x 320高
  x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
  y = map(p.y, TS_MINY, TS_MAXY, 0, 320);
  
  // 限制在螢幕範圍內
  x = constrain(x, 0, 239);
  y = constrain(y, 0, 319);
}

void Screen::printTouchDebug() {
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    int16_t x, y;
    getTouchPoint(x, y);
    Serial.printf("原始: X=%d Y=%d | 映射: X=%d Y=%d\n", p.x, p.y, x, y);
  }
}

int8_t Screen::getKeypadPress(int16_t x, int16_t y) {
  // 判斷 (x,y) 落在哪個按鈕
  // 返回 0-9 或 10(#清除) 或 11(*確認) 或 -1(無效)
  
  // 檢查是否在鍵盤區域內
  if (y < 80 || y > 310 || x < 10 || x > 235) {
    return -1;
  }
  
  // 計算按下的按鈕
  int col = (x - 10) / 75;  // 0, 1, 2
  int row = (y - 80) / 60;  // 0, 1, 2, 3
  
  // 邊界檢查
  if (col < 0 || col > 2 || row < 0 || row > 3) {
    return -1;
  }
  
  // 根據行列返回按鍵值
  if (row == 0) return col + 1;      // 1, 2, 3
  if (row == 1) return col + 4;      // 4, 5, 6
  if (row == 2) return col + 7;      // 7, 8, 9
  if (row == 3) {
    if (col == 0) return 11;         // * (確認)
    if (col == 1) return 0;          // 0
    if (col == 2) return 10;         // # (清除)
  }
  
  return -1;
}

// ========================================
// 校準測試
// ========================================

void Screen::showCalibrationMarkers() {
  tft.fillScreen(BLACK);
  
  // 繪製四個角落的十字標記
  // 左上
  tft.drawFastHLine(10, 20, 20, RED);
  tft.drawFastVLine(20, 10, 20, RED);
  
  // 右上
  tft.drawFastHLine(210, 20, 20, RED);
  tft.drawFastVLine(220, 10, 20, RED);
  
  // 左下
  tft.drawFastHLine(10, 300, 20, RED);
  tft.drawFastVLine(20, 290, 20, RED);
  
  // 右下
  tft.drawFastHLine(210, 300, 20, RED);
  tft.drawFastVLine(220, 290, 20, RED);
  
  // 中心點
  tft.fillCircle(120, 160, 5, GREEN);
  
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(40, 150);
  tft.print("Touch markers for calibration");
  
  Serial.println("顯示：校準標記點");
}
