# Button Handler Library

A C++ library for Arduino that provides debounced button input handling with support for both digital buttons and analog joystick inputs. This library was converted from an Arduino INO file to a proper C++ class structure.

## Features

- **Debounced Digital Button Input**: Handles multiple digital buttons with configurable debounce timing
- **Analog Joystick Support**: Processes analog joystick inputs for navigation
- **Event Callbacks**: Supports custom event handlers for button presses and holds
- **Menu Navigation**: Built-in support for menu navigation with settings pointers
- **Hold Detection**: Detects button hold events for different actions
- **Configurable Timing**: Adjustable debounce and hold times
- **State Management**: Tracks button states and menu states

## Files

- `button.h` - Header file with class declarations and constants
- `button.cpp` - Implementation file with all the button handling logic
- `button_example.cpp` - Example usage showing how to integrate with your Arduino sketch

## Usage

### Basic Setup

```cpp
#include "button.h"

// Define your button pins
const int BUTTON_PINS[] = {27, 32, 33}; // Select, Menu, Back buttons
const int NUM_BUTTONS = 3;

void setup() {
    // Initialize the button handler
    buttonHandler.begin(BUTTON_PINS, NUM_BUTTONS);
    
    // Set configuration
    buttonHandler.setConfiguration(
        verboseOutput, horizontal, buttons, state, previousState,
        settings_pointer, previousSettingsPointer, numOfPointers,
        profileIsOn, disableMenu, buzzer, useOTA, useSPIFFS, fan,
        testState, activeStatus
    );
}

void loop() {
    // Process digital buttons
    buttonHandler.processDigitalButtons();
    
    // Process analog buttons (joystick)
    buttonHandler.readAnalogButtons();
}
```

### Configuration

The library requires several configuration variables to be set:

```cpp
// Application state variables
int verboseOutput = 1;        // Enable/disable debug output
int horizontal = 0;           // Display orientation
int buttons = 1;              // Button configuration
int state = 0;                // Current application state
int previousState = 0;        // Previous application state
int settings_pointer = 0;     // Current menu position
int previousSettingsPointer = 0; // Previous menu position
int numOfPointers = 5;        // Number of menu items
int profileIsOn = 0;          // Profile status
int disableMenu = 0;          // Menu enable/disable
int buzzer = 0;               // Buzzer state
int useOTA = 0;               // OTA update setting
int useSPIFFS = 0;            // Storage setting
int fan = 0;                  // Fan state
int testState = LOW;          // Test mode state
String activeStatus = "Idle"; // Status string
```

### Joystick Integration

To use analog joystick inputs, you need to provide joystick axis objects:

```cpp
// Include your joystick library
#include "YourJoystickLibrary.h"

// Create joystick instances
JoystickAxis AXIS_X;
JoystickAxis AXIS_Y;

void setup() {
    // Set joystick axes
    buttonHandler.setJoystickAxes(&AXIS_X, &AXIS_Y);
}
```

### Custom Event Handlers

You can set custom event handlers for button events:

```cpp
void customEvent1(int pin) {
    Serial.println("Button pressed on pin: " + String(pin));
}

void customEvent2(int pin) {
    Serial.println("Button held on pin: " + String(pin));
}

void setup() {
    // Set custom event callbacks
    buttonHandler.setEvent1Callback(customEvent1);
    buttonHandler.setEvent2Callback(customEvent2);
}
```

### Main Application Callbacks

The library provides extensive callback support for menu navigation and settings:

```cpp
void setup() {
    // Set all main application callbacks
    buttonHandler.setMainCallbacks(
        startReflowScreen, stopReflowScreen, mainMenuScreen, loopScreen,
        showSelectProfile, showChangeProfile, showAddProfile, showSettings,
        showInfo, updateFirmware, saveSelectedProfile, UpdateSettingsPointer,
        setBuzzer, setDisplay, setOTA, setButtons, setStorage, setFan,
        updatePreferences, wifiSetup, testOutputs, testBuzzer, testFan,
        testSSR, testLED, startScreen
    );
}
```

## Button Pin Assignments

The library expects specific pin assignments for different functions:

- **Pin 27**: Select button
- **Pin 32**: Menu button  
- **Pin 33**: Back button

## Joystick Axis Methods

Your joystick library should provide these methods:

```cpp
class JoystickAxis {
public:
    void readAxis();
    int wasAxisPressed(); // Returns 1 for positive, 2 for negative, 0 for no press
};
```

## Constants

The library defines several constants:

```cpp
#define MAX_DIGITAL_BUTTONS 10
#define DEBOUNCE_TIME 50      // milliseconds
#define HOLD_TIME 1000        // milliseconds
```

## State Management

The library manages several states:

- **Button States**: IDLE, PRESSED, HELD, RELEASED
- **Menu States**: MAIN, EDIT_NUMBER
- **Reflow States**: OFF, ON, IDLE, RUNNING

## Dependencies

- Arduino.h (included automatically)
- Your joystick library (if using analog inputs)

## Notes

- The library uses `INPUT_PULLUP` mode for digital buttons
- All timing is based on `millis()` for non-blocking operation
- The library is designed to be non-blocking and should be called regularly in the main loop
- Debug output can be controlled via the `verboseOutput` configuration variable

## Example Integration

See `button_example.cpp` for a complete example of how to integrate this library into your Arduino project.
