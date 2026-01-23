#include "screen.h"
#include "config.h"

void Screen::init() {
  // TODO: 初始化 ILI9341 螢幕
  // 使用 SPI 介面和 config.h 定義的接腳
  // CS=5, RST=22, DC=21, MOSI=23, SCK=18, MISO=19
  Serial.println("初始化螢幕...");
}

void Screen::showWelcome() {
  // TODO: 顯示歡迎畫面
  Serial.println("顯示：歡迎使用智慧門鎖");
}

void Screen::showWaitingForFinger() {
  // TODO: 顯示等待指紋畫面
  Serial.println("顯示：請放置指紋");
}

void Screen::showSuccess() {
  // TODO: 顯示驗證成功畫面
  Serial.println("顯示：驗證成功");
}

void Screen::showFailed() {
  // TODO: 顯示驗證失敗畫面
  Serial.println("顯示：驗證失敗");
}
