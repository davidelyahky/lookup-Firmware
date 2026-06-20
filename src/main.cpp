// WeAct Studio 2.9" 3-COLOR (black/white/red) SPI e-paper test
// for ESP32-S3-DevKitM-1
//
// Panel:      WeAct Studio 2.9" b/w/RED, 296x128, SSD1680-class controller
// Framework:  Arduino (PlatformIO)
// Library:    GxEPD2 by Jean-Marc Zingg
//
// Wiring (see WIRING.md for the full table / diagram):
//   Display   ESP32-S3 GPIO
//   -------   -------------
//   BUSY  ->  4
//   RST   ->  5
//   DC    ->  6
//   CS    ->  7
//   SCL   ->  12   (SPI clock / SCK)
//   SDA   ->  11   (SPI data  / MOSI)
//   GND   ->  GND
//   VCC   ->  3V3  (3.3 V ONLY)

#include <Arduino.h>
#include <GxEPD2_3C.h>   // 3-color (black/white/red) variant
#include <SPI.h>
#include <Fonts/FreeMonoBold9pt7b.h>

// ---- Control pins ----
#define EPD_BUSY 4
#define EPD_RST  5
#define EPD_DC   6
#define EPD_CS   7

// ---- Hardware SPI pins (FSPI) ----
#define EPD_SCK  12  // SCL
#define EPD_MOSI 11  // SDA
#define EPD_MISO 13  // unused by e-paper, but SPI.begin() wants a pin

// WeAct Studio 2.9" 3-color 296x128.
// GxEPD2_290_C90c matches the SSD1680-based WeAct 3-color panel.
// If colors look wrong or the panel stays blank, try GxEPD2_290_Z13c
// (change BOTH occurrences of the class name below).
GxEPD2_3C<GxEPD2_290_C90c, GxEPD2_290_C90c::HEIGHT>
    display(GxEPD2_290_C90c(/*CS=*/EPD_CS, /*DC=*/EPD_DC,
                            /*RST=*/EPD_RST, /*BUSY=*/EPD_BUSY));

void setup() {
  Serial.begin(115200);
  // Native USB CDC: wait (up to ~2s) for the host serial monitor to attach
  // so the first prints aren't lost.
  unsigned long t0 = millis();
  while (!Serial && millis() - t0 < 2000) {
    delay(10);
  }
  delay(200);
  Serial.println("\nWeAct 2.9\" 3-color e-paper test starting...");

  // Bind hardware SPI to our chosen pins, then init the panel.
  display.init(115200, true, 2, false);
  SPI.end();
  SPI.begin(EPD_SCK, EPD_MISO, EPD_MOSI, EPD_CS);

  display.setRotation(1);   // 1 = landscape (296 wide)
  display.setFont(&FreeMonoBold9pt7b);

  // Full-screen refresh: white background, black border, black + red text.
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.drawRect(2, 2, display.width() - 4, display.height() - 4, GxEPD_BLACK);

    display.setTextColor(GxEPD_BLACK);
    display.setCursor(12, 30);
    display.print("Hello, e-ink!");

    display.setTextColor(GxEPD_RED);
    display.setCursor(12, 60);
    display.print("WeAct 2.9 3-color");

    display.setTextColor(GxEPD_BLACK);
    display.setCursor(12, 90);
    display.print("ESP32-S3 SPI OK");

    // a small red filled box to confirm the red plane works
    display.fillRect(220, 70, 60, 40, GxEPD_RED);
  } while (display.nextPage());

  Serial.println("Done - display should now show black + red text.");
  display.hibernate();  // low power; image persists with no power
}

void loop() {
  // E-paper holds its image; nothing to do.
}
