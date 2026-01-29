// Animates white pixels to simulate flying through a star field
#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

// Use hardware SPI
TFT_eSPI tft = TFT_eSPI();

// With 1024 stars the update rate is ~65 frames per second
#define NSTARS 1024
uint8_t sx[NSTARS] = {};
uint8_t sy[NSTARS] = {};
uint8_t sz[NSTARS] = {};

uint8_t za, zb, zc, zx;

// Fast 0-255 random number generator from http://eternityforest.com/Projects/rng.php:
static inline __attribute__((always_inline)) uint8_t rng()
{
  zx++;
  za = (za ^ zc ^ zx);
  zb = (zb + za);
  zc = ((zc + (zb >> 1)) ^ za);
  return zc;
}

void setup() {
  za = random(256);
  zb = random(256);
  zc = random(256);
  zx = random(256);

<<<<<<< HEAD
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // fastSetup() must be used immediately before fastPixel() to prepare screen
  // It must be called after any other graphics drawing function call if fastPixel()
  // is to be called again
  //tft.fastSetup(); // Prepare plot window range for fast pixel plotting
}

void loop()
{
  unsigned long t0 = micros();
  uint8_t spawnDepthVariation = 255;

  for(int i = 0; i < NSTARS; ++i)
  {
    if (sz[i] <= 1)
    {
      sx[i] = 160 - 120 + rng();
      sy[i] = rng();
      sz[i] = spawnDepthVariation--;
=======
  switch (currentState) {
    case MENU: {
      // 檢測觸控選擇驗證方式
      if (display.isTouched()) {
        int16_t x, y;
        display.getTouchPoint(x, y);
        
        // 按鈕佈局 (使用 config.h 定義的常數):
        // 第一排: [指紋] [RFID]   (Y: 50-110)
        // 第二排: [密碼] [人臉]   (Y: 120-180)
        // 第三排: [註冊] [設定]   (Y: 190-250)
        
        // 第一排按鈕
        if (y >= MENU_BTN_ROW1_Y && y <= MENU_BTN_ROW1_Y + MENU_BTN_HEIGHT) {
          if (x >= MENU_BTN_LEFT_X && x <= MENU_BTN_LEFT_X + MENU_BTN_WIDTH) {
            // 指紋按鈕
            Serial.println("選擇：指紋驗證");
            lastAuthMethod = FINGERPRINT;
            currentState = WAITING_INPUT;
            display.showWaitingForFinger();
          } else if (x >= MENU_BTN_RIGHT_X && x <= MENU_BTN_RIGHT_X + MENU_BTN_WIDTH) {
            // RFID按鈕
            Serial.println("選擇：RFID驗證");
            lastAuthMethod = RFID_CARD;
            currentState = WAITING_INPUT;
            display.showWaitingForFinger();  // 可以改成專門的RFID提示畫面
          }
        }
        // 第二排按鈕
        else if (y >= MENU_BTN_ROW2_Y && y <= MENU_BTN_ROW2_Y + MENU_BTN_HEIGHT) {
          if (x >= MENU_BTN_LEFT_X && x <= MENU_BTN_LEFT_X + MENU_BTN_WIDTH) {
            // 密碼按鈕
            Serial.println("選擇：密碼驗證");
            lastAuthMethod = PASSWORD;
            currentState = PASSWORD_INPUT;
            display.showPasswordInput();
          } else if (x >= MENU_BTN_RIGHT_X && x <= MENU_BTN_RIGHT_X + MENU_BTN_WIDTH) {
            // 人臉按鈕
            Serial.println("選擇：人臉驗證");
            lastAuthMethod = FACE_RECOGNITION;
            currentState = WAITING_INPUT;
            display.showWaitingForFinger();  // 可以改成專門的人臉提示畫面
          }
        }
        // 第三排按鈕 (註冊和設定 - 未實作)
        else if (y >= MENU_BTN_ROW3_Y && y <= MENU_BTN_ROW3_Y + MENU_BTN_HEIGHT) {
          if (x >= MENU_BTN_LEFT_X && x <= MENU_BTN_LEFT_X + MENU_BTN_WIDTH) {
            Serial.println("選擇：註冊功能 (未實作)");
          } else if (x >= MENU_BTN_RIGHT_X && x <= MENU_BTN_RIGHT_X + MENU_BTN_WIDTH) {
            Serial.println("選擇：設定功能 (未實作)");
          }
        }
        
        delay(300);  // 防抖
      }
      break;
>>>>>>> f086b20adb13961bf760b7a36c4206a550bcf6ad
    }
    else
    {
      int old_screen_x = ((int)sx[i] - 160) * 256 / sz[i] + 160;
      int old_screen_y = ((int)sy[i] - 120) * 256 / sz[i] + 120;

      // This is a faster pixel drawing function for occasions where many single pixels must be drawn
      tft.drawPixel(old_screen_x, old_screen_y,TFT_BLACK);

      sz[i] -= 2;
      if (sz[i] > 1)
      {
        int screen_x = ((int)sx[i] - 160) * 256 / sz[i] + 160;
        int screen_y = ((int)sy[i] - 120) * 256 / sz[i] + 120;

        if (screen_x >= 0 && screen_y >= 0 && screen_x < 320 && screen_y < 240)
        {
          uint8_t r, g, b;
          r = g = b = 255 - sz[i];
          tft.drawPixel(screen_x, screen_y, tft.color565(r,g,b));
        }
        else
          sz[i] = 0; // Out of screen, die.
      }
    }
  }
  unsigned long t1 = micros();
  //static char timeMicros[8] = {};

 // Calculate frames per second
  Serial.println(1.0/((t1 - t0)/1000000.0));
}


