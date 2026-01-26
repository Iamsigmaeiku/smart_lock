#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "config.h"

class Screen {
public:
  Screen();
  void init();

  void showWelcome();
  void showWaitingForFinger();
  void showSuccess();
  void showFailed();

private:
  Adafruit_ILI9341 tft;
};

#endif
