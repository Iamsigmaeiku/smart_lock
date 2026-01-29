/*
 * 觸控校準測試程式
 * 
 * 用途：測試觸控螢幕並校準座標映射參數
 * 
 * 使用方法：
 * 1. 將此文件臨時重命名為 main.cpp（備份原main.cpp）
 * 2. 編譯並上傳到ESP32
 * 3. 打開序列埠監視器（115200 baud）
 * 4. 觸摸螢幕四角和中心的標記點
 * 5. 觀察序列埠輸出的原始座標值
 * 6. 根據輸出調整 config.h 中的校準參數：
 *    TS_MINX, TS_MAXX, TS_MINY, TS_MAXY
 * 7. 重新編譯測試，直到觸摸準確
 * 8. 完成後恢復原本的 main.cpp
 */

#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include "config.h"

// 創建螢幕和觸控對象
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

void setup() {
  Serial.begin(115200);
  Serial.println("================================");
  Serial.println("觸控校準測試程式");
  Serial.println("================================");
  
  // 初始化螢幕
  tft.begin();
  tft.setRotation(1);  // 橫向
  tft.fillScreen(ILI9341_BLACK);
  
  // 初始化觸控
  touch.begin();
  touch.setRotation(1);
  
  Serial.println("初始化完成");
  
  // 繪製校準標記
  drawCalibrationMarkers();
  
  Serial.println("\n請依序觸摸以下標記點：");
  Serial.println("1. 左上角 (紅色)");
  Serial.println("2. 右上角 (綠色)");
  Serial.println("3. 左下角 (藍色)");
  Serial.println("4. 右下角 (黃色)");
  Serial.println("5. 中心 (白色)");
  Serial.println("\n記錄每個點的原始座標值(Raw X, Raw Y)");
  Serial.println("然後調整 config.h 中的校準參數\n");
}

void loop() {
  if (touch.touched()) {
    TS_Point p = touch.getPoint();
    
    // 使用當前校準參數映射座標
    int16_t mappedX = map(p.x, TS_MINX, TS_MAXX, 0, 320);
    int16_t mappedY = map(p.y, TS_MINY, TS_MAXY, 0, 240);
    mappedX = constrain(mappedX, 0, 319);
    mappedY = constrain(mappedY, 0, 239);
    
    // 印出詳細資訊
    Serial.println("========================================");
    Serial.printf("原始座標: X=%d, Y=%d, Pressure=%d\n", p.x, p.y, p.z);
    Serial.printf("映射座標: X=%d, Y=%d\n", mappedX, mappedY);
    Serial.println("========================================\n");
    
    // 在觸摸點繪製小圓點
    tft.fillCircle(mappedX, mappedY, 3, ILI9341_MAGENTA);
    
    // 防抖
    delay(500);
  }
  
  delay(50);
}

void drawCalibrationMarkers() {
  tft.fillScreen(ILI9341_BLACK);
  
  uint16_t markerSize = 20;
  
  // 標題
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(40, 5);
  tft.print("Touch Calibration");
  
  // 左上角 (10, 10)
  tft.fillCircle(10, 30, markerSize, ILI9341_RED);
  tft.setTextSize(1);
  tft.setCursor(35, 25);
  tft.print("(10,30)");
  
  // 右上角 (310, 10)
  tft.fillCircle(310, 30, markerSize, ILI9341_GREEN);
  tft.setCursor(250, 25);
  tft.print("(310,30)");
  
  // 左下角 (10, 230)
  tft.fillCircle(10, 230, markerSize, ILI9341_BLUE);
  tft.setCursor(35, 225);
  tft.print("(10,230)");
  
  // 右下角 (310, 230)
  tft.fillCircle(310, 230, markerSize, ILI9341_YELLOW);
  tft.setCursor(240, 225);
  tft.print("(310,230)");
  
  // 中心 (160, 120)
  tft.fillCircle(160, 120, markerSize, ILI9341_WHITE);
  tft.setCursor(130, 145);
  tft.print("(160,120)");
  
  // 說明文字
  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(1);
  tft.setCursor(10, 210);
  tft.print("Touch each marker");
  tft.setCursor(10, 220);
  tft.print("Check Serial Monitor");
}
