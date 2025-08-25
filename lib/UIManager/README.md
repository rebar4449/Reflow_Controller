# UIManager Library

A UI manager for the Reflow Controller that handles different screens and touch interactions. This library provides a complete user interface system with screen navigation, button management, and touch interaction handling.

## Features

- Multiple screen management (Main, Profile Select, Settings, Reflow Running, Info)
- Automatic screen transitions based on reflow state
- Touch button integration
- Temperature and status display updates
- Profile selection interface
- Settings management

## Dependencies

- TouchInterface Library
- Adafruit GFX Library
- Adafruit ILI9341 Library

## Usage

### Basic Setup

```cpp
#include "UIManager.h"
#include "TouchInterface.h"
#include <XPT2046_Touchscreen.h>
#include <Adafruit_ILI9341.h>

// Create instances
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);
Adafruit_ILI9341 display = Adafruit_ILI9341(display_cs, display_dc, display_rst);
TouchInterface* touchInterface = new TouchInterface(&ts, &display);
UIManager* uiManager = new UIManager(touchInterface, &display);

void setup() {
  display.begin();
  touchInterface->begin();
  uiManager->begin();
}

void loop() {
  uiManager->update();
  uiManager->updateTemperature(currentTemp);
  uiManager->updateStatus(currentStatus);
}
```

## Screens

### Main Screen
- Displays current temperature
- Start button to begin reflow process
- Settings button to access configuration
- Info button for system information

### Profile Select Screen
- Lists available reflow profiles
- Touch buttons for each profile
- Back button to return to main screen

### Settings Screen
- Configuration options (Fan, Buzzer, OTA, etc.)
- Toggle buttons for each setting
- Back button to return to main screen

### Reflow Running Screen
- Real-time temperature display
- Current setpoint display
- Stop button to cancel reflow process
- Status information

### Info Screen
- System information
- Firmware version
- Current profile details
- Back button to return to main screen

## API Reference

### Constructor
```cpp
UIManager(TouchInterface* touch, Adafruit_ILI9341* tft)
```

### Methods

#### begin()
Initialize the UI manager and draw the initial screen.

#### update()
Update the UI based on current state and process touch input.

#### switchToScreen(screen)
Switch to a specific screen.
- `screen`: ScreenState enum value

#### updateTemperature(temperature)
Update the temperature display.
- `temperature`: Current temperature value

#### updateStatus(status)
Update the status display.
- `status`: Status string

#### getCurrentScreen()
Get the current screen state.

## Screen States

```cpp
enum ScreenState {
  SCREEN_MAIN,           // Main menu screen
  SCREEN_PROFILE_SELECT, // Profile selection screen
  SCREEN_SETTINGS,       // Settings screen
  SCREEN_REFLOW_RUNNING, // Reflow process screen
  SCREEN_INFO           // Information screen
};
```

## Integration with Reflow Logic

The UIManager integrates with the reflow logic by:
- Monitoring `profileIsOn` state to automatically switch screens
- Providing callbacks for profile selection and reflow control
- Updating temperature and status displays in real-time
- Handling touch interactions for all reflow operations

## License

This library is released under the MIT License.
