#include "wifi_comm.h"
#include "config.h"

void WifiComm::init() {
  // TODO: 初始化 ESP32 內建 WiFi
  // WiFi.mode(WIFI_STA);
  Serial.println("初始化 WiFi 通訊模組...");
}

bool WifiComm::connectWiFi(const char* ssid, const char* password) {
  // TODO: 連接到指定的 WiFi
  // WiFi.begin(ssid, password);
  // 等待連線成功
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
