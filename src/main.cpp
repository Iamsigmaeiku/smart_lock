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
  ENROLLING         // 註冊模式（註冊 RFID 卡片或指紋）
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
        
        /* ========== 選單按鈕佈局 ==========
         * 第一排：[指紋] [RFID]
         * 第二排：[密碼] [人臉]
         * 第三排：[註冊] [設定]
         */
        
        // 第一排：驗證方式
        if (y >= 40 && y <= 120) {
          if (x >= 10 && x <= 150) {
            // 指紋按鈕
            Serial.println("選擇：指紋驗證");
            lastAuthMethod = FINGERPRINT;
            currentState = WAITING_INPUT;
            display.showWaitingForFinger();
          } else if (x >= 160 && x <= 310) {
            // RFID按鈕
            Serial.println("選擇：RFID 驗證");
            lastAuthMethod = RFID_CARD;
            currentState = WAITING_INPUT;
            display.showWaitingForCard();
          }
        } 
        // 第二排：其他驗證方式
        else if (y >= 130 && y <= 210) {
          if (x >= 10 && x <= 150) {
            // 密碼按鈕
            Serial.println("選擇：密碼驗證");
            lastAuthMethod = PASSWORD;
            currentState = PASSWORD_INPUT;
            display.showPasswordInput();
          } else if (x >= 160 && x <= 310) {
            // 人臉按鈕
            Serial.println("選擇：人臉驗證");
            lastAuthMethod = FACE_RECOGNITION;
            currentState = WAITING_INPUT;
            display.showWaitingForFinger();  // TODO: 改成 showWaitingForFace()
          }
        }
        // 第三排：註冊與設定
        else if (y >= 220 && y <= 300) {
          if (x >= 10 && x <= 150) {
            // 註冊按鈕
            Serial.println("選擇：註冊 RFID 卡片");
            lastAuthMethod = RFID_CARD;
            currentState = ENROLLING;
            display.showWaitingForCard();
          } else if (x >= 160 && x <= 310) {
            // 設定按鈕
            Serial.println("選擇：系統設定（未實作）");
            // TODO: 進入設定選單
          }
        }
        
        delay(300);  // 防抖
      }
      break;
    }
    
    case WAITING_INPUT: {
      /* ========== 等待感應器輸入 ==========
       * 根據 lastAuthMethod 決定要檢測哪種輸入
       * 如果用戶點擊了其他區域，可以返回選單
       */
      
      // 檢查是否點擊返回
      if (display.isTouched()) {
        int16_t x, y;
        display.getTouchPoint(x, y);
        
        // 左下角返回按鈕區域（可選）
        if (x < 80 && y > 220) {
          Serial.println("返回選單");
          lastAuthMethod = NONE;
          currentState = MENU;
          display.showMainMenu();
          delay(300);
          break;
        }
      }
      
      // 根據選擇的驗證方式檢測對應的輸入
      if (lastAuthMethod == FINGERPRINT) {
        if (fingerSensor.detectFinger()) {
          Serial.println("✓ 檢測到指紋！");
          currentState = VERIFYING;
        }
      } 
      else if (lastAuthMethod == RFID_CARD) {
        if (rfidReader.detectCard()) {
          Serial.println("✓ 檢測到 RFID 卡片！");
          currentState = VERIFYING;
        }
      }
      else if (lastAuthMethod == FACE_RECOGNITION) {
        if (aiCamera.detectFace()) {
          Serial.println("✓ 檢測到人臉！");
          currentState = VERIFYING;
        }
      }
      break;
    }

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
      /* ========== 驗證身份 ==========
       * 根據 lastAuthMethod 呼叫對應的驗證函式
       * 
       * 注意：
       * - rfidReader.verifyCard() 內部會重新讀取 UID 並比對
       * - 因為在 WAITING_INPUT 已經呼叫過 detectCard()
       * - 所以這裡會再讀一次卡片（確保資料正確）
       */
      
      bool verified = false;
      
      Serial.println("正在驗證身份...");

      if (lastAuthMethod == FINGERPRINT) {
        verified = fingerSensor.verifyFinger();
      } 
      else if (lastAuthMethod == RFID_CARD) {
        // verifyCard() 內部會呼叫 detectCard() + readCardUID() + 比對
        verified = rfidReader.verifyCard();
      } 
      else if (lastAuthMethod == FACE_RECOGNITION) {
        verified = aiCamera.verifyFace();
      }

      if (verified) {
        Serial.println("✓✓✓ 驗證成功！開門！");
        display.showSuccess();
        delay(1000);  // 讓用戶看到成功訊息

        // 立刻開鎖
        doorMotor.unlock();
        unlockStartMs = millis();
        isUnlocking = true;
        currentState = UNLOCKING;
      } else {
        Serial.println("❌ 驗證失敗！拒絕存取！");
        display.showFailed();
        delay(2000);

        // 返回選單
        lastAuthMethod = NONE;
        currentState = MENU;
        display.showMainMenu();
      }
      break;
    }

    case UNLOCKING:
      /* ========== 開鎖狀態 ==========
       * 維持開鎖 UNLOCK_DURATION 毫秒後自動上鎖
       */
      if (isUnlocking && (millis() - unlockStartMs >= UNLOCK_DURATION)) {
        Serial.println("自動上鎖");
        doorMotor.lock();
        isUnlocking = false;

        // 返回選單
        lastAuthMethod = NONE;
        currentState = MENU;
        display.showMainMenu();
      }
      break;

    case ENROLLING: {
      /* ========== 註冊模式 ==========
       * 註冊新的 RFID 卡片或指紋
       * 
       * TODO: 可以擴充為選單式註冊（選擇要註冊指紋還是卡片）
       * 目前只支援 RFID 卡片註冊
       */
      
      // 檢查是否點擊取消
      if (display.isTouched()) {
        int16_t x, y;
        display.getTouchPoint(x, y);
        
        // 左下角取消按鈕
        if (x < 80 && y > 220) {
          Serial.println("取消註冊");
          lastAuthMethod = NONE;
          currentState = MENU;
          display.showMainMenu();
          delay(300);
          break;
        }
      }
      
      // 等待 RFID 卡片
      if (lastAuthMethod == RFID_CARD) {
        if (rfidReader.detectCard()) {
          Serial.println("✓ 檢測到卡片，開始註冊...");
          
          // enrollCard() 內部會讀取 UID 並儲存
          if (rfidReader.enrollCard()) {
            display.showSuccess();
            delay(2000);
            Serial.println("返回選單");
          } else {
            display.showFailed();
            delay(2000);
            Serial.println("註冊失敗，返回選單");
          }
          
          // 返回選單
          lastAuthMethod = NONE;
          currentState = MENU;
          display.showMainMenu();
        }
      }
      // TODO: 指紋註冊
      else if (lastAuthMethod == FINGERPRINT) {
        // fingerSensor.enrollFinger();
      }
      
      break;
    }

    case IDLE:
    case LOCKED:
    default:
      break;
  }
}


