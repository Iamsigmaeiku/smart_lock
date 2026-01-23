#include "wifi_comm.h"
#include "config.h"

void WifiComm::init() {
  // TODO: 初始化 ESP8266 通訊
  // 需要設定 UART 串口和 AT 指令
  Serial.println("初始化 WiFi 通訊模組...");
}

bool WifiComm::connectWiFi(const char* ssid, const char* password) {
  // TODO: 連接到指定的 WiFi
  // 使用 AT 指令控制 ESP8266
  return false;
}

bool WifiComm::sendData(const char* data) {
  // TODO: 發送資料到伺服器
  return false;
}

bool WifiComm::receiveData(char* buffer, size_t bufferSize) {
  // TODO: 接收伺服器傳來的資料
  return false;
}

bool WifiComm::isConnected() {
  // TODO: 檢查 WiFi 連線狀態
  return false;
}
