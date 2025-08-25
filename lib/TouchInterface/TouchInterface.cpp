#include "TouchInterface.h"
#include <config.h>

TouchInterface::TouchInterface(XPT2046_Touchscreen* touchscreen, Adafruit_ILI9341* tftDisplay, int maxButtonCount) {
  ts = touchscreen;
  display = tftDisplay;
  maxButtons = maxButtonCount;
  buttonCount = 0;
  buttons = new TouchButton[maxButtons];
  
  // Initialize calibration with default values
  touchCalibrationX1 = 0;
  touchCalibrationY1 = 0;
  touchCalibrationX2 = 4095;
  touchCalibrationY2 = 4095;
  displayCalibrationX1 = 0;
  displayCalibrationY1 = 0;
  displayCalibrationX2 = 320;
  displayCalibrationY2 = 240;
  
  lastTouchState = false;
  lastTouchX = 0;
  lastTouchY = 0;
}

TouchInterface::~TouchInterface() {
  if (buttons) {
    delete[] buttons;
  }
}

void TouchInterface::begin() {
  // Initialize touchscreen
  ts->begin();
  ts->setRotation(1); // Landscape orientation
}

int TouchInterface::addButton(int x, int y, int width, int height, String label, uint16_t color, uint16_t textColor, void (*callback)(), int callbackData) {
  if (buttonCount >= maxButtons) {
    return -1; // No space left
  }
  
  buttons[buttonCount].x = x;
  buttons[buttonCount].y = y;
  buttons[buttonCount].width = width;
  buttons[buttonCount].height = height;
  buttons[buttonCount].label = label;
  buttons[buttonCount].color = color;
  buttons[buttonCount].textColor = textColor;
  buttons[buttonCount].pressed = false;
  buttons[buttonCount].enabled = true;
  buttons[buttonCount].callback = callback;
  buttons[buttonCount].callbackData = callbackData;
  
  return buttonCount++;
}

void TouchInterface::removeButton(int index) {
  if (index < 0 || index >= buttonCount) {
    return;
  }
  
  // Shift remaining buttons
  for (int i = index; i < buttonCount - 1; i++) {
    buttons[i] = buttons[i + 1];
  }
  buttonCount--;
}

void TouchInterface::updateButton(int index, String label, uint16_t color, uint16_t textColor) {
  if (index < 0 || index >= buttonCount) {
    return;
  }
  
  buttons[index].label = label;
  buttons[index].color = color;
  buttons[index].textColor = textColor;
  drawButton(index);
}

void TouchInterface::setButtonEnabled(int index, bool enabled) {
  if (index < 0 || index >= buttonCount) {
    return;
  }
  
  buttons[index].enabled = enabled;
  drawButton(index);
}

void TouchInterface::drawButtons() {
  for (int i = 0; i < buttonCount; i++) {
    drawButton(i);
  }
}

void TouchInterface::drawButton(int index) {
  if (index < 0 || index >= buttonCount) {
    return;
  }
  
  TouchButton& btn = buttons[index];
  
  // Draw button background
  uint16_t bgColor = btn.enabled ? btn.color : 0x8410; // Gray if disabled
  display->fillRoundRect(btn.x, btn.y, btn.width, btn.height, 5, bgColor);
  display->drawRoundRect(btn.x, btn.y, btn.width, btn.height, 5, 0x0000);
  
  // Draw button text
  if (btn.enabled) {
    display->setTextColor(btn.textColor);
  } else {
    display->setTextColor(0x8410); // Gray text if disabled
  }
  display->setTextSize(2);
  
  // Center text in button
  int16_t textX, textY;
  uint16_t textWidth, textHeight;
  display->getTextBounds(btn.label, 0, 0, &textX, &textY, &textWidth, &textHeight);
  
  int centerX = btn.x + (btn.width - textWidth) / 2;
  int centerY = btn.y + (btn.height + textHeight) / 2 - 2;
  
  display->setCursor(centerX, centerY);
  display->print(btn.label);
}

