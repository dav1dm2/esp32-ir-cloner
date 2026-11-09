#pragma once
#include <Arduino.h>

// ---------------- Pins ----------------
constexpr uint16_t kRecvPin = 14;  // IR receiver module OUT
constexpr uint16_t kSendPin = 4;   // IR emitter LED (through resistor)
constexpr uint8_t  kLedPin  = 2;   // built-in LED on most ESP32 dev boards

constexpr uint8_t kButtonPins[] = {25, 26};  // GPIO 25 = ON, GPIO 26 = OFF
constexpr size_t  kButtonCount  = sizeof(kButtonPins) / sizeof(kButtonPins[0]);

// ---------------- Timing ----------------
constexpr uint32_t kHoldMs     = 3000;  // hold time to enter assign mode
constexpr uint32_t kAssignMs   = 5000;  // length of the assign window
constexpr uint32_t kDebounceMs = 30;

// ---------------- IR settings ----------------
constexpr uint16_t kCaptureBufferSize = 1024;
constexpr uint8_t  kCaptureTimeoutMs  = 50;
constexpr uint16_t kMinRaw            = 20;    // ignore tiny noise captures
constexpr uint16_t kCarrierKHz        = 38;
