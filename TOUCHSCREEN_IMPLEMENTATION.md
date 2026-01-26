# 觸控密碼功能實施總結

## 已完成的工作

### 1. 硬體配置 ✅

**文件**: `include/config.h`

添加了：
- 觸控接腳定義 (TOUCH_CS, TOUCH_IRQ)
- 觸控校準參數 (TS_MINX, TS_MAXX, TS_MINY, TS_MAXY)

```cpp
#define TOUCH_CS   15
#define TOUCH_IRQ  2
#define TS_MINX 300
#define TS_MAXX 3800
#define TS_MINY 400
#define TS_MAXY 3750
```

### 2. 庫依賴 ✅

**文件**: `platformio.ini`

添加了：
- `paulstoffregen/XPT2046_Touchscreen@^1.4`

### 3. 密碼管理模組 ✅

**新文件**: 
- `include/password.h`
- `src/password.cpp`

功能：
- ✅ 密碼存儲在 ESP32 Preferences (持久化)
- ✅ 密碼驗證
- ✅ 密碼修改 (需提供舊密碼)
- ✅ 錯誤次數限制 (3次)
- ✅ 自動鎖定 (連續錯誤3次後鎖定30秒)
- ✅ 預設密碼: "1234"

### 4. Screen 類擴展 ✅

**文件**: 
- `include/screen.h`
- `src/screen.cpp`

新增功能：

**觸控相關**:
- `initTouch()` - 初始化觸控
- `isTouched()` - 檢測觸摸
- `getTouchPoint(x, y)` - 獲取觸摸座標（已映射）
- `printTouchDebug()` - 調試工具

**UI功能**:
- `showMainMenu()` - 主選單（4個按鈕：指紋/RFID/密碼/人臉）
- `showPasswordInput()` - 密碼輸入介面（3x4數字鍵盤）
- `updatePasswordDisplay()` - 更新密碼顯示（星號遮罩）
- `getKeypadPress(x, y)` - 按鍵檢測
- `showCalibrationMarkers()` - 校準測試標記

**私有方法**:
- `drawKey()` - 繪製按鍵（可按下效果）

### 5. 主程式整合 ✅

**文件**: `src/main.cpp`

修改：
- ✅ 添加 `Password pwManager` 物件
- ✅ 擴展 `SystemState` enum (新增 MENU, PASSWORD_INPUT)
- ✅ 擴展 `AuthMethod` enum (新增 PASSWORD)
- ✅ setup() 中初始化密碼和觸控
- ✅ 啟動時顯示主選單而非直接等待輸入

**新狀態機邏輯**:

```
MENU (主選單)
  ├─ 觸摸"指紋" → WAITING_INPUT (指紋模式)
  ├─ 觸摸"RFID" → WAITING_INPUT (RFID模式)
  ├─ 觸摸"密碼" → PASSWORD_INPUT
  └─ 觸摸"人臉" → WAITING_INPUT (人臉模式)

PASSWORD_INPUT (密碼輸入)
  ├─ 檢查鎖定狀態
  ├─ 數字鍵 (0-9) → 添加到密碼
  ├─ # 鍵 → 清除密碼
  └─ * 鍵 → 驗證密碼
      ├─ 成功 → UNLOCKING
      └─ 失敗 → 返回 PASSWORD_INPUT 或 MENU

UNLOCKING (開鎖)
  └─ 持續 5 秒 → 自動上鎖 → MENU
```

### 6. 校準測試工具 ✅

**新文件**:
- `test/touch_calibration_test.cpp` - 校準測試程式
- `test/CALIBRATION_GUIDE.md` - 校準指南

功能：
- 顯示5個標記點（四角+中心）
- 印出觸摸的原始座標和映射座標
- 用於調整 config.h 中的校準參數

## 使用流程

### 正常使用

1. **啟動** → 顯示歡迎畫面 (2秒)
2. **主選單** → 4個選項：
   - 🖐️ 指紋辨識
   - 💳 RFID卡片
   - 🔢 輸入密碼
   - 👤 人臉辨識

3. **選擇密碼** → 數字鍵盤：
   ```
   [1] [2] [3]
   [4] [5] [6]
   [7] [8] [9]
   [#] [0] [*]
   
   # = 清除
   * = 確認
   ```

4. **輸入密碼** (4-8位數字)

