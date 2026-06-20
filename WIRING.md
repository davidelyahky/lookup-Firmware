# WeAct Studio 2.9" e-paper — wiring & flashing (ESP32-S3-DevKitM-1)

Panel: WeAct Studio 2.9" black/white, **296×128**, **SSD1680** controller, 8-pin SPI.

## A note on "GPIO pins in order"

You can't literally use GPIO 1, 2, 3… on the ESP32-S3 — several pins are reserved:

- **GPIO 0 / 3 / 45 / 46** — strapping/boot pins (flaky if held by a device)
- **GPIO 19 / 20** — native USB (D− / D+)
- **GPIO 43 / 44** — default UART0 TX/RX
- **GPIO 26–32** — connected to the in-package SPI flash; unusable

So the control pins use a clean ascending run of **safe** GPIOs (4, 5, 6, 7), plus
the hardware-SPI pins **12 (SCK)** and **11 (MOSI)** which stay fixed for reliability.

## Wiring

```
  WeAct 2.9" E-Paper            ESP32-S3-DevKitM-1
 ┌──────────────────┐
 │  BUSY  ───────────────────►  GPIO 4
 │  RST   ───────────────────►  GPIO 5
 │  DC    ───────────────────►  GPIO 6
 │  CS    ───────────────────►  GPIO 7
 │  SCL   ───────────────────►  GPIO 12  (SPI CLK / SCK)
 │  SDA   ───────────────────►  GPIO 11  (SPI MOSI / DIN)
 │  GND   ───────────────────►  GND
 │  VCC   ───────────────────►  3V3   ◄── 3.3 V ONLY, never 5 V
 └──────────────────┘
```

| Display pin | ESP32-S3 GPIO | Role |
|-------------|---------------|------|
| BUSY | 4  | busy/ready status |
| RST  | 5  | hardware reset |
| DC   | 6  | data/command select |
| CS   | 7  | SPI chip select |
| SCL  | 12 | SPI clock (SCK) |
| SDA  | 11 | SPI data in (MOSI) |
| GND  | GND | ground |
| VCC  | 3V3 | power — **3.3 V** |

> Some WeAct boards silkscreen `CLK`=SCL and `DIN`=SDA — same signals.
> Power from **3V3**, never 5V/VIN — these panels are 3.3 V parts.

## Flashing

1. Open this folder in VS Code with the **PlatformIO** extension installed.
2. Connect the board. Use the port labeled **UART** for the simplest experience
   (default serial works without extra build flags).
3. Click **Upload** (→) in the PlatformIO toolbar, or run:
   ```
   pio run -t upload
   ```
4. Open the serial monitor (plug icon) or run `pio device monitor` to see the
   debug prints at 115200 baud.

### If using the native USB port
Uncomment the `build_flags` block in `platformio.ini`
(`ARDUINO_USB_MODE` / `ARDUINO_USB_CDC_ON_BOOT`) so Serial output appears.

### If upload fails to connect
Hold **BOOT**, tap **RESET**, release **BOOT**, then upload again to force
download mode. Make sure you're using a **data** USB-C cable, not charge-only.

## Panel variant: this is the 3-COLOR (black/white/red) module
A fully red (or fully black) screen on a B/W driver = you actually have the
**3-color** panel. The code uses `#include <GxEPD2_3C.h>` and the 3-color
driver class. A 3-color refresh is slow (~15 s) and flickers — that's normal.

If the screen stays blank, garbled, or colors are wrong, change **both**
occurrences of the driver class in `src/main.cpp`:

- 3-color (this panel): `GxEPD2_290_C90c`  ← current, or `GxEPD2_290_Z13c`
- B/W version instead:   switch include back to `GxEPD2_BW.h` and use
  `GxEPD2_290_BS` (or `GxEPD2_290_T94_V2`)
