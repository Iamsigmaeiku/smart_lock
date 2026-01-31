#ifndef RFID_H
#define RFID_H

#include <Arduino.h>
#include <MFRC522.h>

/* ============================================================================
 * RFID RC522 控制類 - 完整可用版本
 * 
 * 📚 Datasheet 資源：
 * - MFRC522 Datasheet: https://www.nxp.com/docs/en/data-sheet/MFRC522.pdf
 * - ISO 14443A 標準: 13.56MHz RFID 通訊協議
 * - 函式庫來源: https://github.com/miguelbalboa/rfid
 * 
 * 💡 每個函式都標注了對應的 Datasheet 章節，方便學習
 * ============================================================================ */

class RFID {
public:
  // ========== 基礎操作 ==========
  
  /* 初始化 RC522 模組
   * 📖 Datasheet 參考: 
   *   - Section 8.1.1: CommandReg (p.36) - 軟體重置
   *   - Section 8.6.3: TxControlReg (p.45) - 開啟天線
   *   - Section 8.1.8: VersionReg (p.40) - 驗證晶片
   */
  void init();
  
  /* 檢測是否有卡片靠近
   * 📖 Datasheet: Section 9.3.3 REQA command (p.10)
   * 原理: 發送 REQA (0x26) 並接收 ATQA 回應
   */
  bool detectCard();
  
  /* 讀取卡片 UID
   * 📖 Datasheet: Section 9.3.4 Anticollision and Select (p.11-12)
   * 
   * @param uid       存放 UID 的緩衝區（需至少 10 bytes）
   * @param uidLength 存放 UID 長度（4/7/10 bytes）
   * @return          是否成功讀取
   */
  bool readCardUID(uint8_t* uid, uint8_t* uidLength);
  
  /* 驗證卡片是否已註冊 */
  bool verifyCard();
  
  /* 註冊新卡片 */
  bool enrollCard();

private:
  MFRC522* mfrc522;  // RC522 函式庫物件
  
  // 卡片儲存
  static const uint8_t MAX_CARDS = 10;
  static const uint8_t MAX_UID_LENGTH = 10;
  uint8_t registeredUIDs[MAX_CARDS][MAX_UID_LENGTH];
  uint8_t uidLengths[MAX_CARDS];
  uint8_t cardCount;
  
  // 輔助函式
  bool compareUID(const uint8_t* uid1, const uint8_t* uid2, uint8_t length);
  void printUID(const uint8_t* uid, uint8_t length);
};

#endif
