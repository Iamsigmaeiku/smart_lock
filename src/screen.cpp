#include "screen.h"
#include "config.h"

// ILI9341 沒有無參數建構子，所以必須用初始化列表建構
Screen::Screen()
  : tft(TFT_CS, TFT_DC, TFT_RST) {
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
