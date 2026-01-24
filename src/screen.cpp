#include "screen.h"
#include "config.h"
#include <Arduino.h>
#include <SPI.h>
#define TFT_CS   5
#define TFT_RST  22
#define TFT_DC   21
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_MISO 19  // 只讀螢幕可不接

void Screen::init() {
  // 初始化 SPI 和腳位
  pinMode(TFT_CS, OUTPUT);
  pinMode(TFT_DC, OUTPUT);
  pinMode(TFT_RST, OUTPUT);
  digitalWrite(TFT_CS, HIGH);
  digitalWrite(TFT_DC, HIGH);
  
  SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, TFT_CS);
  SPI.setFrequency(40000000);  // 40MHz
  
  // 硬體 Reset
  digitalWrite(TFT_RST, HIGH);
  delay(5);
  digitalWrite(TFT_RST, LOW);
  delay(20);
  digitalWrite(TFT_RST, HIGH);
  delay(150);
  
  // 1. Software Reset
  sendCommand(0x01);
  
  // 2. Delay 5ms
  delay(5);
  
  // === 電源與時序控制（確保螢幕穩定供電）===
  
  // Power Control A - 優化電源時序
  sendCommand(0xCB);
  sendData(0x39);
  sendData(0x2C);
  sendData(0x00);
  sendData(0x34);
  sendData(0x02);
  
  // Power Control B - 設定放電路徑
  sendCommand(0xCF);
  sendData(0x00);
  sendData(0xC1);
  sendData(0x30);
  
  // Driver timing control A - 優化驅動時序
  sendCommand(0xE8);
  sendData(0x85);
  sendData(0x00);
  sendData(0x78);
  
  // Driver timing control B - 設定閘極驅動時序
  sendCommand(0xEA);
  sendData(0x00);
  sendData(0x00);
  
  // Power on sequence control - 電源開啟順序
  sendCommand(0xED);
  sendData(0x64);
  sendData(0x03);
  sendData(0x12);
  sendData(0x81);
  
  // Pump ratio control - 電壓泵比例
  sendCommand(0xF7);
  sendData(0x20);
  
  // Power Control 1 - GVDD 電壓（影響對比度）
  sendCommand(0xC0);
  sendData(0x23);
  
  // Power Control 2 - VGH/VGL 電壓
  sendCommand(0xC1);
  sendData(0x10);
  
  // VCOM Control 1 - VCOM 電壓（防止閃爍）
  sendCommand(0xC5);
  sendData(0x3E);
  sendData(0x28);
  
  // VCOM Control 2 - VCOM 偏移
  sendCommand(0xC7);
  sendData(0x86);
  
  // === 顯示時序設定 ===
  
  // Frame Rate Control - 設定刷新率 70Hz
  sendCommand(0xB1);
  sendData(0x00);
  sendData(0x18);
  
  // Display Function Control - 掃描方式與顯示行數
  sendCommand(0xB6);
  sendData(0x08);
  sendData(0x82);
  sendData(0x27);
  
  // === Gamma 校正（顏色準確度）===
  
  // Enable 3G - 關閉 3-Gamma 功能
  sendCommand(0xF2);
  sendData(0x00);
  
  // Gamma Set - 選擇 Gamma 曲線 1
  sendCommand(0x26);
  sendData(0x01);
  
  // Positive Gamma Correction - 正極 Gamma 校正
  sendCommand(0xE0);
  sendData(0x0F);
  sendData(0x31);
  sendData(0x2B);
  sendData(0x0C);
  sendData(0x0E);
  sendData(0x08);
  sendData(0x4E);
  sendData(0xF1);
  sendData(0x37);
  sendData(0x07);
  sendData(0x10);
  sendData(0x03);
  sendData(0x0E);
  sendData(0x09);
  sendData(0x00);
  
  // Negative Gamma Correction - 負極 Gamma 校正
  sendCommand(0xE1);
  sendData(0x00);
  sendData(0x0E);
  sendData(0x14);
  sendData(0x03);
  sendData(0x11);
  sendData(0x07);
  sendData(0x31);
  sendData(0xC1);
  sendData(0x48);
  sendData(0x08);
  sendData(0x0F);
  sendData(0x0C);
  sendData(0x31);
  sendData(0x36);
  sendData(0x0F);
  
  // === 像素格式與掃描方向 ===
  
  // 3. Pixel Format Set - 設定顏色深度
  sendCommand(0x3A);
  sendData(0x55);     // 16-bit RGB565
  
  sendCommand(0x36);  // Memory Access Control
  sendData(0x48);     // BGR color order
  
  // 4. Sleep Out
  sendCommand(0x11);
  
  // 5. Delay 120ms
  delay(120);
  
  // 6. Clear Screen
  setAddress(0, 0, 239, 319);  // 設定全螢幕範圍 (240x320)
  sendCommand(0x2C);  // Memory Write
  // 寫入黑色像素清除螢幕 (240*320 像素)
  for (uint32_t i = 0; i < 240 * 320; i++) {
    sendData(0x00);  // 黑色高位元組
    sendData(0x00);  // 黑色低位元組
  }
  
  // 7. Display On
  sendCommand(0x29);
  
  Serial.println("init screen success");
}

