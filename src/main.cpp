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

Fingerprint fingerSensor;
Screen display;
Motor doorMotor;
RFID rfidReader;
HuskyLens aiCamera;
wifi_comm wifiModule;
Password pwManager;

enum SystemState {
  IDLE,
  MENU,
  WAITING_INPUT,
  PASSWORD_INPUT,
  VERIFYING,
  UNLOCKING,
  LOCKED,
  ENROLLING
};

SystemState currentState = IDLE;

enum AuthMethod {
  NONE,
  FINGERPRINT,
  RFID_CARD,
  FACE_RECOGNITION,
  PASSWORD
};

AuthMethod lastAuthMethod = NONE;

void setup() {
  Serial.begin(SERIAL_BAUD);
  Serial.println("================================");
  Serial.println("智慧門鎖系統啟動中...");
  Serial.println("================================");

  fingerSensor.init();
  display.init();
  display.initTouch();
  doorMotor.init(MOTOR_PIN);
  rfidReader.init();
  aiCamera.init();
  wifiModule.init();
  pwManager.init();

  display.showWelcome();
  delay(2000);

  currentState = MENU;
  display.showMainMenu();

  Serial.println("系統準備就緒！");
}

void loop() {
  wifiModule.update();

  switch (currentState) {

    case MENU: {
      // 檢測觸控選擇驗證方式
      if (display.isTouched()) {
        int16_t x, y;
        display.getTouchPoint(x, y);
        
        // 按鈕佈局 (使用 config.h 定義的常數，直立模式 240x320):
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
            display.showWaitingForCard();
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
        // 第三排按鈕 (註冊和設定)
        else if (y >= MENU_BTN_ROW3_Y && y <= MENU_BTN_ROW3_Y + MENU_BTN_HEIGHT) {
          if (x >= MENU_BTN_LEFT_X && x <= MENU_BTN_LEFT_X + MENU_BTN_WIDTH) {
            // 註冊按鈕
            Serial.println("選擇：註冊卡片");
            currentState = ENROLLING;
            display.showWaitingForCard();
          } else if (x >= MENU_BTN_RIGHT_X && x <= MENU_BTN_RIGHT_X + MENU_BTN_WIDTH) {
            // 設定按鈕
            Serial.println("選擇：設定功能 (未實作)");
          }
        }
        
        delay(300);  // 防抖
      }
      break;
    }

    case WAITING_INPUT:
      if (fingerSensor.detectFinger()) {
        lastAuthMethod = FINGERPRINT;
        currentState = VERIFYING;
      }
      if (rfidReader.detectCard()) {
        lastAuthMethod = RFID_CARD;
        currentState = VERIFYING;
      }
      if (aiCamera.detectFace()) {
        lastAuthMethod = FACE_RECOGNITION;
        currentState = VERIFYING;
      }
      break;

    case PASSWORD_INPUT: {
      static String enteredPW = "";
      static unsigned long lastTouchTime = 0;

      if (pwManager.isLocked()) {
        display.showFailed();
        delay(2000);
        enteredPW = "";
        currentState = MENU;
        display.showMainMenu();
        break;
      }

      if (display.isTouched()) {
        if (millis() - lastTouchTime < 200) break;
        lastTouchTime = millis();

        int16_t x, y;
        display.getTouchPoint(x, y);
        int8_t key = display.getKeypadPress(x, y);

        if (key >= 0 && key <= 9) {
          if (enteredPW.length() < 8) {
            enteredPW += String(key);
            display.updatePasswordDisplay(enteredPW);
          }
        } else if (key == 10) {
          enteredPW = "";
          display.updatePasswordDisplay(enteredPW);
        } else if (key == 11) {
          if (enteredPW.length() < 4) {
            display.showFailed();
            delay(1500);
            enteredPW = "";
            display.showPasswordInput();
          } else {
            bool verified = pwManager.verifyPassword(enteredPW);
            if (verified) {
              display.showSuccess();
              doorMotor.unlock();
              unlockStartMs = millis();
              isUnlocking = true;
              currentState = UNLOCKING;
              enteredPW = "";
            } else {
              display.showFailed();
              delay(2000);
              enteredPW = "";
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
      if (lastAuthMethod == FINGERPRINT) verified = fingerSensor.verifyFinger();
      else if (lastAuthMethod == RFID_CARD) verified = rfidReader.verifyCard();
      else if (lastAuthMethod == FACE_RECOGNITION) verified = aiCamera.verifyFace();

      if (verified) {
        display.showSuccess();
        doorMotor.unlock();
        unlockStartMs = millis();
        isUnlocking = true;
        currentState = UNLOCKING;
      } else {
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
      if (rfidReader.detectCard()) {
        if (rfidReader.enrollCard()) display.showSuccess();
        else display.showFailed();
        delay(2000);
        currentState = MENU;
        display.showMainMenu();
      }
      break;

    default:
      break;
  }
}
