#include <Arduino.h>
#include "config.h"
#include "fingerprint.h"
#include "screen.h"
#include "motor.h"
#include "rfid.h"
#include "huskylens.h"
#include "wifi_comm.h"
#include "password.h"

uint32_t unlockStartMs = 0;
bool isUnlocking = false;

// 硬體物件
Fingerprint fingerSensor;
Screen display;
Motor doorMotor;
RFID rfidReader;
HuskyLens aiCamera;
wifi_comm wifiModule;
Password pwManager;

// 系統狀態
enum SystemState {
  IDLE,             // 閒置
  MENU,             // 選單（選擇驗證方式）
  WAITING_INPUT,    // 等待輸入 (指紋或 RFID)
  PASSWORD_INPUT,   // 密碼輸入中
  VERIFYING,        // 驗證中
  UNLOCKING,        // 開鎖中
  LOCKED,           // 已上鎖
  ENROLLING         // 註冊新卡片
};

SystemState currentState = IDLE;

// 驗證方式
enum AuthMethod {
  NONE,
  FINGERPRINT,
  RFID_CARD,
  FACE_RECOGNITION,
  PASSWORD          // 密碼驗證
};

AuthMethod lastAuthMethod = NONE;

// 主程式初始化
void setup() {
  // 串口初始化
  Serial.begin(SERIAL_BAUD);
  Serial.println("================================");
  Serial.println("智慧門鎖系統啟動中...");
  Serial.println("================================");

  // 初始化硬體模組
  fingerSensor.init();
  display.init();
  display.initTouch();
  doorMotor.init(MOTOR_PIN);
  rfidReader.init();
  aiCamera.init();
  wifiModule.init();
  pwManager.init();

  // 顯示歡迎畫面
  display.showWelcome();
  delay(2000);

  // 進入選單狀態
  currentState = MENU;
  display.showMainMenu();

  Serial.println("系統準備就緒！");
}

