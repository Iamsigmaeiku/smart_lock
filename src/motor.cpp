#include "motor.h"

void Motor::init(uint8_t pin) {
  _pin = pin;
  // TODO: 初始化 SG90 舵機
  // 設定 PWM
  pinMode(_pin, OUTPUT);
  Serial.println("初始化舵機...");
}

void Motor::unlock() {
  // TODO: 控制舵機轉到開鎖位置
  Serial.println("開鎖");
}

void Motor::lock() {
  // TODO: 控制舵機轉到上鎖位置
  Serial.println("上鎖");
}
