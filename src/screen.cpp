#include "screen.h"
#include "config.h"
#include <Arduino.h>

<<<<<<< Updated upstream
// ============================================
// 建構子
// ============================================
Screen::Screen()
  : tft(TFT_CS, TFT_DC, TFT_RST), ts(nullptr) {
}

// ============================================
// 初始化螢幕
// ============================================
void Screen::init() {
  tft.begin();
  tft.setRotation(0);      // 直立（240x320）
  tft.fillScreen(0x0000);
=======
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

// 腳位定義
#define TFT_CS   5
#define TFT_RST  22
#define TFT_DC   21

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
>>>>>>> Stashed changes
  tft.setTextWrap(false);
  Serial.println("ILI9341 初始化完成 (240x320)");
}

<<<<<<< Updated upstream
// ============================================
// 初始化觸控
// ============================================
void Screen::initTouch() {
  // 建立 XPT2046_Touchscreen 物件
  //   TOUCH_CS = 15   【觸控晶片選擇，定義在 config.h】
  //   TOUCH_IRQ = 2   【觸控中斷，可選】
  ts = new XPT2046_Touchscreen(TOUCH_CS, TOUCH_IRQ);
  
  ts->begin();
  ts->setRotation(0);  // 與螢幕 rotation 一致
  
  Serial.println("觸控初始化完成");
}

// ============================================
// 觸控檢測
// ============================================
bool Screen::isTouched() {
  if (ts == nullptr) return false;
  return ts->touched();
}

void Screen::getTouchPoint(int16_t &x, int16_t &y) {
  if (ts == nullptr) {
    x = 0;
    y = 0;
    return;
  }
  
  TS_Point p = ts->getPoint();
  
  // 座標映射（原始座標 → 螢幕像素）
  x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
  y = map(p.y, TS_MINY, TS_MAXY, 0, 320);
  
  // 限制範圍
  x = constrain(x, 0, 239);
  y = constrain(y, 0, 319);
}

void Screen::printTouchDebug() {
  if (ts != nullptr && ts->touched()) {
    TS_Point p = ts->getPoint();
    Serial.printf("原始: (%d, %d)  ", p.x, p.y);
    
    int16_t x, y;
    getTouchPoint(x, y);
    Serial.printf("映射: (%d, %d)\n", x, y);
  }
}

// ============================================
// 輔助繪圖函式：繪製圓角按鈕
// ============================================
void Screen::drawButton(int16_t x, int16_t y, int16_t w, int16_t h, 
                        uint16_t color, String text, bool pressed) {
  int16_t radius = 10;  // 圓角半徑
  
  if (pressed) {
    // 按下效果：深色填充
    tft.fillRoundRect(x, y, w, h, radius, color >> 2);  // 顏色變暗
    tft.drawRoundRect(x, y, w, h, radius, color);
  } else {
    // 正常效果：淺色填充 + 邊框
    tft.fillRoundRect(x, y, w, h, radius, color >> 3);
    tft.drawRoundRect(x, y, w, h, radius, color);
  }
  
  // 文字置中
  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  
  int16_t textW = text.length() * 12;  // 粗略估算文字寬度
  int16_t textX = x + (w - textW) / 2;
  int16_t textY = y + (h - 16) / 2;
  
  tft.setCursor(textX, textY);
  tft.print(text);
}

// ============================================
// 輔助繪圖函式：繪製校準標記點
// ============================================
void Screen::drawMarker(int16_t x, int16_t y) {
  tft.drawCircle(x, y, 5, 0xFFFF);
  tft.drawLine(x - 10, y, x + 10, y, 0xFFFF);
  tft.drawLine(x, y - 10, x, y + 10, 0xFFFF);
}

// ============================================
// 顯示歡迎畫面
// ============================================
void Screen::showWelcome() {
  tft.fillScreen(0x0000);
  delay(10);  // 給硬體時間清除

  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(70, 80);
  tft.print("Welcome");

  tft.setTextColor(0x07E0);
  tft.setTextSize(3);
  tft.setCursor(30, 140);
  tft.print("SMART LOCK");

  tft.setTextColor(0x8410);
  tft.setTextSize(2);
  tft.setCursor(40, 220);
=======
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
>>>>>>> Stashed changes
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
}

// ============================================
// 顯示主選單（5個圓角按鈕）
// ============================================
void Screen::showMainMenu() {
  tft.fillScreen(0x0000);
  delay(10);  // 給硬體時間清除
  
  // 標題
  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(65, 10);
  tft.print("Smart Lock");
  
  // 按鈕參數（垂直排列，優化間距）
  int16_t btnW = 220, btnH = 55;
  int16_t x = 10;
  
  // 繪製 4 個主按鈕（垂直排列，間距15像素）
  drawButton(x, 50, btnW, btnH, 0x07E0, "Finger", false);    // 綠色
  drawButton(x, 120, btnW, btnH, 0x07FF, "RFID", false);     // 青色
  drawButton(x, 190, btnW, btnH, 0xFFE0, "Password", false); // 黃色
  drawButton(x, 260, btnW, btnH, 0xF81F, "Face", false);     // 洋紅
}

