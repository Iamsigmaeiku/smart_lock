#include "wifi_comm.h"
#include "config.h"
#define WIFI_SSID "COMMMM"
#define WIFI_PASSWORD "8765432100"
#include "WiFiMulti.h"

WiFiMulti wifimulti
void WifiComm::init() {
  Serial.begin(115200);
  wifimulti.addAP(WIFI_SSID,WIFI_PASSWORD);
  Serial.println("初始化 WiFi 通訊模組...");
  while(wifimulti.run() != WL_CONNECTED){
    delay(100);
  }
  Serial.println("connected");
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
