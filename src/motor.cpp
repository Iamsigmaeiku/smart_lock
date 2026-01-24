#include <Arduino.h>
#include "motor.h"

void Motor::init(uint8_t pin) {
  _pin = pin;
  _servo.attach(_pin);
  _servo.write(0);
  delay(1000);
  Serial.println("初始化舵機...");
}

void Motor::unlock() {
  _servo.write(0);
  delay(1000);
  Serial.println("開鎖");
}

void Motor::lock() {
  _servo.write(90);
  delay(1000);
  Serial.println("上鎖");
}
