/*
 * Button Handler Library Example
 * 
 * This example shows how to use the ButtonHandler library in your main Arduino sketch.
 * The ButtonHandler class provides debounced button input handling with support for
 * both digital buttons and analog joystick inputs.
 */

#include "ButtonNav.h"

// Include your joystick library (replace with your actual joystick library)
// #include "YourJoystickLibrary.h"

// Define your button pins
const int BUTTON_PINS[] = {27, 32, 33}; // Select, Menu, Back buttons
const int NUM_BUTTONS = 3;

// Create joystick instances (replace with your actual joystick class)
// JoystickAxis AXIS_X;
// JoystickAxis AXIS_Y;

// Global variables for your application state
int verboseOutput = 1;
int horizontal = 0;
int buttons = 1;
int state = 0;
int previousState = 0;
int settings_pointer = 0;
int previousSettingsPointer = 0;
int numOfPointers = 5;
int profileIsOn = 0;
int disableMenu = 0;
int buzzer = 0;
int useOTA = 0;
int useSPIFFS = 0;
int fan = 0;
int testState = LOW;
String activeStatus = "Idle";

void setup() {
    Serial.begin(115200);
    Serial.println("Button Handler Example");
    
    // Initialize the button handler
    buttonHandler.begin(BUTTON_PINS, NUM_BUTTONS);
    
    // Set joystick axes (uncomment when you have your joystick library)
    // buttonHandler.setJoystickAxes(&AXIS_X, &AXIS_Y);
    
    // Set configuration
    buttonHandler.setConfiguration(
        verboseOutput, horizontal, buttons, state, previousState,
        settings_pointer, previousSettingsPointer, numOfPointers,
        profileIsOn, disableMenu, buzzer, useOTA, useSPIFFS, fan,
        testState, activeStatus
    );
    
    // Set custom event callbacks (optional)
    buttonHandler.setEvent1Callback(customEvent1);
    buttonHandler.setEvent2Callback(customEvent2);
    
    // Set main application callbacks
    buttonHandler.setMainCallbacks(
        startReflowScreen, stopReflowScreen, mainMenuScreen, loopScreen,
        showSelectProfile, showChangeProfile, showAddProfile, showSettings,
        showInfo, updateFirmware, saveSelectedProfile, UpdateSettingsPointer,
        setBuzzer, setDisplay, setOTA, setButtons, setStorage, setFan,
        updatePreferences, wifiSetup, testOutputs, testBuzzer, testFan,
        testSSR, testLED, startScreen
    );
}

void loop() {
    // Process digital buttons
    buttonHandler.processDigitalButtons();
    
    // Process analog buttons (joystick)
    buttonHandler.readAnalogButtons();
    
    // Your other application code here
    delay(10); // Small delay to prevent overwhelming the system
}

// Custom event handlers (optional - you can use these instead of the built-in ones)
void customEvent1(int pin) {
    Serial.println("Custom Event 1 triggered on pin: " + String(pin));
    // Your custom logic here
}

void customEvent2(int pin) {
    Serial.println("Custom Event 2 triggered on pin: " + String(pin));
    // Your custom logic here
}

// Main application callback functions
void startReflowScreen() {
    Serial.println("Starting reflow screen");
    state = 7;
    profileIsOn = 1;
}

void stopReflowScreen() {
    Serial.println("Stopping reflow screen");
    state = 8;
    profileIsOn = 0;
}

void mainMenuScreen() {
    Serial.println("Showing main menu");
    state = 1;
    settings_pointer = 0;
}

void loopScreen() {
    Serial.println("Showing loop screen");
    state = 0;
}

void showSelectProfile() {
    Serial.println("Showing select profile");
    state = 2;
}

void showChangeProfile() {
    Serial.println("Showing change profile");
    state = 3;
}

void showAddProfile() {
    Serial.println("Showing add profile");
    state = 4;
}

void showSettings() {
    Serial.println("Showing settings");
    state = 5;
}

void showInfo() {
    Serial.println("Showing info");
    state = 6;
}

void updateFirmware() {
    Serial.println("Updating firmware");
}

void saveSelectedProfile(int profile) {
    Serial.println("Saving selected profile: " + String(profile));
}

void UpdateSettingsPointer() {
    Serial.println("Updating settings pointer to: " + String(settings_pointer));
}

void setBuzzer(int value) {
    Serial.println("Setting buzzer: " + String(value));
    buzzer = value;
}

void setDisplay(int value) {
    Serial.println("Setting display: " + String(value));
    horizontal = value;
}

void setOTA(int value) {
    Serial.println("Setting OTA: " + String(value));
    useOTA = value;
}

void setButtons(int value) {
    Serial.println("Setting buttons: " + String(value));
    buttons = value;
}

void setStorage(int value) {
    Serial.println("Setting storage: " + String(value));
    useSPIFFS = value;
}

void setFan(int value) {
    Serial.println("Setting fan: " + String(value));
    fan = value;
}

void updatePreferences() {
    Serial.println("Updating preferences");
}

void wifiSetup() {
    Serial.println("Setting up WiFi");
}

void testOutputs() {
    Serial.println("Testing outputs");
}

void testBuzzer(int value) {
    Serial.println("Testing buzzer: " + String(value));
}

void testFan(int value) {
    Serial.println("Testing fan: " + String(value));
}

void testSSR(int value) {
    Serial.println("Testing SSR: " + String(value));
}

void testLED(int value) {
    Serial.println("Testing LED: " + String(value));
}

void startScreen() {
    Serial.println("Starting screen");
    state = 0;
}
