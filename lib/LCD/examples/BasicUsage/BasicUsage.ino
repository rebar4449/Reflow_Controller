 /*
  Basic Usage Example for LCD Library
  
  This example demonstrates the basic usage of the LCD library
  for an ESP32 reflow controller.
  
  Hardware Requirements:
  - ESP32 development board
  - TFT display (ILI9341 or compatible)
  - Optional: Buzzer, fan, SSR, LED for testing
  
  Created by: Your Name
  Date: 2024
*/

#include <TFT_eSPI.h>
#include "LCD.h"

// Initialize TFT display
TFT_eSPI tft = TFT_eSPI();

// Initialize LCD library
LCD lcd(tft);

// Button pins (define these according to your hardware)
#define BUTTON_UP 32
#define BUTTON_DOWN 33
#define BUTTON_SELECT 34
#define BUTTON_BACK 35

// Example reflow profiles
ReflowProfile profiles[] = {
    {"Lead-Free", "SAC305"},
    {"Lead", "Sn63Pb37"},
    {"Custom", "Custom Alloy"}
};

void setup() {
    Serial.begin(115200);
    Serial.println("LCD Library Basic Usage Example");
    
    // Initialize button pins
    pinMode(BUTTON_UP, INPUT_PULLUP);
    pinMode(BUTTON_DOWN, INPUT_PULLUP);
    pinMode(BUTTON_SELECT, INPUT_PULLUP);
    pinMode(BUTTON_BACK, INPUT_PULLUP);
    
    // Initialize TFT display
    tft.init();
    tft.setRotation(1);  // Landscape orientation
    tft.fillScreen(TFT_BLACK);
    
    // Configure LCD library
    lcd.setHorizontal(true);      // Set horizontal orientation
    lcd.setBuzzerEnabled(true);   // Enable buzzer
    lcd.setButtonsEnabled(true);  // Enable buttons
    lcd.setFanEnabled(true);      // Enable fan
    lcd.setOTAEnabled(true);      // Enable OTA updates
    lcd.setSPIFFSEnabled(true);   // Enable SPIFFS
    
    // Set initial data
    lcd.setActiveStatus("Ready");
    lcd.setFwVersion("1.0.0");
    lcd.setInputInt(25);          // Initial temperature
    lcd.setConnected(true);       // WiFi connected
    lcd.setFault(false);          // No faults
    
    // Set profiles
    lcd.setProfiles(profiles);
    lcd.setProfileNum(3);
    lcd.setProfileUsed(0);
    
    // Show start screen
    lcd.startScreen();
    
    Serial.println("Setup complete");
}

void loop() {
    // Simulate temperature reading (replace with actual sensor)
    static int temp = 25;
    temp += random(-2, 3);  // Random temperature variation
    if (temp < 20) temp = 20;
    if (temp > 100) temp = 100;
    
    lcd.setInputInt(temp);
    
    // Handle button inputs
    handleButtons();
    
    // Process menu
    lcd.processMenu();
    
    // Update display based on current state
    updateDisplay();
    
    delay(100);
}

void handleButtons() {
    static unsigned long lastButtonPress = 0;
    unsigned long currentTime = millis();
    
    // Debounce buttons
    if (currentTime - lastButtonPress < 200) {
        return;
    }
    
    // Handle button presses
    if (digitalRead(BUTTON_UP) == LOW) {
        lastButtonPress = currentTime;
        int pointer = lcd.getSettingsPointer();
        int numPointers = lcd.getNumOfPointers();
        if (pointer > 0) {
            lcd.setSettingsPointer(pointer - 1);
        } else {
            lcd.setSettingsPointer(numPointers - 1);
        }
        Serial.println("Button UP pressed");
    }
    
    if (digitalRead(BUTTON_DOWN) == LOW) {
        lastButtonPress = currentTime;
        int pointer = lcd.getSettingsPointer();
        int numPointers = lcd.getNumOfPointers();
        if (pointer < numPointers - 1) {
            lcd.setSettingsPointer(pointer + 1);
        } else {
            lcd.setSettingsPointer(0);
        }
        Serial.println("Button DOWN pressed");
    }
    
    if (digitalRead(BUTTON_SELECT) == LOW) {
        lastButtonPress = currentTime;
        handleSelection();
        Serial.println("Button SELECT pressed");
    }
    
    if (digitalRead(BUTTON_BACK) == LOW) {
        lastButtonPress = currentTime;
        handleBack();
        Serial.println("Button BACK pressed");
    }
}

