#ifndef CONFIG_H
#define CONFIG_H

// ========================================
// 接腳定義 (根據你的硬體設定)
// ========================================

// SG90 伺服馬達接腳
#define MOTOR_PIN 13  // 橘色線 (PWM 訊號)

// AS608 指紋模組接腳 (UART2)
#define FINGERPRINT_RX 16  // ESP32 RX <- AS608 TX
#define FINGERPRINT_TX 17  // ESP32 TX -> AS608 RX

// ILI9341 螢幕接腳 (SPI)
#define TFT_CS   5    // Chip Select
#define TFT_RST  22   // Reset
#define TFT_DC   21   // Data/Command
#define TFT_MOSI 23   // Master Out Slave In
#define TFT_SCLK 18   // Clock
#define TFT_MISO 19   // Master In Slave Out (選用)

// RFID RC522 接腳 (SPI - 與螢幕共用 MOSI/MISO/SCLK)
#define RFID_CS   4   // Chip Select (SDA)
#define RFID_RST  27  // Reset

// XPT2046 觸控接腳 (SPI - 與螢幕共用 MOSI/MISO/SCLK)
#define TOUCH_CS   15   // 觸控晶片選擇
#define TOUCH_IRQ  2    // 觸控中斷（可選）

// 觸控校準參數（需根據實際硬體調整）
#define TS_MINX 300
#define TS_MAXX 3800
#define TS_MINY 400
#define TS_MAXY 3750

// HUSKYLENS AI 辨識鏡頭接腳 (I2C)
#define HUSKYLENS_SDA 25  // I2C 資料線
#define HUSKYLENS_SCL 26  // I2C 時鐘線
#define HUSKYLENS_I2C_ADDR 0x32  // I2C 地址（預設）

// ========================================
// 系統設定
// ========================================
#define SERIAL_BAUD 115200
#define UNLOCK_DURATION 5000  // 開鎖持續時間 (毫秒)

#endif
