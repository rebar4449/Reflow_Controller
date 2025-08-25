#include "LCD.h"

// Constructor
LCD::LCD(Adafruit_ILI9341& tft, TouchInterface* touch) : display(tft), touchInterface(touch) {
    // Initialize state variables
    state = STATE_HOME;
    previousState = STATE_HOME;
    settings_pointer = 0;
    previousSettingsPointer = 0;
    numOfPointers = 5;
    activeMenu = 0;
    
    // Initialize configuration flags
    horizontal = true;
    buzzer = true;
    buttons = true;
    fan = true;
    useOTA = true;
    useSPIFFS = true;
    updataAvailable = false;
    disableMenu = false;
    connected = false;
    isFault = false;
    testState = false;
    
    // Initialize data
    inputInt = 0;
    activeStatus = "Ready";
    fwVersion = "1.0.0";
    profileUsed = 0;
    profileNum = 0;
    paste_profile = nullptr;
}

// Destructor
LCD::~LCD() {
    // Cleanup if needed
}

// Touch interface methods
void LCD::setTouchInterface(TouchInterface* touch) {
    touchInterface = touch;
}

void LCD::processTouch() {
    if (touchInterface) {
        touchInterface->processTouch();
    }
}

// Text rendering methods
void LCD::centeredText(String text, uint16_t color, int yCord, int xCord) {
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    
    int centerX = (display.width() - w) / 2;
    if (xCord != 0) centerX = xCord;
    
    display.setCursor(centerX, yCord);
    display.setTextColor(color);
    display.print(text);
}

void LCD::rightText(String text, uint16_t color, int yCord, int xCord) {
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    
    int rightX = display.width() - w - 10;
    if (xCord != 0) rightX = xCord;
    
    display.setCursor(rightX, yCord);
    display.setTextColor(color);
    display.print(text);
}

void LCD::leftText(String text, uint16_t color, int yCord, int xCord) {
    int leftX = 10;
    if (xCord != 0) leftX = xCord;
    
    display.setCursor(leftX, yCord);
    display.setTextColor(color);
    display.print(text);
}

// Screen display methods
void LCD::startScreen() {
    display.fillScreen(ILI9341_BLACK);
    display.setTextSize(2);
    centeredText("Reflow Controller", ILI9341_WHITE, 50);
    display.setTextSize(1);
    centeredText("Starting...", ILI9341_CYAN, 100);
    state = STATE_HOME;
}

void LCD::loopScreen() {
    display.fillScreen(ILI9341_BLACK);
    
    // Draw header
    display.setTextSize(2);
    centeredText("Reflow Controller", ILI9341_WHITE, 20);
    
    // Draw temperature
    display.setTextSize(3);
    centeredText(String(inputInt) + "C", ILI9341_YELLOW, 80);
    
    // Draw status
    display.setTextSize(1);
    centeredText("Status: " + activeStatus, ILI9341_CYAN, 140);
    
    // Draw connection status
    if (connected) {
        leftText("WiFi: Connected", ILI9341_GREEN, 180);
    } else {
        leftText("WiFi: Disconnected", ILI9341_RED, 180);
    }
    
    // Draw fault status
    if (isFault) {
        rightText("FAULT", ILI9341_RED, 180);
    }
}

void LCD::mainMenuScreen() {
    display.fillScreen(ILI9341_BLACK);
    
    // Draw header
    display.setTextSize(2);
    centeredText("Main Menu", ILI9341_WHITE, 20);
    
    // Draw menu items
    display.setTextSize(1);
    for (int i = 0; i < numOfPointers; i++) {
        uint16_t color = (i == settings_pointer) ? ILI9341_YELLOW : ILI9341_WHITE;
        centeredText(menuItems[i], color, 60 + (i * 25));
    }
}

