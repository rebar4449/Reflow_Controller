#ifndef LCD_H
#define LCD_H

#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "TouchInterface.h"

// Color definitions
#define ILI9341_BLACK       0x0000
#define ILI9341_NAVY        0x000F
#define ILI9341_DARKGREEN   0x03E0
#define ILI9341_DARKCYAN    0x03EF
#define ILI9341_MAROON      0x7800
#define ILI9341_PURPLE      0x780F
#define ILI9341_OLIVE       0x7BE0
#define ILI9341_LIGHTGREY   0xC618
#define ILI9341_DARKGREY    0x7BEF
#define ILI9341_BLUE        0x001F
#define ILI9341_GREEN       0x07E0
#define ILI9341_CYAN        0x07FF
#define ILI9341_RED         0xF800
#define ILI9341_MAGENTA     0xF81F
#define ILI9341_YELLOW      0xFFE0
#define ILI9341_WHITE       0xFFFF
#define ILI9341_ORANGE      0xFD20
#define ILI9341_GREENYELLOW 0xAFE5
#define ILI9341_PINK        0xF81F

// Pin definitions
#define buzzerPin 25
#define fanPin 26
#define ssrPin 27
#define ledPin 2

// State definitions
#define STATE_HOME 0
#define STATE_MAIN_MENU 1
#define STATE_SELECT_PROFILE 2
#define STATE_CHANGE_PROFILE 3
#define STATE_ADD_PROFILE 4
#define STATE_SETTINGS 5
#define STATE_INFO 6
#define STATE_START_REFLOW 7
#define STATE_STOP_REFLOW 8
#define STATE_TEST_OUTPUTS 9

// Profile structure
struct ReflowProfile {
    String title;
    String alloy;
    // Add other profile parameters as needed
};

class LCD {
private:
    Adafruit_ILI9341& display;
    TouchInterface* touchInterface;
    
    // State management
    int state;
    int previousState;
    int settings_pointer;
    int previousSettingsPointer;
    int numOfPointers;
    int activeMenu;
    
    // Configuration flags
    bool horizontal;
    bool buzzer;
    bool buttons;
    bool fan;
    bool useOTA;
    bool useSPIFFS;
    bool updataAvailable;
    bool disableMenu;
    bool connected;
    bool isFault;
    bool testState;
    
    // Data
    int inputInt;
    String activeStatus;
    String fwVersion;
    int profileUsed;
    int profileNum;
    ReflowProfile* paste_profile;
    
    // Menu items
    const char* menuItems[5] = {"Select profile", "Change profile", "Add profile", "Settings", "Info"};
    
    // Private helper methods
    void UpdateSettingsPointer();
    void ShowMenuOptions(bool clearAll);
    
    // Settings display methods
    void setBuzzer(int y);
    void setButtons(int y);
    void setFan(int y);
    void setDisplay(int y);
    void setOTA(int y);
    void setStorage(int y);
    void wifiSetupShow(int y);
    
    // Test methods
    void testBuzzer(int y);
    void testFan(int y);
    void testSSR(int y);
    void testLED(int y);

public:
    // Constructor
    LCD(Adafruit_ILI9341& tft, TouchInterface* touch = nullptr);
    
    // Destructor
    ~LCD();
    
    // Touch interface methods
    void setTouchInterface(TouchInterface* touch);
    void processTouch();
    
    // Text rendering methods
    void centeredText(String text, uint16_t color, int yCord, int xCord = 0);
    void rightText(String text, uint16_t color, int yCord, int xCord = 0);
    void leftText(String text, uint16_t color, int yCord, int xCord = 0);
    
    // Main menu processing
    void processMenu();
    
    // Screen display methods
    void startScreen();
    void loopScreen();
    void mainMenuScreen();
    void showSelectProfile();
    void showChangeProfile();
    void showAddProfile();
    void showSettings(byte pointer = 0);
    void showInfo();
    void infoScreen();
    void startReflowScreen();
    void stopReflowScreen();
    void startUpdateScreen();
    void updateOK();
    void testOutputs();
    
    // Configuration methods
    void setHorizontal(bool horizontal);
    void setBuzzerEnabled(bool enabled);
    void setButtonsEnabled(bool enabled);
    void setFanEnabled(bool enabled);
    void setOTAEnabled(bool enabled);
    void setSPIFFSEnabled(bool enabled);
    void setUpdateAvailable(bool available);
    void setMenuDisabled(bool disabled);
    void setConnected(bool connected);
    void setFault(bool fault);
    void setTestState(bool state);
    
    // Data setting methods
    void setInputInt(int value);
    void setActiveStatus(String status);
    void setFwVersion(String version);
    void setProfileUsed(int profile);
    void setProfileNum(int num);
    void setProfiles(ReflowProfile* profiles);
    
    // Getter methods
    int getState() const;
    int getSettingsPointer() const;
    int getNumOfPointers() const;
    
    // Configuration getter methods
    bool getHorizontal() const;
    bool getBuzzerEnabled() const;
    bool getButtonsEnabled() const;
    bool getFanEnabled() const;
    bool getOTAEnabled() const;
    bool getSPIFFSEnabled() const;
    bool getUpdateAvailable() const;
    bool getMenuDisabled() const;
    bool getConnected() const;
    bool getFault() const;
    bool getTestState() const;
    
    // State management
    void setState(int newState);
    void setSettingsPointer(int pointer);
    void setNumOfPointers(int num);
};

#endif // LCD_H
