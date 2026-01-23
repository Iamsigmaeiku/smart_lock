#ifndef WIFI_COMM_H
#define WIFI_COMM_H

#include <Arduino.h>

// ESP8266 WiFi 通訊模組控制類
class WifiComm {
public:
  // 初始化
  void init();
  
  // 連接 WiFi
  bool connectWiFi(const char* ssid, const char* password);
  
  // 發送資料到伺服器
  bool sendData(const char* data);
  
  // 接收伺服器資料
  bool receiveData(char* buffer, size_t bufferSize);
  
  // 檢查連線狀態
  bool isConnected();

private:
  // TODO: 添加必要的成員變數
  // 例如: Serial 物件、連線狀態等
};

#endif