void LCD::showSelectProfile() {
    display.fillScreen(ILI9341_BLACK);
    
    // Draw header
    display.setTextSize(2);
    centeredText("Select Profile", ILI9341_WHITE, 20);
    
    // Draw profiles if available
    if (paste_profile && profileNum > 0) {
        display.setTextSize(1);
        for (int i = 0; i < profileNum && i < 5; i++) {
            uint16_t color = (i == settings_pointer) ? ILI9341_YELLOW : ILI9341_WHITE;
            String profileText = String(i + 1) + ": " + paste_profile[i].title;
            centeredText(profileText, color, 60 + (i * 25));
        }
    } else {
        centeredText("No profiles available", ILI9341_RED, 100);
    }
}

void LCD::showSettings(byte pointer) {
    display.fillScreen(ILI9341_BLACK);
    
    // Draw header
    display.setTextSize(2);
    centeredText("Settings", ILI9341_WHITE, 20);
    
    // Draw settings options
    display.setTextSize(1);
    setBuzzer(60);
    setButtons(85);
    setFan(110);
    setDisplay(135);
    setOTA(160);
    setStorage(185);
}

void LCD::showInfo() {
    display.fillScreen(ILI9341_BLACK);
    
    // Draw header
    display.setTextSize(2);
    centeredText("System Info", ILI9341_WHITE, 20);
    
    // Draw info
    display.setTextSize(1);
    leftText("Firmware: " + fwVersion, ILI9341_WHITE, 60);
    leftText("Status: " + activeStatus, ILI9341_CYAN, 85);
    leftText("Temperature: " + String(inputInt) + "C", ILI9341_YELLOW, 110);
    
    if (connected) {
        leftText("WiFi: Connected", ILI9341_GREEN, 135);
    } else {
        leftText("WiFi: Disconnected", ILI9341_RED, 135);
    }
}

void LCD::startReflowScreen() {
    display.fillScreen(ILI9341_BLACK);
    
    // Draw header
    display.setTextSize(2);
    centeredText("Reflow Running", ILI9341_WHITE, 20);
    
    // Draw temperature
    display.setTextSize(3);
    centeredText(String(inputInt) + "C", ILI9341_YELLOW, 80);
    
    // Draw status
    display.setTextSize(1);
    centeredText("Status: " + activeStatus, ILI9341_CYAN, 140);
    
    // Draw stop instruction
    centeredText("Touch to stop", ILI9341_RED, 180);
}

void LCD::stopReflowScreen() {
    display.fillScreen(ILI9341_BLACK);
    
    // Draw header
    display.setTextSize(2);
    centeredText("Reflow Stopped", ILI9341_WHITE, 20);
    
    // Draw final temperature
    display.setTextSize(3);
    centeredText(String(inputInt) + "C", ILI9341_YELLOW, 80);
    
    // Draw status
    display.setTextSize(1);
    centeredText("Status: " + activeStatus, ILI9341_CYAN, 140);
}

// Settings display methods
void LCD::setBuzzer(int y) {
    String text = "Buzzer: " + String(buzzer ? "ON" : "OFF");
    uint16_t color = (settings_pointer == 0) ? ILI9341_YELLOW : (buzzer ? ILI9341_GREEN : ILI9341_RED);
    leftText(text, color, y);
}

void LCD::setButtons(int y) {
    String text = "Buttons: " + String(buttons ? "ON" : "OFF");
    uint16_t color = (settings_pointer == 1) ? ILI9341_YELLOW : (buttons ? ILI9341_GREEN : ILI9341_RED);
    leftText(text, color, y);
}

void LCD::setFan(int y) {
    String text = "Fan: " + String(fan ? "ON" : "OFF");
    uint16_t color = (settings_pointer == 2) ? ILI9341_YELLOW : (fan ? ILI9341_GREEN : ILI9341_RED);
    leftText(text, color, y);
}

void LCD::setDisplay(int y) {
    String text = "Display: " + String(horizontal ? "Horizontal" : "Vertical");
    uint16_t color = (settings_pointer == 3) ? ILI9341_YELLOW : ILI9341_WHITE;
    leftText(text, color, y);
}

