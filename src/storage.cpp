#include "storage.h"
#include <Preferences.h>

Signal sig[kButtonCount];
const char* kKeyName[kButtonCount] = {"ON", "OFF"};

static Preferences prefs;

void saveSignal(int i) {
  prefs.begin("irclone", false);
  prefs.putUShort((String("len") + i).c_str(), sig[i].len);
  prefs.putBytes((String("dat") + i).c_str(), sig[i].data,
                 sig[i].len * sizeof(uint16_t));
  prefs.end();
}

void loadSignals() {
  prefs.begin("irclone", false);
  for (size_t i = 0; i < kButtonCount; i++) {
    uint16_t len = prefs.getUShort((String("len") + i).c_str(), 0);
    if (len > 0 && len <= kCaptureBufferSize) {
      prefs.getBytes((String("dat") + i).c_str(), sig[i].data,
                     len * sizeof(uint16_t));
      sig[i].len = len;
    }
    Serial.printf("%s: %u timings stored\n", kKeyName[i], sig[i].len);
  }
  prefs.end();
}
