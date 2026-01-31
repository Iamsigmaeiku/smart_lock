/* ============================================================================
 * RFID 學習實驗程式
 * 
 * 🎯 用途：
 * 這個檔案包含了獨立的實驗範例，可以單獨編譯測試
 * 不會影響主專案的 main.cpp
 * 
 * 📝 使用方法：
 * 1. 將這個檔案的內容複製到 src/main.cpp
 * 2. 或者在 platformio.ini 中設定：
 *    build_src_filter = +<*> -<main.cpp> +<../test/rfid_experiments.cpp>
 * 
 * ⚠️ 注意：
 * 同一時間只能有一個 main.cpp 或這個檔案被編譯
 * ============================================================================ */

#include <Arduino.h>
#include "rfid.h"
#include "config.h"

RFID rfidReader;

// ========== 選擇要執行的實驗模式 ==========
enum ExperimentMode {
  EXP_VERSION,           // 實驗 1: 讀取版本號
  EXP_ANTENNA,           // 實驗 2: 天線控制
  EXP_MANUAL_REQA,       // 實驗 3: 手動 REQA
  EXP_DUMP_REGISTERS,    // 實驗 5: 印出暫存器
  EXP_CARD_TYPE,         // 實驗 6: 卡片類型偵測
  EXP_CONTINUOUS_READ,   // 連續讀卡模式
  EXP_ENROLL_MODE        // 註冊模式
};

// 修改這裡選擇實驗模式
const ExperimentMode currentMode = EXP_CONTINUOUS_READ;

void setup() {
  Serial.begin(115200);
  delay(2000);  // 等待 Serial Monitor 開啟
  
  Serial.println("\n\n");
  Serial.println("╔════════════════════════════════════════════════════╗");
  Serial.println("║       RFID RC522 學習實驗系統                      ║");
  Serial.println("║       Learn by First Principles                   ║");
  Serial.println("╚════════════════════════════════════════════════════╝");
  Serial.println();
  
  // 初始化 RFID
  rfidReader.init();
  
  delay(2000);
  
  // 根據模式執行對應的實驗
  switch (currentMode) {
    case EXP_VERSION:
      Serial.println("\n▶ 執行實驗 1: 讀取版本號");
      rfidReader.experiment_ReadVersion();
      break;
      
    case EXP_ANTENNA:
      Serial.println("\n▶ 執行實驗 2: 天線控制");
      Serial.println("將在 3 秒後關閉天線...");
      delay(3000);
      rfidReader.experiment_AntennaControl(false);
      
      Serial.println("將在 5 秒後重新開啟天線...");
      delay(5000);
      rfidReader.experiment_AntennaControl(true);
      break;
      
    case EXP_MANUAL_REQA:
      Serial.println("\n▶ 執行實驗 3: 手動 REQA");
      Serial.println("請在 5 秒內靠近卡片...");
      delay(5000);
      rfidReader.experiment_ManualREQA();
      break;
      
    case EXP_DUMP_REGISTERS:
      Serial.println("\n▶ 執行實驗 5: 印出所有暫存器");
      rfidReader.experiment_DumpRegisters();
      break;
      
    case EXP_CARD_TYPE:
      Serial.println("\n▶ 執行實驗 6: 卡片類型偵測");
      Serial.println("請在 5 秒內靠近卡片...");
      delay(5000);
      rfidReader.experiment_CardTypeDetection();
      break;
      
    case EXP_CONTINUOUS_READ:
      Serial.println("\n▶ 進入連續讀卡模式");
      Serial.println("請靠近卡片進行測試...\n");
      break;
      
    case EXP_ENROLL_MODE:
      Serial.println("\n▶ 進入註冊模式");
      Serial.println("請依序靠近要註冊的卡片...\n");
      break;
  }
}