void TouchInterface::processTouch() {
  TS_Point p = ts->getPoint();
  
  // Restore pin modes (touchscreen library changes them)
  pinMode(XPT2046_IRQ, INPUT);
  pinMode(display_MOSI, OUTPUT);
  pinMode(display_MISO, INPUT);
  pinMode(display_CLK, OUTPUT);
  pinMode(display_CS, OUTPUT);
  
  if (p.z > 10) { // Touch detected
    int displayX, displayY;
    convertTouchToDisplay(p.x, p.y, displayX, displayY);
    
    if (!lastTouchState) { // New touch
      int buttonIndex = getButtonAt(displayX, displayY);
      if (buttonIndex >= 0 && buttons[buttonIndex].enabled) {
        buttons[buttonIndex].pressed = true;
        drawButton(buttonIndex); // Visual feedback
        
        // Execute callback if available
        if (buttons[buttonIndex].callback) {
          buttons[buttonIndex].callback();
        }
        
        // Handle profile selection if this is a profile button
        if (buttons[buttonIndex].callbackData >= 0) {
          // This is a profile selection button
          extern void onProfileSelect(int profileIndex);
          onProfileSelect(buttons[buttonIndex].callbackData);
        }
      }
    }
    
    lastTouchState = true;
    lastTouchX = displayX;
    lastTouchY = displayY;
  } else {
    // Release all buttons
    for (int i = 0; i < buttonCount; i++) {
      if (buttons[i].pressed) {
        buttons[i].pressed = false;
        drawButton(i); // Redraw to show unpressed state
      }
    }
    lastTouchState = false;
  }
}

int TouchInterface::getButtonAt(int x, int y) {
  for (int i = 0; i < buttonCount; i++) {
    if (x >= buttons[i].x && x <= buttons[i].x + buttons[i].width &&
        y >= buttons[i].y && y <= buttons[i].y + buttons[i].height) {
      return i;
    }
  }
  return -1; // No button found
}

void TouchInterface::convertTouchToDisplay(int touchX, int touchY, int& displayX, int& displayY) {
  // Simple linear mapping
  displayX = map(touchX, touchCalibrationX1, touchCalibrationX2, displayCalibrationX1, displayCalibrationX2);
  displayY = map(touchY, touchCalibrationY1, touchCalibrationY2, displayCalibrationY1, displayCalibrationY2);
  
  // Clamp values
  displayX = constrain(displayX, 0, 319);
  displayY = constrain(displayY, 0, 239);
}

void TouchInterface::setCalibration(int touchX1, int touchY1, int touchX2, int touchY2, 
                                   int displayX1, int displayY1, int displayX2, int displayY2) {
  touchCalibrationX1 = touchX1;
  touchCalibrationY1 = touchY1;
  touchCalibrationX2 = touchX2;
  touchCalibrationY2 = touchY2;
  displayCalibrationX1 = displayX1;
  displayCalibrationY1 = displayY1;
  displayCalibrationX2 = displayX2;
  displayCalibrationY2 = displayY2;
}

bool TouchInterface::getTouchPoint(int& x, int& y) {
  TS_Point p = ts->getPoint();
  
  // Restore pin modes
  pinMode(XPT2046_IRQ, INPUT);
  pinMode(display_MOSI, OUTPUT);
  pinMode(display_MISO, INPUT);
  pinMode(display_CLK, OUTPUT);
  pinMode(display_CS, OUTPUT);
  
  if (p.z > 10) {
    convertTouchToDisplay(p.x, p.y, x, y);
    return true;
  }
  return false;
}

void TouchInterface::clearButtons() {
  buttonCount = 0;
}

int TouchInterface::getButtonData(int index) {
  if (index >= 0 && index < buttonCount) {
    return buttons[index].callbackData;
  }
  return -1;
}
