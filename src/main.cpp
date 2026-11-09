/*
 * IR Clone test project (ESP32)
 * Library: IRremoteESP8266 by crankyoncrimson (declared in platformio.ini)
 *
 * Buttons (wired between GPIO and GND, internal pull-ups are used):
 *   Button 0 -> "ON"  signal
 *   Button 1 -> "OFF" signal
 *
 * Short press          : transmit the signal stored for that button
 * Hold for 3 seconds   : enter assign mode for that button
 * Assign mode          : built-in LED blinks slowly for up to 5 seconds,
 *                        first valid IR signal received is stored
 * Feedback             : 3 quick blinks = saved
 *                        1 long blink   = nothing saved / nothing stored yet
 *
 * Signals are kept in flash (Preferences/NVS), so they survive a reboot.
 */

#include <Arduino.h>
#include "config.h"
#include "storage.h"
#include "ir_controller.h"
#include "button.h"

// ---------------- Finite State Machine ----------------
enum FsmState {
  STATE_TRANSMIT,
  STATE_ASSIGNING
};

static FsmState currentState = STATE_TRANSMIT;
static int targetButton = 0;

void setup() {
  Serial.begin(115200);
  pinMode(kLedPin, OUTPUT);
  initButtons();
  initIR();
  loadSignals();
  Serial.println("Ready. Short press = transmit, hold 3 s = assign.");
}

void loop() {
  switch (currentState) {
    case STATE_TRANSMIT:
      for (size_t i = 0; i < kButtonCount; i++) {
        ButtonEvent evt = handleButton(i);
        if (evt == BUTTON_EVENT_SHORT_PRESS) {
          transmitSignal(i);
        } else if (evt == BUTTON_EVENT_HOLD) {
          targetButton = i;
          currentState = STATE_ASSIGNING;
          break;
        }
      }
      break;

    case STATE_ASSIGNING:
      assignSignal(targetButton);
      currentState = STATE_TRANSMIT;
      break;
  }
}