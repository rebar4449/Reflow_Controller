# LCD Library for ESP32 Reflow Controller

A comprehensive LCD display library for ESP32-based reflow controllers with menu navigation, temperature display, and system configuration features.

## Features

- **Menu Navigation**: Complete menu system with state management
- **Temperature Display**: Real-time temperature monitoring with color-coded status
- **Profile Management**: Support for multiple reflow profiles
- **System Configuration**: Settings for buzzer, fan, buttons, and display orientation
- **Test Functions**: Built-in testing for outputs (buzzer, fan, SSR, LED)
- **WiFi Integration**: Display WiFi status and IP information
- **Update Management**: Firmware update status and progress display

## Installation

1. Download or clone this library to your Arduino libraries folder
2. Install the required dependencies:
   - TFT_eSPI
   - WiFi (included with ESP32)
   - ArduinoJson

## Dependencies

This library requires the following libraries:
- `TFT_eSPI` - For TFT display control
- `WiFi` - For WiFi functionality (included with ESP32)
- `ArduinoJson` - For JSON data handling

## Hardware Requirements

- ESP32 development board
- TFT display (ILI9341 or compatible)
- Optional: Buzzer, fan, SSR, LED for output testing

## Pin Definitions

The library uses the following default pin assignments:
- Buzzer: GPIO 25
- Fan: GPIO 26
- SSR: GPIO 27
- LED: GPIO 2

You can modify these in the `LCD.h` file if needed.

## Basic Usage

```cpp
#include <TFT_eSPI.h>
#include "LCD.h"

TFT_eSPI tft = TFT_eSPI();
LCD lcd(tft);

void setup() {
    Serial.begin(115200);
    
    // Initialize TFT display
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    
    // Initialize LCD library
    lcd.setHorizontal(true);  // Set display orientation
    lcd.setBuzzerEnabled(true);
    lcd.setButtonsEnabled(true);
    lcd.setFanEnabled(true);
    
    // Set initial data
    lcd.setActiveStatus("Ready");
    lcd.setFwVersion("1.0.0");
    lcd.setInputInt(25);  // Set temperature
    
    // Show start screen
    lcd.startScreen();
}

void loop() {
    // Update temperature (example)
    int temperature = readTemperature();  // Your temperature reading function
    lcd.setInputInt(temperature);
    
    // Process menu navigation
    lcd.processMenu();
    
    // Handle button inputs and update display
    handleButtons();
    
    delay(100);
}
```

## Menu Navigation

The library provides several screen methods for different menu states:

### Main Menu
```cpp
lcd.mainMenuScreen();  // Shows main menu with options
```

### Profile Selection
```cpp
lcd.showSelectProfile();  // Shows available reflow profiles
```

### Settings
```cpp
lcd.showSettings();  // Shows system settings menu
```

### Information
```cpp
lcd.showInfo();  // Shows system information
```

### Test Outputs
```cpp
lcd.testOutputs();  // Shows output testing menu
```

## State Management

The library uses a state machine for menu navigation:

- `STATE_HOME` (0): Home screen
- `STATE_MAIN_MENU` (1): Main menu
- `STATE_SELECT_PROFILE` (2): Profile selection
- `STATE_CHANGE_PROFILE` (3): Profile editing
- `STATE_ADD_PROFILE` (4): Add new profile
- `STATE_SETTINGS` (5): Settings menu
- `STATE_INFO` (6): Information screen
- `STATE_START_REFLOW` (7): Start reflow confirmation
- `STATE_STOP_REFLOW` (8): Stop reflow confirmation
- `STATE_TEST_OUTPUTS` (9): Test outputs menu

## Configuration Methods

### Display Settings
```cpp
lcd.setHorizontal(true);  // Set display orientation
```

### Hardware Settings
```cpp
lcd.setBuzzerEnabled(true);    // Enable/disable buzzer
lcd.setButtonsEnabled(true);   // Enable/disable buttons
lcd.setFanEnabled(true);       // Enable/disable fan
lcd.setOTAEnabled(true);       // Enable/disable OTA updates
lcd.setSPIFFSEnabled(true);    // Enable/disable SPIFFS
```

