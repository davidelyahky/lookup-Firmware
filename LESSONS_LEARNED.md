# Lessons Learned — WeAct 2.9" 3-color e-paper on ESP32-S3

A reusable playbook from getting a WeAct Studio 2.9" (black/white/red) SPI
e-paper display working on an ESP32-S3-DevKitM-1 with PlatformIO + GxEPD2.

---

## 1. Match the framework to your library
The project started as **ESP-IDF (C)**, but **GxEPD2** (the easy e-paper
library) is **Arduino-only**. Switched `platformio.ini` to
`framework = arduino` and used a `.cpp` file.

> **Takeaway:** Check what framework your library needs *before* writing code.
> GxEPD2 = Arduino.

## 2. "GPIO pins in order" still has to dodge reserved pins
On the ESP32-S3 you can't literally use GPIO 1, 2, 3… Avoid:
- **0 / 3 / 45 / 46** — strapping / boot pins
- **19 / 20** — native USB (D− / D+)
- **43 / 44** — default UART0 TX/RX
- **26–32** — wired to the SPI flash

Used **4, 5, 6, 7** for control + **12 (SCK) / 11 (MOSI)** for hardware SPI.

> **Takeaway:** Pick a clean ascending run of *safe* pins, not pin 1.

## 3. Editor errors ≠ compiler errors
Red squiggles on `#include <Arduino.h>` etc. were stale **IntelliSense**, even
though `pio run` compiled fine. Fix:
```
pio project init --ide vscode
```
This regenerates `.vscode/c_cpp_properties.json` with correct include paths.

> **Takeaway:** If it builds but the editor complains, regenerate the IDE config
> (or run "C/C++: Reset IntelliSense Database").

## 4. Don't let CMake Tools hijack the build
The VS Code **CMake Tools** extension wandered into a library folder under
`.pio/` and failed with `Unknown CMake command "idf_component_register"`.
Fix: build with **PlatformIO**, and disable CMake auto-configure in
`.vscode/settings.json`:
```json
{
  "cmake.configureOnOpen": false,
  "cmake.configureOnEdit": false,
  "cmake.automaticReconfigure": false,
  "C_Cpp.default.configurationProvider": "platformio.platformio-ide"
}
```

> **Takeaway:** Use PlatformIO's ✓ (Build) / → (Upload) buttons or the CLI —
> not the generic CMake "Build" button.

## 5. Boot loop on first boot = flash size / partition mismatch (the big one)
Serial spammed:
```
E spi_flash: Detected size(4096k) smaller than the size in the binary image header(8192k). Probe failed.
assert failed: do_core_init startup.c:328 (flash_ret == ESP_OK)
Rebooting...
```
The board has **4 MB** flash; the board definition assumed **8 MB**, so the
image header was wrong and it rebooted forever — `setup()` never ran. Fix in
`platformio.ini`:
```ini
board_upload.flash_size = 4MB
board_build.flash_size = 4MB
board_build.partitions = default.csv
```
Then erase + re-upload:
```
pio run -t erase
pio run -t upload
```

> **Takeaway:** A reboot loop right at boot is almost always flash size /
> partition mismatch. **Read the serial log — it names the problem.**

## 6. A fully red (or black) screen = wrong panel type
A black/white driver on a **3-color (black/white/red)** panel floods the screen
red. The fix was to use the 3-color driver:
- `#include <GxEPD2_3C.h>` (not `GxEPD2_BW.h`)
- template `GxEPD2_3C<...>`
- driver class `GxEPD2_290_C90c` (fallback: `GxEPD2_290_Z13c`)
- draw with `GxEPD_RED`

A 3-color refresh is **slow (~15 s) and flickers** black↔red↔white — normal.

> **Takeaway:** Red/black flooding tells you the variant. A B/W panel physically
> can't show red, so color output = you have the 3-color module.

---

## Quick command reference (PlatformIO CLI)
```
pio run                 # build
pio run -t upload       # build + flash
pio run -t erase        # wipe flash (fixes corrupt partitions / boot loops)
pio device monitor      # serial @ 115200
```
> **Gotcha:** Only one program can hold the COM port. **Close the serial monitor
> before uploading or erasing**, or you'll get "port is busy / Access is denied".

## Final working config
- **Board:** ESP32-S3-DevKitM-1 (4 MB flash, 2 MB PSRAM)
- **Panel:** WeAct 2.9" 3-color, 296×128, driver `GxEPD2_290_C90c`
- **Pins:** BUSY=4, RST=5, DC=6, CS=7, SCK=12, MOSI=11, VCC=3V3, GND=GND
- **Port:** native USB-C ("USB"), with `ARDUINO_USB_MODE=1` + `ARDUINO_USB_CDC_ON_BOOT=1`
