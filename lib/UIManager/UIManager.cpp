#include "UIManager.h"
#include "config.h"

// Forward declaration of profile_t structure
typedef struct {
  char title[32];
  char alloy[32];
  uint16_t melting_point;
  uint16_t temp_range_0;
  uint16_t temp_range_1;
  uint16_t time_range_0;
  uint16_t time_range_1;
  char reference[128];
  uint16_t stages_preheat_0;
  uint16_t stages_preheat_1;
  uint16_t stages_soak_0;
  uint16_t stages_soak_1;
  uint16_t stages_reflow_0;
  uint16_t stages_reflow_1;
  uint16_t stages_cool_0;
  uint16_t stages_cool_1;
} profile_t;

// Define NUM_OF_PROFILES if not already defined
#ifndef NUM_OF_PROFILES
#define NUM_OF_PROFILES 10
#endif

// Global variables for callbacks
extern byte state;
extern bool profileIsOn;
extern bool disableMenu;
extern int profileUsed;
extern String activeStatus;
extern float input;
extern profile_t paste_profile[10];
extern bool connected;
extern bool isFault;
extern int profileNum;

// Standalone function for TouchInterface to call
void onProfileSelect(int profileIndex) {
  UIManager::onProfileSelect(profileIndex);
}



UIManager::UIManager(TouchInterface* touch, Adafruit_ILI9341* tft) {
  touchInterface = touch;
  display = tft;
  lcd = new LCD(*tft, touch);  // Create LCD instance with touch support
  currentScreen = SCREEN_MAIN;
  previousScreen = SCREEN_MAIN;
  lastTouchX = 0;
  lastTouchY = 0;
  
  // Initialize button indices
  memset(&buttons, -1, sizeof(buttons));
}

void UIManager::begin() {
  // Set display orientation and background
  display->setRotation(1); // Landscape
  display->fillScreen(ILI9341_BLACK);
  
  // Draw initial screen
  drawCurrentScreen();
}

void UIManager::update() {
  // Check if screen needs to change based on reflow state
  if (profileIsOn && currentScreen != SCREEN_REFLOW_RUNNING) {
    switchToScreen(SCREEN_REFLOW_RUNNING);
  } else if (!profileIsOn && currentScreen == SCREEN_REFLOW_RUNNING) {
    switchToScreen(SCREEN_MAIN);
  }
  
  // Update temperature display if on main or reflow screen
  if (currentScreen == SCREEN_MAIN || currentScreen == SCREEN_REFLOW_RUNNING) {
    updateTemperature(input);
  }
  
  // Process touch input
  processTouch();
}

void UIManager::switchToScreen(ScreenState screen) {
  if (screen == currentScreen) return;
  
  previousScreen = currentScreen;
  currentScreen = screen;
  
  // Clear all buttons
  touchInterface->clearButtons();
  
  // Draw new screen
  drawCurrentScreen();
}

void UIManager::drawCurrentScreen() {
  clearScreen();
  
  switch (currentScreen) {
    case SCREEN_MAIN:
      drawMainScreen();
      break;
    case SCREEN_PROFILE_SELECT:
      drawProfileSelectScreen();
      break;
    case SCREEN_SETTINGS:
      drawSettingsScreen();
      break;
    case SCREEN_REFLOW_RUNNING:
      drawReflowRunningScreen();
      break;
    case SCREEN_INFO:
      drawInfoScreen();
      break;
  }
}

void UIManager::processTouch() {
  touchInterface->processTouch();
}

void UIManager::updateTemperature(float temperature) {
  if (currentScreen == SCREEN_MAIN || currentScreen == SCREEN_REFLOW_RUNNING) {
    drawTemperatureDisplay();
  }
}

void UIManager::setLCDData() {
  // Update LCD with current data
  lcd->setInputInt((int)input);
  lcd->setActiveStatus(activeStatus);
  lcd->setConnected(connected);
  lcd->setFault(isFault);
  lcd->setProfileUsed(profileUsed);
  lcd->setProfileNum(profileNum);
  // Note: LCD expects ReflowProfile* but we have profile_t*
  // This needs to be handled properly or the LCD interface updated
  // For now, skip setting profiles to avoid crash
  // lcd->setProfiles((ReflowProfile*)paste_profile);
}

void UIManager::updateStatus(String status) {
  activeStatus = status;
  lcd->setActiveStatus(status);
  drawStatusBar();
}

void UIManager::clearScreen() {
  display->fillScreen(ILI9341_BLACK);
}

void UIManager::drawHeader(String title) {
  lcd->centeredText(title, ILI9341_WHITE, 10);
  display->drawLine(0, 35, 320, 35, ILI9341_WHITE);
}

void UIManager::drawTemperatureDisplay() {
  // Draw temperature in large font
  display->setTextColor(ILI9341_YELLOW);
  display->setTextSize(4);
  display->setCursor(50, 80);
  display->print(input, 1);
  display->setTextSize(2);
  display->print("C");
  
  // Draw setpoint if reflow is running
  if (profileIsOn) {
    display->setTextColor(ILI9341_GREEN);
    display->setTextSize(2);
    display->setCursor(50, 120);
    display->print("Set: ");
    display->print(paste_profile[profileUsed].stages_reflow_1);
    display->print("C");
  }
}