// ============================================
// 顯示密碼輸入介面
// ============================================
void Screen::showPasswordInput() {
  tft.fillScreen(0x0000);
  delay(10);  // 給硬體時間清除
  
  // 標題
  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(20, 15);
  tft.print("Enter Password");
  
  // 密碼顯示區域（白色框）
  tft.drawRoundRect(20, 50, 200, 30, 5, 0xFFFF);
  
  // 繪製數字鍵盤
  int16_t keyW = 60, keyH = 50;
  int16_t startX = 30, startY = 95;
  int16_t gap = 10;
  
  String keys[4][3] = {
    {"1", "2", "3"},
    {"4", "5", "6"},
    {"7", "8", "9"},
    {"#", "0", "*"}
  };
  
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 3; col++) {
      int16_t x = startX + col * (keyW + gap);
      int16_t y = startY + row * (keyH + gap);
      drawButton(x, y, keyW, keyH, 0x4A49, keys[row][col], false);
    }
  }
}

// ============================================
// 更新密碼顯示（星號）
// ============================================
void Screen::updatePasswordDisplay(String maskedPW) {
  // 清除舊內容
  tft.fillRect(25, 55, 190, 20, 0x0000);
  
  // 顯示星號
  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(30, 57);
  tft.print(maskedPW);
}

// ============================================
// 按鍵檢測（返回 0-9 或特殊鍵）
// ============================================
int8_t Screen::getKeypadPress(int16_t x, int16_t y) {
  int16_t keyW = 60, keyH = 50;
  int16_t startX = 30, startY = 95;
  int16_t gap = 10;
  
  // 計算按了哪一行哪一列
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 3; col++) {
      int16_t kx = startX + col * (keyW + gap);
      int16_t ky = startY + row * (keyH + gap);
      
      if (x >= kx && x <= kx + keyW && y >= ky && y <= ky + keyH) {
        // 按鍵映射
        if (row < 3) return row * 3 + col + 1;  // 1-9
        else if (col == 0) return 10;            // # 鍵
        else if (col == 1) return 0;             // 0
        else return 11;                          // * 鍵
      }
    }
  }
  
  return -1;  // 沒按到
}

// ============================================
// 顯示等待指紋
// ============================================
void Screen::showWaitingForFinger() {
<<<<<<< Updated upstream
  tft.fillScreen(0x0000);
  delay(10);  // 給硬體時間清除

  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(30, 80);
  tft.print("Please verify");

  tft.setTextColor(0x07E0);
  tft.setTextSize(3);
  tft.setCursor(50, 140);
  tft.print("FINGER");

  tft.setTextColor(0x8410);
  tft.setTextSize(2);
  tft.setCursor(20, 220);
  tft.print("Touch sensor...");
=======
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
>>>>>>> Stashed changes
}

// ============================================
// 顯示等待 RFID 卡片
// ============================================
void Screen::showWaitingForCard() {
  tft.fillScreen(0x0000);
  delay(10);  // 給硬體時間清除

  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(20, 80);
  tft.print("Enroll new card");

  tft.setTextColor(0x07FF);  // 青色
  tft.setTextSize(3);
  tft.setCursor(20, 140);
  tft.print("RFID CARD");

  tft.setTextColor(0x8410);
  tft.setTextSize(2);
  tft.setCursor(10, 220);
  tft.print("Place card now...");
}

// ============================================
// 顯示成功畫面
// ============================================
void Screen::showSuccess() {
<<<<<<< Updated upstream
  tft.fillScreen(0x0000);
  delay(10);  // 給硬體時間清除

  tft.setTextColor(0x07E0);
  tft.setTextSize(3);
  tft.setCursor(40, 130);
  tft.print("SUCCESS");

  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(30, 190);
  tft.print("Door unlocked");
=======
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
>>>>>>> Stashed changes
}

// ============================================
// 顯示失敗畫面
// ============================================
void Screen::showFailed() {
<<<<<<< Updated upstream
  tft.fillScreen(0x0000);
  delay(10);  // 給硬體時間清除

  tft.setTextColor(0xF800);   // 紅色（RGB565）
  tft.setTextSize(3);
  tft.setCursor(55, 130);
  tft.print("FAILED");

  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(60, 190);
  tft.print("Try again");
=======
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
>>>>>>> Stashed changes
}

// ============================================
// 顯示校準標記點
// ============================================
void Screen::showCalibrationMarkers() {
  tft.fillScreen(0x0000);
  delay(10);  // 給硬體時間清除
  
  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(20, 150);
  tft.print("Touch Calibration");
  
  // 繪製 5 個標記點（四角 + 中心）
  drawMarker(10, 10);      // 左上
  drawMarker(230, 10);     // 右上
  drawMarker(10, 310);     // 左下
  drawMarker(230, 310);    // 右下
  drawMarker(120, 160);    // 中心
}
