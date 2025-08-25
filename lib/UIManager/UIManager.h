#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <Arduino.h>
#include "TouchInterface.h"
#include "LCD.h"


// Screen states
enum ScreenState {
  SCREEN_MAIN,
  SCREEN_PROFILE_SELECT,
  SCREEN_SETTINGS,
  SCREEN_REFLOW_RUNNING,
  SCREEN_INFO
};

class UIManager {
private:
  Adafruit_ILI9341* display;
  LCD* lcd;  // Add LCD instance for utilities
  ScreenState currentScreen;
  ScreenState previousScreen;
  
public:
  TouchInterface* touchInterface;  // Made public for callbacks
  int lastTouchX, lastTouchY;  // Added for callback access
  
  // Button indices for each screen
  struct ButtonIndices {
    int main_start;
    int main_settings;
    int main_info;
    int profile_select_back;
    int profile_select_buttons[10]; // Up to 10 profiles
    int settings_back;
    int settings_buttons[5]; // Various settings
    int reflow_stop;
    int info_back;
  } buttons;
  
  // Screen drawing functions
  void drawMainScreen();
  void drawProfileSelectScreen();
  void drawSettingsScreen();
  void drawReflowRunningScreen();
  void drawInfoScreen();
  
  // Button callback functions
  static void onStartReflow();
  static void onSettings();
  static void onInfo();
  static void onBack();
  static void onProfileSelect(int profileIndex);
  static void onStopReflow();
  static void onSettingToggle(int settingIndex);
  
  // Helper functions
  void clearScreen();
  void drawHeader(String title);
  void drawTemperatureDisplay();
  void drawStatusBar();
  
public:
  // LCD utility methods
  void setLCDData();
  UIManager(TouchInterface* touch, Adafruit_ILI9341* tft);
  
  // Initialize UI
  void begin();
  
  // Update UI based on current state
  void update();
  
  // Switch to a specific screen
  void switchToScreen(ScreenState screen);
  
  // Get current screen
  ScreenState getCurrentScreen() { return currentScreen; }
  
  // Draw current screen
  void drawCurrentScreen();
  
  // Process touch input
  void processTouch();
  
  // Update temperature display
  void updateTemperature(float temperature);
  
  // Update status
  void updateStatus(String status);
};

// Global instance (will be defined in main.cpp)
extern UIManager* uiManager;

#endif
