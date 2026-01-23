#include <Arduino.h>
#include "config.h"
#include "fingerprint.h"
#include "screen.h"
#include "motor.h"
#include "rfid.h"
#include "wifi_comm.h"

// 硬體物件
Fingerprint fingerSensor;
Screen display;
Motor doorMotor;
RFID rfidReader;
WifiComm wifiModule;

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
  RFID_CARD
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
  switch (currentState) {
    case WAITING_INPUT:
      // TODO: 檢測指紋
      if (fingerSensor.detectFinger()) {
        Serial.println("檢測到指紋！");
        lastAuthMethod = FINGERPRINT;
        currentState = VERIFYING;
      }
      
      // TODO: 檢測 RFID 卡片
      if (rfidReader.detectCard()) {
        Serial.println("檢測到 RFID 卡片！");
        lastAuthMethod = RFID_CARD;
        currentState = VERIFYING;
      }
      break;
      
    case VERIFYING: {
      // TODO: 根據驗證方式進行驗證
      bool verified = false;
      
      if (lastAuthMethod == FINGERPRINT) {
        verified = fingerSensor.verifyFinger();
      } else if (lastAuthMethod == RFID_CARD) {
        verified = rfidReader.verifyCard();
      }
      
      if (verified) {
        Serial.println("驗證成功！");
        display.showSuccess();
        currentState = UNLOCKING;
        
        // TODO: 可以在這裡發送通知到伺服器
        // wifiModule.sendData("門鎖已開啟");
      } else {
        Serial.println("驗證失敗！");
        display.showFailed();
        delay(2000);
        currentState = WAITING_INPUT;
        display.showWaitingForFinger();
      }
      break;
    }
      
    case UNLOCKING:
      // TODO: 開鎖
      doorMotor.unlock();
      delay(UNLOCK_DURATION); // 保持開鎖狀態
      
      // TODO: 自動上鎖
      doorMotor.lock();
      currentState = WAITING_INPUT;
      display.showWaitingForFinger();
      break;
      
    default:
      break;
  }
  
  delay(100);
}
