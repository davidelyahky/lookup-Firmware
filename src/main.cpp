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

  // Full-screen refresh: white background, black border, a friendly greeting
  // on the left, and some fun graphics on the right.
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);

    // Decorative double border (black outer, red inner).
    display.drawRect(2, 2, display.width() - 4, display.height() - 4, GxEPD_BLACK);
    display.drawRect(5, 5, display.width() - 10, display.height() - 10, GxEPD_RED);

    // ---- Greeting text (left side) ----
    display.setTextSize(2);                 // 2x the 9pt font

    display.setTextColor(GxEPD_BLACK);
    display.setCursor(20, 45);
    display.print("Hello");

    display.setTextColor(GxEPD_RED);
    display.setCursor(20, 80);
    display.print("Wael!");

    // little underline flourish under the name
    display.drawLine(20, 90, 110, 90, GxEPD_BLACK);

    display.setTextSize(1);

    // ---- Fun graphics (right side) ----
    // A cheerful smiley "sun" in the upper-right.
    const int sunX = 235, sunY = 45, sunR = 22;
    display.fillCircle(sunX, sunY, sunR, GxEPD_RED);   // sun face
    // sun rays
    for (int a = 0; a < 360; a += 45) {
      float rad = a * 3.14159265f / 180.0f;
      int x1 = sunX + (int)((sunR + 3) * cos(rad));
      int y1 = sunY + (int)((sunR + 3) * sin(rad));
      int x2 = sunX + (int)((sunR + 9) * cos(rad));
      int y2 = sunY + (int)((sunR + 9) * sin(rad));
      display.drawLine(x1, y1, x2, y2, GxEPD_RED);
    }
    // smiley face on the sun (black)
    display.fillCircle(sunX - 8, sunY - 5, 3, GxEPD_BLACK);  // left eye
    display.fillCircle(sunX + 8, sunY - 5, 3, GxEPD_BLACK);  // right eye
    // smile: a short arc made of dots
    for (int dx = -9; dx <= 9; dx++) {
      int dy = (int)(0.08f * dx * dx);   // gentle upward smile curve
      display.drawPixel(sunX + dx, sunY + 8 - dy, GxEPD_BLACK);
    }

    // A couple of hearts in the lower-right.
    auto drawHeart = [&](int cx, int cy, int s, uint16_t color) {
      display.fillCircle(cx - s / 2, cy, s / 2, color);
      display.fillCircle(cx + s / 2, cy, s / 2, color);
      display.fillTriangle(cx - s, cy + 1, cx + s, cy + 1,
                           cx, cy + s + 2, color);
    };
    drawHeart(225, 95, 8, GxEPD_RED);
    drawHeart(255, 105, 6, GxEPD_BLACK);
    drawHeart(278, 92, 7, GxEPD_RED);
  } while (display.nextPage());

  Serial.println("Done - display should now greet Wael.");
  display.hibernate();  // low power; image persists with no power
}

void loop() {
  // E-paper holds its image; nothing to do.
}
