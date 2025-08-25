#ifndef TOUCH_INTERFACE_H
#define TOUCH_INTERFACE_H

#include <Arduino.h>
#include <config.h>
#include <XPT2046_Touchscreen.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>

/*// Pin definitions for TouchInterface
#define XPT2046_IRQ 4
#define display_MOSI 23
#define display_MISO 19
#define display_CLK 18
#define display_CS 5
*/
// Forward declaration
void onProfileSelect(int profileIndex);

// Touch button structure
struct TouchButton {
  int x, y, width, height;
  String label;
  uint16_t color;
  uint16_t textColor;
  bool pressed;
  bool enabled;
  void (*callback)();
  int callbackData; // Additional data for callback
};

class TouchInterface {
private:
  XPT2046_Touchscreen* ts;
  Adafruit_ILI9341* display;
  TouchButton* buttons;
  int buttonCount;
  int maxButtons;
  
  // Touch calibration values
  int touchCalibrationX1, touchCalibrationY1, touchCalibrationX2, touchCalibrationY2;
  int displayCalibrationX1, displayCalibrationY1, displayCalibrationX2, displayCalibrationY2;
  
  // Touch state
  bool lastTouchState;
  int lastTouchX, lastTouchY;
  
  // Convert touch coordinates to display coordinates
  void convertTouchToDisplay(int touchX, int touchY, int& displayX, int& displayY);
  
public:
  TouchInterface(XPT2046_Touchscreen* touchscreen, Adafruit_ILI9341* tftDisplay, int maxButtonCount = 20);
  ~TouchInterface();
  
  // Initialize touch interface
  void begin();
  
  // Add a button to the interface
  int addButton(int x, int y, int width, int height, String label, uint16_t color, uint16_t textColor, void (*callback)() = nullptr, int callbackData = 0);
  
  // Remove a button
  void removeButton(int index);
  
  // Update button properties
  void updateButton(int index, String label, uint16_t color, uint16_t textColor);
  void setButtonEnabled(int index, bool enabled);
  
  // Draw all buttons
  void drawButtons();
  
  // Draw a specific button
  void drawButton(int index);
  
  // Process touch input
  void processTouch();
  
  // Check if a point is within a button
  int getButtonAt(int x, int y);
  
  // Set touch calibration
  void setCalibration(int touchX1, int touchY1, int touchX2, int touchY2, 
                     int displayX1, int displayY1, int displayX2, int displayY2);
  
  // Get touch coordinates
  bool getTouchPoint(int& x, int& y);
  
  // Clear all buttons
  void clearButtons();
  
  // Get button count
  int getButtonCount() { return buttonCount; }
  
  // Get button data
  int getButtonData(int index);
};

#endif
