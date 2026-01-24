#include <Arduino.h>
#include "config.h"
#include "fingerprint.h"
#include "screen.h"
#include "motor.h"
#include "rfid.h"
#include "huskylens.h"
#include "wifi_comm.h"
uint32_t unlockStartMs = 0;
bool isUnlocking = false;
// 硬體物件
Fingerprint fingerSensor;
Screen display;
Motor doorMotor;
RFID rfidReader;
HuskyLens aiCamera;
wifi_comm wifiModule;

// 系統狀態
enum SystemState {
  IDLE,             // 閒置
  WAITING_INPUT,    // 等待輸入 (指紋或 RFID)
  VERIFYING,        // 驗證中
  UNLOCKING,        // 開鎖中
  LOCKED            // 已上鎖
};

SystemState currentState = IDLE;

// 驗證方式
enum AuthMethod {
  NONE,
  FINGERPRINT,
  RFID_CARD,
  FACE_RECOGNITION
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
  doorMotor.init(MOTOR_PIN);
  rfidReader.init();
  aiCamera.init();
  wifiModule.init();

  // 顯示歡迎畫面
  display.showWelcome();
  delay(2000);

  // 進入等待狀態
  currentState = WAITING_INPUT;
  display.showWaitingForFinger();

  Serial.println("系統準備就緒！");
}

// 主程式循環
void loop() {
  // 如果你的 wifi_comm 是 non-blocking 設計，建議每圈都呼叫一次
  wifiModule.update();

  switch (currentState) {
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
        currentState = WAITING_INPUT;
        display.showWaitingForFinger();
      }
      break;
    }

    case UNLOCKING:

      if (isUnlocking && (millis() - unlockStartMs >= UNLOCK_DURATION)) {
        doorMotor.lock();
        isUnlocking = false;

        lastAuthMethod = NONE;
        currentState = WAITING_INPUT;
        display.showWaitingForFinger();
      }
      break;

    case IDLE:
    case LOCKED:
    default:
      break;
  }
}