5. **驗證**：
   - ✅ 成功 → 綠色畫面 → 開鎖 5 秒 → 自動上鎖 → 回主選單
   - ❌ 失敗 → 紅色畫面 → 重新輸入
   - 🔒 錯誤3次 → 鎖定 30 秒 → 返回主選單

### 首次設置

1. **校準觸控**：
   ```bash
   # 使用校準測試程式
   cp test/touch_calibration_test.cpp src/main.cpp.test
   # 按照 test/CALIBRATION_GUIDE.md 執行
   ```

2. **修改預設密碼**：
   - 首次啟動時密碼為 "1234"
   - 可以通過 `pwManager.changePassword(oldPW, newPW)` 修改

## 安全特性

- ✅ 密碼長度限制 (4-8位)
- ✅ 僅支持數字密碼
- ✅ 密碼以星號遮罩顯示
- ✅ 錯誤嘗試計數 (最多3次)
- ✅ 自動鎖定機制 (30秒)
- ✅ 密碼持久化存儲 (Preferences)
- ⚠️ 密碼明文存儲（未來可加密）

## 下一步優化建議

### 必要優化

1. **觸控校準**: 根據實際硬體調整 config.h 參數
2. **UI調整**: 根據實際顯示效果微調按鍵位置和大小
3. **防抖優化**: 如果有重複觸發，調整延遲時間

### 可選增強

1. **密碼加密**: 使用 mbedtls 對密碼進行哈希
2. **多組密碼**: 支持管理員密碼和用戶密碼
3. **密碼修改介面**: 長按螢幕進入設定模式
4. **觸控反饋**: 按鍵按下時改變顏色或震動
5. **日誌記錄**: 記錄每次解鎖嘗試
6. **超時返回**: 長時間無操作自動返回主選單

## 文件結構

```
smart_lock/
├── include/
│   ├── config.h          [修改] 觸控接腳和校準參數
│   ├── screen.h          [修改] 觸控和UI方法
│   └── password.h        [新建] 密碼管理類
├── src/
│   ├── main.cpp          [修改] 整合密碼功能
│   ├── screen.cpp        [修改] 實現觸控和UI
│   └── password.cpp      [新建] 密碼管理實現
├── test/
│   ├── touch_calibration_test.cpp  [新建] 校準測試
│   └── CALIBRATION_GUIDE.md        [新建] 校準指南
├── platformio.ini        [修改] 添加觸控庫
└── TOUCHSCREEN_IMPLEMENTATION.md   [本文件]
```

## 編譯和上傳

```bash
# 清理並編譯
pio run -t clean
pio run

# 上傳到ESP32
pio run -t upload

# 監視序列埠
pio device monitor
```

## 故障排除

### 觸摸無反應

1. 檢查接線：
   - T_CS → GPIO15
   - T_IRQ → GPIO2
   - T_DIN → MOSI (GPIO23)
   - T_DO → MISO (GPIO19)
   - T_CLK → SCK (GPIO18)

2. 檢查序列埠輸出是否有 "初始化觸控完成"

3. 使用校準測試程式檢測觸控是否工作

### 觸摸位置不準

1. 執行校準測試（見 test/CALIBRATION_GUIDE.md）
2. 調整 config.h 中的校準參數
3. 確認 rotation 設定一致

### 密碼無法保存

1. 檢查 Preferences 是否初始化成功
2. 檢查序列埠是否有錯誤訊息
3. 嘗試使用 `pwManager.resetToDefault()`

### 按鍵反應慢或重複觸發

1. 增加防抖延遲（main.cpp 中的 delay()）
2. 檢查 lastTouchTime 邏輯
3. 降低觸控靈敏度

## 技術細節

### 觸控座標映射

```cpp
// 原始觸控座標範圍 (XPT2046)
X: 300 ~ 3800
Y: 400 ~ 3750

// 映射到螢幕像素 (320x240, rotation=1)
mappedX = map(rawX, TS_MINX, TS_MAXX, 0, 320)
mappedY = map(rawY, TS_MINY, TS_MAXY, 0, 240)
```

### 鍵盤佈局

```cpp
起始位置: (40, 80)
按鍵大小: 70x50
間距: 10

按鍵 [1]: (40, 80) ~ (110, 130)
按鍵 [2]: (120, 80) ~ (190, 130)
...
```

### 密碼存儲

```cpp
命名空間: "smartlock"
鍵: "password"
格式: String (明文)
```

## 完成日期

2026-01-25

## 作者

AI Assistant (Claude Sonnet 4.5) + User