void LCD::setOTA(int y) {
    String text = "OTA: " + String(useOTA ? "ON" : "OFF");
    uint16_t color = (settings_pointer == 4) ? ILI9341_YELLOW : (useOTA ? ILI9341_GREEN : ILI9341_RED);
    leftText(text, color, y);
}

void LCD::setStorage(int y) {
    String text = "Storage: " + String(useSPIFFS ? "SPIFFS" : "SD");
    uint16_t color = (settings_pointer == 5) ? ILI9341_YELLOW : ILI9341_WHITE;
    leftText(text, color, y);
}

// Configuration methods
void LCD::setHorizontal(bool horizontal) { this->horizontal = horizontal; }
void LCD::setBuzzerEnabled(bool enabled) { this->buzzer = enabled; }
void LCD::setButtonsEnabled(bool enabled) { this->buttons = enabled; }
void LCD::setFanEnabled(bool enabled) { this->fan = enabled; }
void LCD::setOTAEnabled(bool enabled) { this->useOTA = enabled; }
void LCD::setSPIFFSEnabled(bool enabled) { this->useSPIFFS = enabled; }
void LCD::setUpdateAvailable(bool available) { this->updataAvailable = available; }
void LCD::setMenuDisabled(bool disabled) { this->disableMenu = disabled; }
void LCD::setConnected(bool connected) { this->connected = connected; }
void LCD::setFault(bool fault) { this->isFault = fault; }
void LCD::setTestState(bool state) { this->testState = state; }

// Data setting methods
void LCD::setInputInt(int value) { this->inputInt = value; }
void LCD::setActiveStatus(String status) { this->activeStatus = status; }
void LCD::setFwVersion(String version) { this->fwVersion = version; }
void LCD::setProfileUsed(int profile) { this->profileUsed = profile; }
void LCD::setProfileNum(int num) { this->profileNum = num; }
void LCD::setProfiles(ReflowProfile* profiles) { this->paste_profile = profiles; }

// Getter methods
int LCD::getState() const { return state; }
int LCD::getSettingsPointer() const { return settings_pointer; }
int LCD::getNumOfPointers() const { return numOfPointers; }

bool LCD::getHorizontal() const { return horizontal; }
bool LCD::getBuzzerEnabled() const { return buzzer; }
bool LCD::getButtonsEnabled() const { return buttons; }
bool LCD::getFanEnabled() const { return fan; }
bool LCD::getOTAEnabled() const { return useOTA; }
bool LCD::getSPIFFSEnabled() const { return useSPIFFS; }
bool LCD::getUpdateAvailable() const { return updataAvailable; }
bool LCD::getMenuDisabled() const { return disableMenu; }
bool LCD::getConnected() const { return connected; }
bool LCD::getFault() const { return isFault; }
bool LCD::getTestState() const { return testState; }

// State management
void LCD::setState(int newState) { state = newState; }
void LCD::setSettingsPointer(int pointer) { settings_pointer = pointer; }
void LCD::setNumOfPointers(int num) { numOfPointers = num; }

// Menu processing (simplified for touch interface)
void LCD::processMenu() {
    // This method will be handled by UIManager for touch interface
    // Keeping it for compatibility
}

// Additional methods for compatibility
void LCD::showChangeProfile() { showSelectProfile(); }
void LCD::showAddProfile() { showSelectProfile(); }
void LCD::infoScreen() { showInfo(); }
void LCD::startUpdateScreen() { showInfo(); }
void LCD::updateOK() { showInfo(); }
void LCD::testOutputs() { showInfo(); }
void LCD::wifiSetupShow(int y) { leftText("WiFi Setup", ILI9341_WHITE, y); }
void LCD::testBuzzer(int y) { leftText("Test Buzzer", ILI9341_WHITE, y); }
void LCD::testFan(int y) { leftText("Test Fan", ILI9341_WHITE, y); }
void LCD::testSSR(int y) { leftText("Test SSR", ILI9341_WHITE, y); }
void LCD::testLED(int y) { leftText("Test LED", ILI9341_WHITE, y); }
void LCD::UpdateSettingsPointer() { /* Handled by UIManager */ }
