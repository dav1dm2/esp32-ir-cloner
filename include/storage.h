#pragma once
#include <Arduino.h>
#include "config.h"

// ---------------- Stored signals ----------------
struct Signal {
  uint16_t len = 0;
  uint16_t data[kCaptureBufferSize];
};

extern Signal sig[kButtonCount];
extern const char* kKeyName[kButtonCount];

void saveSignal(int i);
void loadSignals();
