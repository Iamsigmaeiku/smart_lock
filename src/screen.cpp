#include "screen.h"
#include "config.h"

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
  tft.setRotation(1);      // 橫向（320x240）
  tft.fillScreen(0x0000);
  tft.setTextWrap(false);
}

// ============================================
// 初始化觸控
// ============================================
void Screen::initTouch() {
  // 建立 XPT2046_Touchscreen 物件
  //   TOUCH_CS = 15   【觸控晶片選擇，定義在 config.h】
  //   TOUCH_IRQ = 2   【觸控中斷，可選】
  ts = new XPT2046_Touchscreen(TOUCH_CS, TOUCH_IRQ);
  
  ts->begin();
  ts->setRotation(1);  // 與螢幕 rotation 一致
  
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
  x = map(p.x, TS_MINX, TS_MAXX, 0, 320);
  y = map(p.y, TS_MINY, TS_MAXY, 0, 240);
  
  // 限制範圍
  x = constrain(x, 0, 319);
  y = constrain(y, 0, 239);
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

// ============================================
// 顯示主選單（5個圓角按鈕）
// ============================================
void Screen::showMainMenu() {
  tft.fillScreen(0x0000);
  
  // 標題
  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(90, 10);
  tft.print("Smart Lock");
  
  // 按鈕參數
  int16_t btnW = 140, btnH = 80;
  int16_t x1 = 10, x2 = 170;
  int16_t y1 = 40, y2 = 130;
  
  // 繪製 4 個主按鈕
  drawButton(x1, y1, btnW, btnH, 0x07E0, "Finger", false);    // 綠色
  drawButton(x2, y1, btnW, btnH, 0x07FF, "RFID", false);      // 青色
  drawButton(x1, y2, btnW, btnH, 0xFFE0, "Password", false);  // 黃色
  drawButton(x2, y2, btnW, btnH, 0xF81F, "Face", false);      // 洋紅
  
  // 底部註冊按鈕（較小）
  drawButton(60, 220, 200, 50, 0x8410, "Enroll Card", false); // 灰色
}

// ============================================
// 顯示密碼輸入介面
// ============================================
void Screen::showPasswordInput() {
  tft.fillScreen(0x0000);
  
  // 標題
  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(80, 10);
  tft.print("Enter Password");
  
  // 密碼顯示區域（白色框）
  tft.drawRoundRect(40, 35, 240, 30, 5, 0xFFFF);
  
  // 繪製數字鍵盤
  int16_t keyW = 70, keyH = 50;
  int16_t startX = 40, startY = 80;
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
  tft.fillRect(45, 40, 230, 20, 0x0000);
  
  // 顯示星號
  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(50, 42);
  tft.print(maskedPW);
}

// ============================================
// 按鍵檢測（返回 0-9 或特殊鍵）
// ============================================
int8_t Screen::getKeypadPress(int16_t x, int16_t y) {
  int16_t keyW = 70, keyH = 50;
  int16_t startX = 40, startY = 80;
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

// ============================================
// 顯示等待 RFID 卡片
// ============================================
void Screen::showWaitingForCard() {
  tft.fillScreen(0x0000);

  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(20, 60);
  tft.print("Enroll new card");

  tft.setTextColor(0x07FF);  // 青色
  tft.setTextSize(3);
  tft.setCursor(40, 110);
  tft.print("RFID CARD");

  tft.setTextColor(0x8410);
  tft.setTextSize(2);
  tft.setCursor(20, 180);
  tft.print("Place card now...");
}

// ============================================
// 顯示成功畫面
// ============================================
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

// ============================================
// 顯示失敗畫面
// ============================================
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

// ============================================
// 顯示校準標記點
// ============================================
void Screen::showCalibrationMarkers() {
  tft.fillScreen(0x0000);
  
  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(40, 110);
  tft.print("Touch Calibration");
  
  // 繪製 5 個標記點（四角 + 中心）
  drawMarker(10, 10);      // 左上
  drawMarker(310, 10);     // 右上
  drawMarker(10, 230);     // 左下
  drawMarker(310, 230);    // 右下
  drawMarker(160, 120);    // 中心
}
