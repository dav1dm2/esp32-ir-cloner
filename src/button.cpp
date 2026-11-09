#include "button.h"
#include "config.h"

// ---------------- Buttons ----------------
struct Button {
  uint8_t  pin;
  bool     down = false;
  uint32_t lastChange = 0;
  uint32_t pressedAt = 0;
  bool     longDone = false;
  explicit Button(uint8_t p) : pin(p) {}
};

static Button btn[kButtonCount] = {Button(kButtonPins[0]), Button(kButtonPins[1])};

void initButtons() {
  for (size_t i = 0; i < kButtonCount; i++) {
    pinMode(btn[i].pin, INPUT_PULLUP);
  }
}

ButtonEvent handleButton(int i) {
  Button &b = btn[i];
  bool down = (digitalRead(b.pin) == LOW);
  uint32_t now = millis();

  if (down != b.down && now - b.lastChange > kDebounceMs) {
    b.lastChange = now;
    b.down = down;
    if (down) {
      b.pressedAt = now;
      b.longDone = false;
    } else if (!b.longDone) {
      return BUTTON_EVENT_SHORT_PRESS;  // released before 3 s = short press
    }
  }

  if (b.down && !b.longDone && now - b.pressedAt >= kHoldMs) {
    b.longDone = true;  // so the release afterwards doesn't transmit
    return BUTTON_EVENT_HOLD;
  }

  return BUTTON_EVENT_NONE;
}