void loop() {
  // 只有連續模式會在 loop 中持續執行
  if (currentMode == EXP_CONTINUOUS_READ) {
    // ========== 連續讀卡模式 ==========
    if (rfidReader.detectCard()) {
      Serial.println("\n" + String("=").repeat(50));
      Serial.println("✓ 偵測到卡片！");
      Serial.println(String("=").repeat(50));
      
      uint8_t uid[10];
      uint8_t len;
      
      if (rfidReader.readCardUID(uid, &len)) {
        Serial.print("UID 長度: ");
        Serial.print(len);
        Serial.println(" bytes");
        
        Serial.print("UID (HEX): ");
        for (int i = 0; i < len; i++) {
          if (uid[i] < 0x10) Serial.print("0");
          Serial.print(uid[i], HEX);
          if (i < len - 1) Serial.print(":");
        }
        Serial.println();
        
        Serial.print("UID (DEC): ");
        for (int i = 0; i < len; i++) {
          Serial.print(uid[i]);
          if (i < len - 1) Serial.print(" ");
        }
        Serial.println();
      }
      
      Serial.println(String("=").repeat(50));
      Serial.println();
      
      delay(2000);  // 防止重複讀取
    }
  }
  else if (currentMode == EXP_ENROLL_MODE) {
    // ========== 註冊模式 ==========
    static uint8_t enrollCount = 0;
    
    if (rfidReader.detectCard()) {
      Serial.println("\n" + String("-").repeat(50));
      
      if (rfidReader.enrollCard()) {
        enrollCount++;
        Serial.print("✓ 第 ");
        Serial.print(enrollCount);
        Serial.println(" 張卡片註冊成功！");
        
        if (enrollCount >= 3) {
          Serial.println("\n" + String("=").repeat(50));
          Serial.println("已註冊 3 張卡片，切換到驗證模式...");
          Serial.println(String("=").repeat(50));
          delay(2000);
          
          // 切換到驗證模式
          while (true) {
            if (rfidReader.detectCard()) {
              Serial.println("\n正在驗證...");
              if (rfidReader.verifyCard()) {
                Serial.println("✓✓✓ 驗證通過！歡迎使用！");
              } else {
                Serial.println("❌❌❌ 驗證失敗！拒絕存取！");
              }
              delay(2000);
            }
            delay(100);
          }
        } else {
          Serial.println("請靠近下一張要註冊的卡片...");
        }
      }
      
      Serial.println(String("-").repeat(50));
      delay(2000);
    }
  }
  else {
    // 其他實驗模式只執行一次，這裡閒置
    delay(1000);
  }
  
  delay(100);
}

/* ============================================================================
 * 💡 學習任務
 * ============================================================================
 * 
 * 【任務 1】基礎實驗（1-2 天）
 * --------------------------------------------------
 * 1. 依序執行實驗 1, 2, 5
 * 2. 理解每個暫存器的功能
 * 3. 記錄實驗結果到學習筆記
 * 4. 對照 Datasheet 查詢不懂的地方
 * 
 * 【任務 2】通訊協議（3-4 天）
 * --------------------------------------------------
 * 1. 執行實驗 3（手動 REQA）
 * 2. 理解 ISO 14443A 的通訊流程
 * 3. 修改程式，嘗試發送 WUPA (0x52)
 * 4. 觀察 ATQA 的格式與含義
 * 
 * 【任務 3】卡片管理（5-7 天）
 * --------------------------------------------------
 * 1. 執行實驗 6（卡片類型偵測）
 * 2. 測試不同類型的卡片
 * 3. 使用 EXP_ENROLL_MODE 註冊 3 張卡
 * 4. 測試驗證功能是否正常
 * 
 * 【任務 4】進階挑戰（2-3 週）
 * --------------------------------------------------
 * 1. 不使用 MFRC522 函式庫，從零實作讀卡功能
 * 2. 實作 MIFARE Classic 的扇區讀寫
 * 3. 研究 MIFARE 認證機制
 * 4. 嘗試破解預設金鑰（FF FF FF FF FF FF）
 * 
 * 【任務 5】整合應用（1 週）
 * --------------------------------------------------
 * 1. 整合到智慧門鎖專案
 * 2. 與指紋辨識、密碼等多重驗證結合
 * 3. 加入 WiFi 遠端管理功能
 * 4. 記錄刷卡歷史到 SD 卡或雲端
 * 
 * ============================================================================
 */

/* ============================================================================
 * 📚 除錯技巧
 * ============================================================================
 * 
 * 【技巧 1】使用 Serial.print 追蹤變數
 * --------------------------------------------------
 * 在關鍵步驟加上：
 * ```cpp
 * Serial.print("DEBUG: variable = ");
 * Serial.println(variable);
 * ```
 * 
 * 【技巧 2】檢查錯誤暫存器
 * --------------------------------------------------
 * 在每個操作後：
 * ```cpp
 * byte error = mfrc522->PCD_ReadRegister(ErrorReg);
 * if (error != 0) {
 *   Serial.print("ErrorReg = 0b");
 *   Serial.println(error, BIN);
 * }
 * ```
 * 
 * 【技巧 3】使用示波器
 * --------------------------------------------------
 * 觀察以下信號：
 * - SCK: 應該看到時鐘脈衝
 * - MOSI: 資料傳輸波形
 * - TX1/TX2: 13.56MHz 載波
 * 
 * 【技巧 4】對照時序圖
 * --------------------------------------------------
 * 參考 Datasheet Figure 21, 22 檢查：
 * - CS 是否正確拉低/拉高
 * - 資料傳輸順序是否正確
 * - 時鐘極性/相位是否符合 Mode 0
 * 
 * ============================================================================
 */