void UIManager::drawStatusBar() {
  lcd->leftText("Status: " + activeStatus, ILI9341_CYAN, 220);
}

void UIManager::drawMainScreen() {
  drawHeader("Reflow Controller");
  drawTemperatureDisplay();
  drawStatusBar();
  
  // Add main screen buttons
  buttons.main_start = touchInterface->addButton(20, 150, 80, 40, "Start", ILI9341_GREEN, ILI9341_BLACK, onStartReflow);
  buttons.main_settings = touchInterface->addButton(120, 150, 80, 40, "Settings", ILI9341_BLUE, ILI9341_WHITE, onSettings);
  buttons.main_info = touchInterface->addButton(220, 150, 80, 40, "Info", ILI9341_MAGENTA, ILI9341_WHITE, onInfo);
  
  touchInterface->drawButtons();
}

void UIManager::drawProfileSelectScreen() {
  drawHeader("Select Profile");
  
  // Add profile buttons (up to 10)
  int buttonWidth = 140;
  int buttonHeight = 30;
  int x = 20;
  int y = 50;
  int buttonsPerRow = 2;
  
  for (int i = 0; i < NUM_OF_PROFILES; i++) {
    if (i > 0 && i % buttonsPerRow == 0) {
      x = 20;
      y += buttonHeight + 10;
    }
    
    String label = String(i + 1) + ": " + paste_profile[i].title;
    buttons.profile_select_buttons[i] = touchInterface->addButton(x, y, buttonWidth, buttonHeight, label, ILI9341_DARKGREEN, ILI9341_WHITE, nullptr, i);
    x += buttonWidth + 20;
  }
  
  // Add back button
  buttons.profile_select_back = touchInterface->addButton(120, 200, 80, 30, "Back", ILI9341_RED, ILI9341_WHITE, onBack);
  
  touchInterface->drawButtons();
}

void UIManager::drawSettingsScreen() {
  drawHeader("Settings");
  
  // Add settings buttons
  buttons.settings_buttons[0] = touchInterface->addButton(20, 50, 120, 30, "Fan: ON", ILI9341_BLUE, ILI9341_WHITE, nullptr);
  buttons.settings_buttons[1] = touchInterface->addButton(20, 90, 120, 30, "Buzzer: ON", ILI9341_BLUE, ILI9341_WHITE, nullptr);
  buttons.settings_buttons[2] = touchInterface->addButton(20, 130, 120, 30, "OTA: ON", ILI9341_BLUE, ILI9341_WHITE, nullptr);
  
  // Add back button
  buttons.settings_back = touchInterface->addButton(120, 200, 80, 30, "Back", ILI9341_RED, ILI9341_WHITE, onBack);
  
  touchInterface->drawButtons();
}

void UIManager::drawReflowRunningScreen() {
  drawHeader("Reflow Running");
  drawTemperatureDisplay();
  drawStatusBar();
  
  // Add stop button
  buttons.reflow_stop = touchInterface->addButton(120, 150, 80, 40, "STOP", ILI9341_RED, ILI9341_WHITE, onStopReflow);
  
  touchInterface->drawButtons();
}

void UIManager::drawInfoScreen() {
  drawHeader("System Info");
  
  display->setTextColor(ILI9341_WHITE);
  display->setTextSize(1);
  display->setCursor(10, 50);
  display->print("Firmware: v0.3.0.0");
  display->setCursor(10, 70);
  display->print("Profile: ");
  display->print(paste_profile[profileUsed].title);
  display->setCursor(10, 90);
  display->print("Alloy: ");
  display->print(paste_profile[profileUsed].alloy);
  display->setCursor(10, 110);
  display->print("Current Temp: ");
  display->print(input, 1);
  display->print("C");
  
  // Add back button
  buttons.info_back = touchInterface->addButton(120, 200, 80, 30, "Back", ILI9341_RED, ILI9341_WHITE, onBack);
  
  touchInterface->drawButtons();
}

// Static callback functions
void UIManager::onStartReflow() {
  if (uiManager) {
    uiManager->switchToScreen(SCREEN_PROFILE_SELECT);
  }
}

void UIManager::onSettings() {
  if (uiManager) {
    uiManager->switchToScreen(SCREEN_SETTINGS);
  }
}

void UIManager::onInfo() {
  if (uiManager) {
    uiManager->switchToScreen(SCREEN_INFO);
  }
}

void UIManager::onBack() {
  if (uiManager) {
    uiManager->switchToScreen(SCREEN_MAIN);
  }
}

void UIManager::onProfileSelect(int profileIndex) {
  profileUsed = profileIndex;
  profileIsOn = true;
  disableMenu = true;
  if (uiManager) {
    uiManager->switchToScreen(SCREEN_REFLOW_RUNNING);
  }
}

void UIManager::onStopReflow() {
  profileIsOn = false;
  disableMenu = false;
  if (uiManager) {
    uiManager->switchToScreen(SCREEN_MAIN);
  }
}

void UIManager::onSettingToggle(int settingIndex) {
  // Handle setting toggles
  // This would need to be implemented based on your specific settings
}