### System Status
```cpp
lcd.setConnected(true);        // Set WiFi connection status
lcd.setFault(false);           // Set fault status
lcd.setUpdateAvailable(false); // Set update availability
```

## Data Setting Methods

```cpp
lcd.setInputInt(25);                    // Set temperature value
lcd.setActiveStatus("Ready");           // Set status message
lcd.setFwVersion("1.0.0");             // Set firmware version
lcd.setProfileUsed(0);                 // Set active profile
lcd.setProfileNum(3);                  // Set number of profiles
lcd.setProfiles(profiles);             // Set profile array
```

## Profile Structure

```cpp
struct ReflowProfile {
    String title;
    String alloy;
    // Add other profile parameters as needed
};

// Example usage
ReflowProfile profiles[] = {
    {"Lead-Free", "SAC305"},
    {"Lead", "Sn63Pb37"},
    {"Custom", "Custom Alloy"}
};

lcd.setProfiles(profiles);
lcd.setProfileNum(3);
```

## Text Rendering

The library provides three text rendering methods:

```cpp
lcd.centeredText("Centered Text", ILI9341_WHITE, 100);  // Center-aligned
lcd.leftText("Left Text", ILI9341_WHITE, 100);          // Left-aligned
lcd.rightText("Right Text", ILI9341_WHITE, 100);        // Right-aligned
```

## Color Definitions

The library includes standard ILI9341 color definitions:
- `ILI9341_BLACK`, `ILI9341_WHITE`
- `ILI9341_RED`, `ILI9341_GREEN`, `ILI9341_BLUE`
- `ILI9341_YELLOW`, `ILI9341_ORANGE`
- And many more...

## Debug Mode

Enable debug output by defining `DEBUG` before including the library:

```cpp
#define DEBUG
#include "LCD.h"
```

## Example Projects

### Basic Temperature Monitor
```cpp
#include <TFT_eSPI.h>
#include "LCD.h"

TFT_eSPI tft = TFT_eSPI();
LCD lcd(tft);

void setup() {
    tft.init();
    tft.setRotation(1);
    
    lcd.setHorizontal(true);
    lcd.setActiveStatus("Monitoring");
    lcd.startScreen();
}

void loop() {
    int temp = analogRead(36) * 0.1;  // Simple temperature reading
    lcd.setInputInt(temp);
    lcd.loopScreen();
    delay(1000);
}
```

### Menu Navigation Example
```cpp
#include <TFT_eSPI.h>
#include "LCD.h"

TFT_eSPI tft = TFT_eSPI();
LCD lcd(tft);

void setup() {
    tft.init();
    tft.setRotation(1);
    
    lcd.setHorizontal(true);
    lcd.setButtonsEnabled(true);
    lcd.mainMenuScreen();
}

void loop() {
    // Handle button presses
    if (digitalRead(BUTTON_UP) == LOW) {
        int pointer = lcd.getSettingsPointer();
        lcd.setSettingsPointer(pointer - 1);
    }
    if (digitalRead(BUTTON_DOWN) == LOW) {
        int pointer = lcd.getSettingsPointer();
        lcd.setSettingsPointer(pointer + 1);
    }
    if (digitalRead(BUTTON_SELECT) == LOW) {
        // Handle selection based on current state
        int state = lcd.getState();
        switch(state) {
            case STATE_MAIN_MENU:
                // Navigate to selected menu item
                break;
        }
    }
    
    delay(50);
}
```

## Troubleshooting

### Common Issues

1. **Display not showing**: Check TFT_eSPI configuration and pin connections
2. **Text not centered**: Verify display dimensions in TFT_eSPI configuration
3. **Menu not responding**: Ensure button pins are correctly configured
4. **Memory issues**: Reduce text size or optimize profile data

### Debug Tips

- Enable debug mode to see text bounds calculations
- Check Serial output for error messages
- Verify all required libraries are installed
- Test individual components (display, buttons) separately

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

## License

This library is released under the MIT License. See LICENSE file for details.

## Changelog

### Version 1.0.0
- Initial release
- Complete menu system
- Temperature display
- Profile management
- Settings configuration
- Output testing
- WiFi integration
