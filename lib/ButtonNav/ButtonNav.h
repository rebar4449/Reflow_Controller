#ifndef BUTTON_H
#define BUTTON_H

#ifndef ARDUINO_H
#include <Arduino.h>
#endif

// JoystickAxis class definition for analog button support
class JoystickAxis {
private:
    int pin;
    int threshold;
    int lastValue;
    int currentValue;
    bool wasPressed;
    
public:
    JoystickAxis(int axisPin, int pressThreshold = 512) : 
        pin(axisPin), threshold(pressThreshold), lastValue(0), currentValue(0), wasPressed(false) {
        pinMode(pin, INPUT);
    }
    
    void readAxis() {
        lastValue = currentValue;
        currentValue = analogRead(pin);
    }
    
    int wasAxisPressed() {
        // Return 0 for no press, 1 for right/down, 2 for left/up
        if (currentValue > threshold && lastValue <= threshold) {
            return 1; // Right/Down press
        } else if (currentValue < (1023 - threshold) && lastValue >= (1023 - threshold)) {
            return 2; // Left/Up press
        }
        return 0; // No press
    }
    
    int getCurrentValue() const { return currentValue; }
    int getLastValue() const { return lastValue; }
};

// Button configuration constants
#define MAX_DIGITAL_BUTTONS 10
#define DEBOUNCE_TIME 50
#define HOLD_TIME 1000

// Button states
enum ButtonState {
    BUTTON_IDLE,
    BUTTON_PRESSED,
    BUTTON_HELD,
    BUTTON_RELEASED
};

// Menu states (if needed for the main application)
enum MenuState {
    MENU_STATE_MAIN,
    MENU_STATE_EDIT_NUMBER
};

// Reflow states (if needed for the main application)
enum ReflowStatus {
    REFLOW_STATUS_OFF,
    REFLOW_STATUS_ON
};

enum ReflowState {
    REFLOW_STATE_IDLE,
    REFLOW_STATE_RUNNING
};

class ButtonHandler {
private:
    // Digital button arrays
    int digitalButtonPins[MAX_DIGITAL_BUTTONS];
    int numDigButtons;
    int buttonVal[MAX_DIGITAL_BUTTONS];
    int buttonLast[MAX_DIGITAL_BUTTONS];
    unsigned long btnDnTime[MAX_DIGITAL_BUTTONS];
    unsigned long btnUpTime[MAX_DIGITAL_BUTTONS];
    bool ignoreUp[MAX_DIGITAL_BUTTONS];
    bool menuMode[MAX_DIGITAL_BUTTONS];
    
    // Debounce and timing
    unsigned long debounce;
    unsigned long holdTime;
    unsigned long analogLastDebounceTime;
    
    // Event callback function pointers
    void (*event1Callback)(int pin);
    void (*event2Callback)(int pin);
    
    // External dependencies (these should be provided by the main application)
    JoystickAxis* AXIS_X;
    JoystickAxis* AXIS_Y;
    
    // Configuration variables (these should be provided by the main application)
    int verboseOutput;
    int horizontal;
    int buttons;
    int state;
    int previousState;
    int settings_pointer;
    int previousSettingsPointer;
    int numOfPointers;
    int profileIsOn;
    int disableMenu;
    int buzzer;
    int useOTA;
    int useSPIFFS;
    int fan;
    int testState;
    String activeStatus;
    
    // Function pointers for main application callbacks
    void (*startReflowScreen)();
    void (*stopReflowScreen)();
    void (*mainMenuScreen)();
    void (*loopScreen)();
    void (*showSelectProfile)();
    void (*showChangeProfile)();
    void (*showAddProfile)();
    void (*showSettings)();
    void (*showInfo)();
    void (*updateFirmware)();
    void (*saveSelectedProfile)(int profile);
    void (*UpdateSettingsPointer)();
    void (*setBuzzer)(int value);
    void (*setDisplay)(int value);
    void (*setOTA)(int value);
    void (*setButtons)(int value);
    void (*setStorage)(int value);
    void (*setFan)(int value);
    void (*updatePreferences)();
    void (*wifiSetup)();
    void (*testOutputs)();
    void (*testBuzzer)(int value);
    void (*testFan)(int value);
    void (*testSSR)(int value);
    void (*testLED)(int value);
    void (*startScreen)();

public:
    // Constructor
    ButtonHandler();
    
    // Initialization
    void begin(const int* pins, int numButtons, unsigned long debounceTime = DEBOUNCE_TIME, unsigned long holdTimeMs = HOLD_TIME);
    
    // Set external dependencies
    void setJoystickAxes(JoystickAxis* axisX, JoystickAxis* axisY);
    void setConfiguration(int verbose, int horiz, int btn, int st, int prevSt, int setPtr, int prevSetPtr, int numPtr, int profile, int disable, int bzr, int ota, int spiffs, int fn, int testSt, String status);
    
    // Set callback functions
    void setEvent1Callback(void (*callback)(int pin));
    void setEvent2Callback(void (*callback)(int pin));
    
    // Set main application callback functions
    void setMainCallbacks(
        void (*startReflow)(), void (*stopReflow)(), void (*mainMenu)(), void (*loop)(),
        void (*selectProfile)(), void (*changeProfile)(), void (*addProfile)(), void (*settings)(),
        void (*info)(), void (*firmware)(), void (*saveProfile)(int), void (*updatePtr)(),
        void (*setBzr)(int), void (*setDisp)(int), void (*setOta)(int), void (*setBtn)(int),
        void (*setStor)(int), void (*setFn)(int), void (*updatePref)(), void (*wifi)(),
        void (*testOut)(), void (*testBzr)(int), void (*testFn)(int), void (*testSsr)(int),
        void (*testLed)(int), void (*startScr)()
    );
    
    // Main processing functions
    byte processDigitalButtons();
    void readAnalogButtons();
    
    // Event handlers
    void event1(int pin);
    void event2(int pin);
    
    // Getters for state
    int getState() const { return state; }
    int getSettingsPointer() const { return settings_pointer; }
    bool isProfileOn() const { return profileIsOn != 0; }
    
    // Setters for state
    void setState(int newState) { state = newState; }
    void setSettingsPointer(int newPointer) { settings_pointer = newPointer; }
    void setProfileOn(bool on) { profileIsOn = on ? 1 : 0; }
};

// Global instance (if needed)
extern ButtonHandler buttonHandler;

#endif // BUTTON_H
