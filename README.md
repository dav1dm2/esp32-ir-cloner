# ESP32 IR Cloner

This PlatformIO project learns two commands from an infrared remote and replays them from a pair of buttons. It was built as a prototype for air-conditioner automation. Once taught, it can replay its commands after a power cycle and can run without a computer.

Use this guide to build, flash, and test the cloner. See the [firmware guide](docs/firmware.md) for the capture, storage, replay, and integration details.

## Parts

| Qty | Part | Notes |
| --- | --- | --- |
| 1 | 30-pin ESP32 development board | The project targets the classic ESP32 using PlatformIO's `esp32dev` board profile. Use the GPIO labels printed on your board. |
| 1 | KY-022 IR receiver module | Use a module that supports 3.3 V power and a 3.3 V-compatible signal output. |
| 1 | Generic IR LED | A bare two-lead emitter, with its polarity identified before wiring. |
| 1 | Series resistor | The original resistance is unknown. Start with **330 Ω** for the direct GPIO circuit below. |
| 2 | Momentary, normally open push buttons | One for the `ON` slot and one for the `OFF` slot. |
| 1 | Breadboard and jumper wires | All parts share the ESP32 ground. |
| 1 | USB data cable | Powers the board and connects it to the computer for uploading and viewing serial logs. |
| 1 | Original IR remote and target appliance | Needed to teach the cloner and verify that the replay works. |
| Optional | MB102 breadboard power supply and its power adapter | Powers the build when the ESP32's USB cable is disconnected. |

## Wire the circuit

Disconnect power while wiring. The numbers below are **GPIO numbers**. The order of the physical header pins depends on the board. The firmware pin definitions are in [include/config.h](include/config.h).

| ESP32 connection | Wire to | Function |
| --- | --- | --- |
| `3V3` | KY-022 `VCC` / `+` | Receiver supply. |
| `GND` | KY-022 `GND` / `−`, IR LED cathode, and one terminal of each button | Common ground. |
| `GPIO14` | KY-022 `OUT` / `S` | Receiver signal input. |
| `GPIO4` | Series resistor, then IR LED anode | Active-high IR emitter output. |
| `GPIO25` | Free terminal of the `ON` button | Button for slot 0. |
| `GPIO26` | Free terminal of the `OFF` button | Button for slot 1. |
| `GPIO2` | Active-high status LED, if fitted on the board | The firmware assumes an onboard LED here. If your board lacks one, the serial log still gives feedback. |

```text
ESP32 3V3    ───────────────────── KY-022 VCC (+)
ESP32 GND    ───────────────────── KY-022 GND (−)
ESP32 GPIO14 ◀──────────────────── KY-022 OUT (S)

ESP32 GPIO4  ── [330 Ω] ── IR LED anode → cathode ── GND
ESP32 GPIO25 ── [ON button, normally open] ───────── GND
ESP32 GPIO26 ── [OFF button, normally open] ──────── GND
```

