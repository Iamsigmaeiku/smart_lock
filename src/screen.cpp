#include "screen.h"
#include "config.h"
#include <Adafruit_ILI9341.h>

// 創建 Adafruit_ILI9341 對象
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void Screen::init() {
  tft.begin();              // 執行完整初始化序列
  tft.setRotation(1);       // 橫向顯示（240寬x320高 -> 320寬x240高）
  tft.fillScreen(ILI9341_BLACK);
  Serial.println("初始化螢幕完成");
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
