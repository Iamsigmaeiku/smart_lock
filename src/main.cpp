#include <Arduino.h>
#include "fingerprint.h"
#include "screen.h"
#include "motor.h"

// 定義接腳
#define MOTOR_PIN 13

// 硬體物件
Fingerprint fingerSensor;
Screen display;
Motor doorMotor;

// 系統狀態
enum SystemState {
  IDLE,           // 閒置
  WAITING_FINGER, // 等待指紋
  VERIFYING,      // 驗證中
  UNLOCKING,      // 開鎖中
  LOCKED          // 已上鎖
};

SystemState currentState = IDLE;

// 主程式初始化
void setup() {
  // 串口初始化
  Serial.begin(115200);
  Serial.println("================================");
  Serial.println("智慧門鎖系統啟動中...");
  Serial.println("================================");
  
  // 初始化硬體模組
  fingerSensor.init();
  display.init();
  doorMotor.init(MOTOR_PIN);
  
  // 顯示歡迎畫面
  display.showWelcome();
  delay(2000);
  
  // 進入等待狀態
  currentState = WAITING_FINGER;
  display.showWaitingForFinger();
  
  Serial.println("系統準備就緒！");
}

// 主程式循環
void loop() {
  switch (currentState) {
    case WAITING_FINGER:
      // 檢測是否有指紋
      if (fingerSensor.detectFinger()) {
        Serial.println("檢測到指紋！");
        currentState = VERIFYING;
      }
      break;
      
    case VERIFYING:
      // 驗證指紋
      if (fingerSensor.verifyFinger()) {
        Serial.println("驗證成功！");
        display.showSuccess();
        currentState = UNLOCKING;
      } else {
        Serial.println("驗證失敗！");
        display.showFailed();
        delay(2000);
        currentState = WAITING_FINGER;
        display.showWaitingForFinger();
      }
      break;
      
    case UNLOCKING:
      // 開鎖
      doorMotor.unlock();
      delay(5000); // 保持開鎖狀態 5 秒
      
      // 自動上鎖
      doorMotor.lock();
      currentState = WAITING_FINGER;
      display.showWaitingForFinger();
      break;
      
    default:
      break;
  }
  
  delay(100);
}