/* ============================================================================
 * 📚 智慧鎖系統架構說明
 * ============================================================================
 * 
 * 【狀態機設計】
 * ----------------------------------------
 * IDLE → MENU → WAITING_INPUT → VERIFYING → UNLOCKING → MENU
 *         ↑                                      ↓
 *         └──────── ENROLLING ←─────────────────┘
 * 
 * 
 * 【RFID 完整流程】
 * ----------------------------------------
 * 1. 用戶點擊 [RFID] 按鈕
 *    → currentState = WAITING_INPUT
 *    → lastAuthMethod = RFID_CARD
 * 
 * 2. WAITING_INPUT 狀態
 *    → 持續呼叫 rfidReader.detectCard()
 *    → 偵測到卡片後進入 VERIFYING
 * 
 * 3. VERIFYING 狀態
 *    → 呼叫 rfidReader.verifyCard()
 *    → 內部會重新讀取 UID 並與已註冊列表比對
 *    → 成功 → UNLOCKING
 *    → 失敗 → 回到 MENU
 * 
 * 4. UNLOCKING 狀態
 *    → 馬達開鎖
 *    → 維持 5 秒
 *    → 自動上鎖並回到 MENU
 * 
 * 
 * 【註冊流程】
 * ----------------------------------------
 * 1. 用戶點擊 [註冊] 按鈕
 *    → currentState = ENROLLING
 * 
 * 2. ENROLLING 狀態
 *    → 等待卡片靠近
 *    → 呼叫 rfidReader.enrollCard()
 *    → 成功/失敗顯示後回到 MENU
 * 
 * 
 * 【多種驗證方式】
 * ----------------------------------------
 * - 指紋辨識 (Fingerprint)
 * - RFID 卡片 (RFID Card)
 * - 人臉辨識 (Face Recognition)
 * - 密碼輸入 (Password)
 * - WiFi 遠端控制 (透過 wifi_comm)
 * 
 * 
 * 【安全性考量】
 * ----------------------------------------
 * ⚠️ 目前實作：
 * - RFID 只驗證 UID（可被複製）
 * - 卡片資料儲存在 RAM（重開機會遺失）
 * 
 * 🔒 建議改進：
 * 1. 加入 MIFARE 認證（金鑰驗證）
 * 2. 儲存到 EEPROM/Flash（持久化）
 * 3. 多重驗證（RFID + 密碼）
 * 4. 記錄存取日誌（時間戳 + UID）
 * 5. 遠端管理（透過 WiFi 新增/刪除卡片）
 * 
 * ============================================================================ */