- Check the KY-022's pin markings before plugging it in. The 3.3 V wiring keeps the receiver and ESP32 on the same logic supply. Keep the receiver's signal at ESP32-compatible voltage even if your KY-022 also supports a 5 V supply. [KY-022 pin assignment and ratings](https://sensorkit.joy-it.net/en/sensors/ky-022) · [ESP32 electrical specifications](https://documentation.espressif.com/esp32_datasheet_en.html)
- Check the emitter's polarity. On many through-hole LEDs the longer lead is the anode and the flat edge marks the cathode. For a 3.3 V GPIO and an assumed 1.3 V forward drop, 330 Ω gives about 6 mA while the output is high. Use the actual LED's ratings if known. Start testing close to the appliance since this circuit drives the emitter directly from the GPIO.
- The firmware enables the ESP32's internal pull-ups. A released button reads high and pressing it connects its GPIO to ground. If using four-leg tactile switches, pick terminals that only make contact when the switch is pressed.

### Power options

- **USB**. Plug the ESP32 into the computer for uploading and setup. Feed the KY-022 from the ESP32's `3V3` pin as shown above.
- **Standalone with the MB102**. Disconnect the ESP32's USB cable. Use an adapter rated for your MB102 and set the selected breadboard rail to **5 V**. Verify the rail's voltage and polarity before connecting its positive line to the ESP32 board's documented **5 V power input** and its negative line to ESP32 `GND`. The power input may be labeled `5V` or `VIN`, depending on the board. Keep the KY-022 on the ESP32's `3V3` pin. This is a suggested standalone hookup because the prototype's original MB102 wiring and adapter were unspecified. Use one ESP32 power source at a time and keep 5 V away from `3V3` and the GPIO pins. [MB102 rail selection](https://www.sunfounder.com/products/mb102-breadboard-power-module)

## Build and upload

Open the folder containing [platformio.ini](platformio.ini) in VS Code with PlatformIO installed, or run these commands with PlatformIO Core from that folder. The `esp32dev` environment uses the Arduino framework and declares `IRremoteESP8266` as `crankyoldgit/IRremoteESP8266@^2.9.0`. Upload and serial monitoring are configured for 115200 baud. [PlatformIO ESP32 Dev Module](https://docs.platformio.org/en/stable/boards/espressif32/esp32dev.html) · [IRremoteESP8266](https://github.com/crankyoldgit/IRremoteESP8266/tree/v2.9.0)

```sh
pio run -e esp32dev
pio device list
pio run -e esp32dev -t upload --upload-port /dev/ttyUSB0
pio device monitor --port /dev/ttyUSB0 --baud 115200
```

Replace `/dev/ttyUSB0` in the upload and monitor commands with the port shown for your board, such as `COM3` on Windows. The upload port in `platformio.ini` defaults to `/dev/ttyUSB0`. The command-line flag overrides it. Update the setting for uploads through VS Code, or remove it to let PlatformIO detect the port. [PlatformIO upload options](https://docs.platformio.org/en/stable/userguide/cmd_run.html)

The firmware compiled with PlatformIO Core 6.2.0, Espressif 32 platform 6.12.0, Arduino-ESP32 2.0.17, and IRremoteESP8266 2.9.0. Use the steps below to verify capture and replay on your hardware.

## Teach and test

1. Open the serial monitor. After reset, the firmware reports the stored timing count for each slot, then prints `Ready.` as part of its startup message. A count of zero means the slot is empty.
2. Hold the cloner's `ON` button for about **3 seconds**. When the status LED starts blinking slowly or the serial monitor shows `[ASSIGN ON]`, point the original remote at the KY-022 and press the desired command once within **5 seconds**.
3. Look for three quick blinks and a captured timing count followed by `Saved.`. If you get one long blink and `Nothing saved.`, try again. Teach the `OFF` button the same way. Each label is a slot name, so the actual behavior depends on the remote command you teach it.
4. Aim the IR emitter at the appliance from close range. Briefly press and release each cloner button and check the appliance's response. The status LED turns on during transmission and the serial monitor reports `[TRANSMIT ON]` or `[TRANSMIT OFF]`.
5. Power-cycle the ESP32 and repeat the replay test to confirm both commands were restored from flash. To replace one command, hold the matching cloner button again and teach it a new command. If the learning window expires without an accepted capture, the previous assignment stays in place.

| Symptom | What to check |
| --- | --- |
| The board fails to upload | Use a USB data cable, confirm the serial port, and close any serial monitor using it. |
| `Nothing saved.` | Verify receiver power, ground, and `OUT` wiring. Wait for the assignment prompt before pressing the remote. The serial log also reports short captures and overflows. |
| `[ON] no signal stored yet` or `[OFF] no signal stored yet` | Teach the indicated slot before trying to replay it. |
| Replay logs a transmission but the appliance does not respond | Check IR LED polarity and GPIO4 wiring. Move the emitter closer and aim it at the appliance's IR window. Verify the original remote works, then try teaching a fresh press. Some remotes require a different carrier or a longer sequence than this firmware captures. |
| Both buttons toggle power | Check the original remote. If it sends a toggle command, teaching it to both slots reproduces the toggle in both slots. |
