/*
  Basic Touch Interface Example
  
  This example demonstrates the basic usage of the TouchInterface library
  with an ILI9341 TFT display and XPT2046 touchscreen.
  
  Hardware:
  - ESP32 Development Board
  - ILI9341 TFT Display (2.4" 320x240)
  - XPT2046 Touchscreen Controller
  
  Pin Connections:
  - Display CS: GPIO 5
  - Display DC: GPIO 21
  - Display MOSI: GPIO 23
  - Display SCK: GPIO 18
  - Display RST: -1 (not connected)
  - Touch CS: GPIO 16
  - Touch IRQ: GPIO 4
*/

#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
#include "TouchInterface.h"

// Pin definitions
#define display_cs   5
#define display_dc   21
#define display_mosi 23
#define display_sclk 18
#define display_rst  -1
#define XPT2046_CS  16
#define XPT2046_IRQ 4

// Create instances
Adafruit_ILI9341 display = Adafruit_ILI9341(display_cs, display_dc, display_rst);
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);
TouchInterface touchInterface(&ts, &display);

// Button callback functions
void onStartButton() {
  Serial.println("Start button pressed!");
  display.fillScreen(ILI9341_GREEN);
  display.setCursor(50, 100);
  display.setTextColor(ILI9341_BLACK);
  display.setTextSize(2);
  display.println("STARTED!");
  delay(1000);
  drawMainScreen();
}

void onSettingsButton() {
  Serial.println("Settings button pressed!");
  display.fillScreen(ILI9341_BLUE);
  display.setCursor(50, 100);
  display.setTextColor(ILI9341_WHITE);
  display.setTextSize(2);
  display.println("SETTINGS!");
  delay(1000);
  drawMainScreen();
}

void onInfoButton() {
  Serial.println("Info button pressed!");
  display.fillScreen(ILI9341_MAGENTA);
  display.setCursor(50, 100);
  display.setTextColor(ILI9341_WHITE);
  display.setTextSize(2);
  display.println("INFO!");
  delay(1000);
  drawMainScreen();
}

void drawMainScreen() {
  display.fillScreen(ILI9341_BLACK);
  
  // Draw title
  display.setTextColor(ILI9341_WHITE);
  display.setTextSize(2);
  display.setCursor(50, 20);
  display.println("Touch Demo");
  
  // Draw temperature display
  display.setTextColor(ILI9341_YELLOW);
  display.setTextSize(3);
  display.setCursor(80, 60);
  display.println("25.5C");
  
  // Add touch buttons
  touchInterface.clearButtons();
  touchInterface.addButton(20, 120, 80, 40, "Start", ILI9341_GREEN, ILI9341_BLACK, onStartButton);
  touchInterface.addButton(120, 120, 80, 40, "Settings", ILI9341_BLUE, ILI9341_WHITE, onSettingsButton);
  touchInterface.addButton(220, 120, 80, 40, "Info", ILI9341_MAGENTA, ILI9341_WHITE, onInfoButton);
  
  touchInterface.drawButtons();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Touch Interface Demo Starting...");
  
  // Initialize display
  display.begin();
  display.setRotation(1); // Landscape
  display.fillScreen(ILI9341_BLACK);
  
  // Initialize touch interface
  touchInterface.begin();
  
  // Draw initial screen
  drawMainScreen();
  
  Serial.println("Touch Interface Demo Ready!");
}

void loop() {
  // Process touch input
  touchInterface.processTouch();
  
  // Add any other loop logic here
  delay(10);
}