void handleSelection() {
    int state = lcd.getState();
    int pointer = lcd.getSettingsPointer();
    
    switch (state) {
        case STATE_HOME:
            // Go to main menu from home screen
            lcd.mainMenuScreen();
            break;
            
        case STATE_MAIN_MENU:
            // Navigate based on selected menu item
            switch (pointer) {
                case 0:  // Select profile
                    lcd.showSelectProfile();
                    break;
                case 1:  // Change profile
                    lcd.showChangeProfile();
                    break;
                case 2:  // Add profile
                    lcd.showAddProfile();
                    break;
                case 3:  // Settings
                    lcd.showSettings();
                    break;
                case 4:  // Info
                    lcd.showInfo();
                    break;
                case 5:  // Update firmware (if available)
                    if (lcd.getUpdateAvailable()) {
                        lcd.startUpdateScreen();
                    }
                    break;
            }
            break;
            
        case STATE_SELECT_PROFILE:
            // Select the highlighted profile
            lcd.setProfileUsed(pointer);
            lcd.loopScreen();  // Return to home screen
            break;
            
        case STATE_SETTINGS:
            // Handle settings selection
            handleSettingsSelection(pointer);
            break;
            
        case STATE_TEST_OUTPUTS:
            // Toggle test state for selected output
            lcd.setTestState(!lcd.getTestState());
            lcd.testOutputs();  // Refresh the test screen
            break;
            
        case STATE_START_REFLOW:
            // Start reflow process
            lcd.setActiveStatus("Reflowing");
            lcd.loopScreen();
            break;
            
        case STATE_STOP_REFLOW:
            // Stop reflow process
            lcd.setActiveStatus("Stopped");
            lcd.loopScreen();
            break;
    }
}

void handleBack() {
    int state = lcd.getState();
    
    switch (state) {
        case STATE_MAIN_MENU:
        case STATE_SELECT_PROFILE:
        case STATE_CHANGE_PROFILE:
        case STATE_ADD_PROFILE:
        case STATE_SETTINGS:
        case STATE_INFO:
        case STATE_TEST_OUTPUTS:
            // Return to home screen
            lcd.loopScreen();
            break;
            
        case STATE_START_REFLOW:
        case STATE_STOP_REFLOW:
            // Return to previous screen
            lcd.mainMenuScreen();
            break;
    }
}

void handleSettingsSelection(int setting) {
    switch (setting) {
        case 0:  // Buzzer
            lcd.setBuzzerEnabled(!lcd.getBuzzerEnabled());
            lcd.showSettings();
            break;
        case 1:  // Display orientation
            lcd.setHorizontal(!lcd.getHorizontal());
            lcd.showSettings();
            break;
        case 2:  // OTA updates
            lcd.setOTAEnabled(!lcd.getOTAEnabled());
            lcd.showSettings();
            break;
        case 3:  // Buttons
            lcd.setButtonsEnabled(!lcd.getButtonsEnabled());
            lcd.showSettings();
            break;
        case 4:  // SPIFFS
            lcd.setSPIFFSEnabled(!lcd.getSPIFFSEnabled());
            lcd.showSettings();
            break;
        case 5:  // Fan
            lcd.setFanEnabled(!lcd.getFanEnabled());
            lcd.showSettings();
            break;
        case 6:  // WiFi setup
            // Handle WiFi setup (not implemented in this example)
            break;
        case 7:  // Test outputs
            lcd.testOutputs();
            break;
    }
}

void updateDisplay() {
    // Update display based on current state
    int state = lcd.getState();
    
    switch (state) {
        case STATE_HOME:
            // Home screen updates automatically
            break;
            
        case STATE_MAIN_MENU:
            // Menu updates automatically
            break;
            
        case STATE_SELECT_PROFILE:
            // Profile selection updates automatically
            break;
            
        case STATE_SETTINGS:
            // Settings updates automatically
            break;
            
        case STATE_INFO:
            // Info screen updates automatically
            break;
            
        case STATE_TEST_OUTPUTS:
            // Test screen updates automatically
            break;
    }
}

// Helper functions (you'll need to implement these based on your needs)
bool getUpdateAvailable() {
    // Check if firmware update is available
    return false;
}

bool getBuzzerEnabled() {
    // Get buzzer enabled state
    return true;
}

bool getHorizontal() {
    // Get display orientation
    return true;
}

bool getOTAEnabled() {
    // Get OTA enabled state
    return true;
}

bool getButtonsEnabled() {
    // Get buttons enabled state
    return true;
}

bool getSPIFFSEnabled() {
    // Get SPIFFS enabled state
    return true;
}

bool getFanEnabled() {
    // Get fan enabled state
    return true;
}

bool getTestState() {
    // Get test state
    return false;
}