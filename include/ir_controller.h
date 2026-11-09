#pragma once
#include <Arduino.h>

void initIR();
void blinkLed(uint8_t times, uint16_t ms);
void assignSignal(int i);
void transmitSignal(int i);