void Screen::showWelcome() {
  // 1. 清空螢幕 (填滿黑色背景)
  fillScreen(0x0000);  // 黑色
  
  // 2. 顯示藍色矩形標題區
  fillRect(10, 40, 220, 60, 0x001F);  // 藍色矩形 (RGB565: 0x001F)
  
  // 3. 顯示白色文字 "Smart Lock"
  drawText(40, 55, "Smart Lock", 0xFFFF, 3);  // 白色，大小3
  
  // 4. 顯示提示文字
  drawText(30, 150, "Touch sensor", 0x07E0, 2);  // 綠色，大小2
  drawText(50, 180, "to unlock", 0x07E0, 2);
  
  Serial.println("welcome to smart lock");
}

void Screen::showWaitingForFinger() {
  // 清空螢幕
  fillScreen(0x0000);
  
  // 顯示黃色警示區
  fillRect(20, 80, 200, 80, 0xFFE0);  // 黃色 (RGB565: 0xFFE0)
  
  // 顯示提示文字
  drawText(40, 100, "Please place", 0x0000, 2);  // 黑色文字
  drawText(50, 130, "your finger", 0x0000, 2);
  
  Serial.println("顯示：請放置指紋");
}

void Screen::showSuccess() {
  // 清空螢幕
  fillScreen(0x0000);
  
  // 顯示綠色成功區
  fillRect(20, 80, 200, 100, 0x07E0);  // 綠色
  
  // 顯示成功文字
  drawText(60, 100, "Access", 0xFFFF, 3);  // 白色
  drawText(55, 135, "Granted!", 0xFFFF, 3);
  
  Serial.println("顯示：驗證成功");
}

void Screen::showFailed() {
  // 清空螢幕
  fillScreen(0x0000);
  
  // 顯示紅色失敗區
  fillRect(20, 80, 200, 100, 0xF800);  // 紅色 (RGB565: 0xF800)
  
  // 顯示失敗文字
  drawText(60, 100, "Access", 0xFFFF, 3);
  drawText(65, 135, "Denied!", 0xFFFF, 3);
  
  Serial.println("顯示：驗證失敗");
}

void Screen::sendCommand(uint8_t cmd) {
  digitalWrite(TFT_DC, LOW);   // DC=LOW 表示指令
  digitalWrite(TFT_CS, LOW);   // 選中螢幕
  SPI.transfer(cmd);
  digitalWrite(TFT_CS, HIGH);  // 取消選擇
}

void Screen::sendData(uint8_t data) {
  digitalWrite(TFT_DC, HIGH);  // DC=HIGH 表示資料
  digitalWrite(TFT_CS, LOW);   // 選中螢幕
  SPI.transfer(data);
  digitalWrite(TFT_CS, HIGH);  // 取消選擇
}

void Screen::setAddress(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  // Column Address Set (0x2A)
  sendCommand(0x2A);
  sendData(x1 >> 8);    // 起始列高位元組
  sendData(x1 & 0xFF);  // 起始列低位元組
  sendData(x2 >> 8);    // 結束列高位元組
  sendData(x2 & 0xFF);  // 結束列低位元組
  
  // Page Address Set (0x2B)
  sendCommand(0x2B);
  sendData(y1 >> 8);    // 起始頁高位元組
  sendData(y1 & 0xFF);  // 起始頁低位元組
  sendData(y2 >> 8);    // 結束頁高位元組
  sendData(y2 & 0xFF);  // 結束頁低位元組
}

void Screen::fillScreen(uint16_t color) {
  // 填滿整個螢幕 240x320
  setAddress(0, 0, 239, 319);
  sendCommand(0x2C);  // Memory Write
  for (uint32_t i = 0; i < 240 * 320; i++) {
    sendData(color >> 8);
    sendData(color & 0xFF);
  }
}

void Screen::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
  // 填滿指定矩形區域
  setAddress(x, y, x + w - 1, y + h - 1);
  sendCommand(0x2C);  // Memory Write
  for (uint32_t i = 0; i < (uint32_t)w * h; i++) {
    sendData(color >> 8);
    sendData(color & 0xFF);
  }
}

void Screen::drawText(uint16_t x, uint16_t y, const char* text, uint16_t color, uint8_t size) {
  // 簡化版：只畫文字區域的色塊（不實際渲染字型）
  // 完整實現需要字型表（如 Adafruit_GFX 的 glcdfont.c）
  uint16_t len = strlen(text);
  uint16_t w = len * 6 * size;  // 每個字元寬 6 像素
  uint16_t h = 8 * size;         // 高 8 像素
  fillRect(x, y, w, h, color);
}
