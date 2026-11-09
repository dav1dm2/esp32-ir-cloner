#include "ir_controller.h"
#include "config.h"
#include "storage.h"
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>

static IRrecv irrecv(kRecvPin, kCaptureBufferSize, kCaptureTimeoutMs, true);
static IRsend irsend(kSendPin);

void blinkLed(uint8_t times, uint16_t ms) {
  for (uint8_t n = 0; n < times; n++) {
    digitalWrite(kLedPin, HIGH);
    delay(ms);
    digitalWrite(kLedPin, LOW);
    delay(ms);
  }
}

void initIR() {
  irsend.begin();
}

void assignSignal(int i) {
  Serial.printf("\n[ASSIGN %s] Point the original remote at the receiver "
                "and press the button now (5 s)...\n", kKeyName[i]);

  decode_results results;
  irrecv.enableIRIn();  // receiver only runs during assign mode
  uint32_t start = millis();
  bool saved = false;

  while (!saved && millis() - start < kAssignMs) {
    digitalWrite(kLedPin, ((millis() - start) / 250) % 2 == 0);

    if (irrecv.decode(&results)) {
      uint16_t n = getCorrectedRawLength(&results);
      if (results.overflow) {
        Serial.println("Capture buffer overflow - increase kCaptureBufferSize");
      } else if (n < kMinRaw) {
        Serial.printf("Ignored short capture (%u timings)\n", n);
      } else if (n > kCaptureBufferSize) {
        Serial.printf("Captured signal too long (%u timings) - increase kCaptureBufferSize\n", n);
      } else {
        uint16_t *raw = resultToRawArray(&results);
        if (raw != nullptr) {
          memcpy(sig[i].data, raw, n * sizeof(uint16_t));
          sig[i].len = n;
          saved = true;
          Serial.printf("Captured %u timings, protocol: %s\n", n,
                        typeToString(results.decode_type).c_str());
        }
        delete[] raw;
      }
      irrecv.resume();
    }
    delay(1);
  }

  irrecv.disableIRIn();
  digitalWrite(kLedPin, LOW);

  if (saved) {
    saveSignal(i);
    Serial.println("Saved.");
    blinkLed(3, 100);
  } else {
    Serial.println("Nothing saved.");
    blinkLed(1, 600);
  }
}

void transmitSignal(int i) {
  if (sig[i].len == 0) {
    Serial.printf("[%s] no signal stored yet\n", kKeyName[i]);
    blinkLed(1, 600);
    return;
  }
  Serial.printf("[TRANSMIT %s] %u timings\n", kKeyName[i], sig[i].len);
  digitalWrite(kLedPin, HIGH);
  irsend.sendRaw(sig[i].data, sig[i].len, kCarrierKHz);
  digitalWrite(kLedPin, LOW);
}