// 主程式循環
void loop() {
  // 如果你的 wifi_comm 是 non-blocking 設計，建議每圈都呼叫一次
  wifiModule.update();

  switch (currentState) {
    case MENU: {
      // 檢測觸控選擇驗證方式
      if (display.isTouched()) {
        int16_t x, y;
        display.getTouchPoint(x, y);
        
        // 按鈕佈局（圓角按鈕）：
        // [指紋 140x80]   [RFID 140x80]
        // [密碼 140x80]   [人臉 140x80]
        //     [註冊卡片 200x50]
        
        int16_t btnW = 140, btnH = 80;
        int16_t x1 = 10, x2 = 170;
        int16_t y1 = 40, y2 = 130;
        
        // 檢測 2x2 主按鈕
        if (x >= x1 && x <= x1 + btnW && y >= y1 && y <= y1 + btnH) {
          // 指紋按鈕（左上）
          Serial.println("選擇：指紋驗證");
          lastAuthMethod = FINGERPRINT;
          currentState = WAITING_INPUT;
          display.showWaitingForFinger();
        } else if (x >= x2 && x <= x2 + btnW && y >= y1 && y <= y1 + btnH) {
          // RFID 按鈕（右上）
          Serial.println("選擇：RFID驗證");
          lastAuthMethod = RFID_CARD;
          currentState = WAITING_INPUT;
          display.showWaitingForFinger();  // 可以改成專門的RFID提示畫面
        } else if (x >= x1 && x <= x1 + btnW && y >= y2 && y <= y2 + btnH) {
          // 密碼按鈕（左下）
          Serial.println("選擇：密碼驗證");
          lastAuthMethod = PASSWORD;
          currentState = PASSWORD_INPUT;
          display.showPasswordInput();
        } else if (x >= x2 && x <= x2 + btnW && y >= y2 && y <= y2 + btnH) {
          // 人臉按鈕（右下）
          Serial.println("選擇：人臉驗證");
          lastAuthMethod = FACE_RECOGNITION;
          currentState = WAITING_INPUT;
          display.showWaitingForFinger();  // 可以改成專門的人臉提示畫面
        } else if (x >= 60 && x <= 260 && y >= 220 && y <= 270) {
          // 註冊卡片按鈕（底部中間）
          Serial.println("選擇：註冊新卡片");
          currentState = ENROLLING;
          display.showWaitingForCard();
        }
        
        delay(300);  // 防抖
      }
      break;
    }
    
    case WAITING_INPUT:
      // 檢測指紋
      if (fingerSensor.detectFinger()) {
        Serial.println("檢測到指紋！");
        lastAuthMethod = FINGERPRINT;
        currentState = VERIFYING;
      }

      // 檢測 RFID 卡片
      if (rfidReader.detectCard()) {
        Serial.println("檢測到 RFID 卡片！");
        lastAuthMethod = RFID_CARD;
        currentState = VERIFYING;
      }
      
      // TODO: 檢測人臉
      if (aiCamera.detectFace()) {
        Serial.println("檢測到人臉！");
        lastAuthMethod = FACE_RECOGNITION;
        currentState = VERIFYING;
      }
      break;

    case PASSWORD_INPUT: {
      static String enteredPW = "";
      static unsigned long lastTouchTime = 0;
      
      // 檢查是否被鎖定
      if (pwManager.isLocked()) {
        Serial.println("系統已鎖定");
        display.showFailed();
        delay(2000);
        
        enteredPW = "";
        currentState = MENU;
        display.showMainMenu();
        break;
      }
      
      if (display.isTouched()) {
        // 防抖：200ms內的觸摸視為同一次
        if (millis() - lastTouchTime < 200) {
          break;
        }
        lastTouchTime = millis();
        
        int16_t x, y;
        display.getTouchPoint(x, y);
        int8_t key = display.getKeypadPress(x, y);
        
        if (key >= 0 && key <= 9) {
          // 數字鍵
          if (enteredPW.length() < 8) {  // 最多8位
            enteredPW += String(key);
            display.updatePasswordDisplay(enteredPW);
            Serial.printf("輸入數字: %d (長度: %d)\n", key, enteredPW.length());
          }
        } else if (key == 10) {
          // # 鍵 = 清除
          enteredPW = "";
          display.updatePasswordDisplay(enteredPW);
          Serial.println("清除密碼");
        } else if (key == 11) {
          // * 鍵 = 確認
          Serial.printf("確認密碼: %s\n", enteredPW.c_str());
          
          if (enteredPW.length() < 4) {
            // 密碼太短
            display.showFailed();
            delay(1500);
            enteredPW = "";
            display.showPasswordInput();
          } else {
            // 驗證密碼
            bool verified = pwManager.verifyPassword(enteredPW);
            
            if (verified) {
              Serial.println("密碼驗證成功！");
              display.showSuccess();
              
              // 立刻開鎖
              doorMotor.unlock();
              unlockStartMs = millis();
              isUnlocking = true;
              currentState = UNLOCKING;
              enteredPW = "";
            } else {
              Serial.println("密碼驗證失敗！");
              display.showFailed();
              delay(2000);
              
              enteredPW = "";
              
              // 如果被鎖定，返回選單；否則重新輸入
              if (pwManager.isLocked()) {
                currentState = MENU;
                display.showMainMenu();
              } else {
                display.showPasswordInput();
              }
            }
          }
        }
      }
      break;
    }

    case VERIFYING: {
      bool verified = false;

      if (lastAuthMethod == FINGERPRINT) {
        verified = fingerSensor.verifyFinger();
      } else if (lastAuthMethod == RFID_CARD) {
        verified = rfidReader.verifyCard();
      } else if (lastAuthMethod == FACE_RECOGNITION) {
        verified = aiCamera.verifyFace();
      }

      if (verified) {
        Serial.println("驗證成功！");
        display.showSuccess();

        // 立刻開鎖（不等下一圈）
        doorMotor.unlock();
        unlockStartMs = millis();
        isUnlocking = true;
        currentState = UNLOCKING;
      } else {
        Serial.println("驗證失敗！");
        display.showFailed();
        delay(2000);

        lastAuthMethod = NONE;
        currentState = MENU;
        display.showMainMenu();
      }
      break;
    }

    case UNLOCKING:
      if (isUnlocking && (millis() - unlockStartMs >= UNLOCK_DURATION)) {
        doorMotor.lock();
        isUnlocking = false;

        lastAuthMethod = NONE;
        currentState = MENU;
        display.showMainMenu();
      }
      break;

    case ENROLLING:
      // 等待感應 RFID 卡片並註冊
      if (rfidReader.detectCard()) {
        Serial.println("偵測到卡片，開始註冊...");
        
        if (rfidReader.enrollCard()) {
          // 註冊成功
          Serial.println("✓ 卡片註冊成功！");
          display.showSuccess();
          delay(2000);
        } else {
          // 註冊失敗（已註冊過或已滿）
          Serial.println("✗ 卡片註冊失敗");
          display.showFailed();
          delay(2000);
        }
        
        // 返回主選單
        currentState = MENU;
        display.showMainMenu();
      }
      break;

    case IDLE:
    case LOCKED:
    default:
      break;
  }
}