#pragma once
#include <Arduino.h>

enum ButtonEvent {
  BUTTON_EVENT_NONE,
  BUTTON_EVENT_SHORT_PRESS,
  BUTTON_EVENT_HOLD
};

void initButtons();
ButtonEvent handleButton(int i);
