# Arduino Button Library

A comprehensive Arduino library for reading momentary contact switches like tactile button switches and analog joystick inputs. This library provides debounced button input handling with support for both digital buttons and analog axis inputs, making it ideal for use in state machine constructs and user interface applications.

## Features

- **Digital Button Support**: Read and debounce digital button inputs
- **Analog Axis Support**: Read analog joystick/thumbstick inputs with configurable thresholds
- **Debouncing**: Built-in hardware debouncing to prevent false triggers
- **State Tracking**: Track button press, release, and hold states
- **Timing Functions**: Measure press duration and time since last state change
- **Pull-up Resistor Support**: Automatic internal pull-up resistor configuration
- **Invert Logic**: Support for both active-high and active-low button configurations
- **Multiple Input Types**: Support for both digital pins and analog pins

## Installation

1. Download the library files
2. Place the `Button` folder in your Arduino `libraries` directory
3. Restart the Arduino IDE
4. Include the library in your sketch: `#include "Button.h"`

## Basic Usage

### Digital Button Example

```cpp
#include "Button.h"

// Create a button object
// Parameters: pin, invert (0=active high, 1=active low), debounce time (ms)
Button myButton(2, 0, 50);

void setup() {
    Serial.begin(9600);
}

void loop() {
    // Read the button state (call this as often as possible)
    myButton.read();
    
    // Check if button is currently pressed
    if (myButton.isPressed()) {
        Serial.println("Button is pressed!");
    }
    
    // Check if button was just pressed (since last read)
    if (myButton.wasPressed()) {
        Serial.println("Button was just pressed!");
    }
    
    // Check if button was just released
    if (myButton.wasReleased()) {
        Serial.println("Button was just released!");
    }
    
    // Check if button has been pressed for 2 seconds
    if (myButton.pressedFor(2000)) {
        Serial.println("Button held for 2 seconds!");
    }
}
```

### Analog Axis Example

```cpp
#include "Button.h"

// Create a button object for analog input
Button joystickButton(A0, 0, 50);

void setup() {
    Serial.begin(9600);
}

void loop() {
    // Read the analog axis
    joystickButton.readAxis();
    
    // Check if axis is pressed and get intensity
    uint8_t axisValue = joystickButton.isAxisPressed();
    if (axisValue == DPAD_V_FULL) {
        Serial.println("Full press detected!");
    } else if (axisValue == DPAD_V_HALF) {
        Serial.println("Half press detected!");
    }
    
    // Check if axis was just pressed
    uint8_t wasAxisPressed = joystickButton.wasAxisPressed();
    if (wasAxisPressed) {
        Serial.print("Axis was pressed with intensity: ");
        Serial.println(wasAxisPressed);
    }
}
```

## API Reference

### Constructor

```cpp
Button(uint8_t pin, uint8_t invert, uint32_t dbTime)
```

**Parameters:**
- `pin`: Arduino pin number the button is connected to
- `invert`: If 0, interprets high state as pressed, low as released. If non-zero, interprets low state as pressed, high as released
- `dbTime`: Debounce time in milliseconds

### Digital Button Methods

#### `uint8_t read()`
Reads the current state of the button with debouncing. Returns 1 if pressed, 0 if released.

#### `uint8_t isPressed()`
Returns 1 if the button is currently pressed, 0 if released. Does not cause the button to be read.

#### `uint8_t isReleased()`
Returns 1 if the button is currently released, 0 if pressed. Does not cause the button to be read.

#### `uint8_t wasPressed()`
Returns 1 if the button was pressed since the last read, 0 otherwise.

#### `uint8_t wasReleased()`
Returns 1 if the button was released since the last read, 0 otherwise.

#### `uint8_t pressedFor(uint32_t ms)`
Returns 1 if the button is pressed and has been in that state for the specified time in milliseconds.

#### `uint8_t releasedFor(uint32_t ms)`
Returns 1 if the button is released and has been in that state for the specified time in milliseconds.

#### `uint32_t lastChange()`
Returns the time (in milliseconds) when the button last changed state.

### Analog Axis Methods

#### `uint8_t readAxis()`
Reads the analog axis value and converts it to button states. Returns 1 if pressed, 0 if released.

#### `uint8_t isAxisPressed()`
Returns the axis intensity:
- `DPAD_V_NONE` (0): No press detected
- `DPAD_V_HALF` (1): Half press detected (analog value between 1500-2000)
- `DPAD_V_FULL` (2): Full press detected (analog value > 3900)

#### `uint8_t wasAxisPressed()`
Returns the axis intensity if the axis was just pressed, 0 otherwise.

### Constants

```cpp
#define DPAD_V_FULL  2    // Full press threshold
#define DPAD_V_HALF  1    // Half press threshold  
#define DPAD_V_NONE  0    // No press
```

## Advanced Usage

### Multiple Buttons

```cpp
#include "Button.h"

Button button1(2, 0, 50);  // Digital button on pin 2
Button button2(3, 0, 50);  // Digital button on pin 3
Button joystick(A0, 0, 50); // Analog joystick on pin A0

void loop() {
    // Read all buttons
    button1.read();
    button2.read();
    joystick.readAxis();
    
    // Handle button events
    if (button1.wasPressed()) {
        Serial.println("Button 1 pressed");
    }
    
    if (button2.wasPressed()) {
        Serial.println("Button 2 pressed");
    }
    
    if (joystick.wasAxisPressed()) {
        Serial.println("Joystick pressed");
    }
}
```

### Long Press Detection

```cpp
#include "Button.h"

Button myButton(2, 0, 50);

void loop() {
    myButton.read();
    
    // Detect short press (less than 1 second)
    if (myButton.wasPressed() && !myButton.pressedFor(1000)) {
        Serial.println("Short press detected");
    }
    
    // Detect long press (more than 2 seconds)
    if (myButton.pressedFor(2000)) {
        Serial.println("Long press detected");
    }
}
```

### State Machine Example

```cpp
#include "Button.h"

Button selectButton(2, 0, 50);
Button backButton(3, 0, 50);

enum MenuState {
    MENU_MAIN,
    MENU_SETTINGS,
    MENU_PROFILE
};

MenuState currentState = MENU_MAIN;

void loop() {
    selectButton.read();
    backButton.read();
    
    switch (currentState) {
        case MENU_MAIN:
            if (selectButton.wasPressed()) {
                currentState = MENU_SETTINGS;
                Serial.println("Entering settings");
            }
            break;
            
        case MENU_SETTINGS:
            if (selectButton.wasPressed()) {
                currentState = MENU_PROFILE;
                Serial.println("Entering profile");
            }
            if (backButton.wasPressed()) {
                currentState = MENU_MAIN;
                Serial.println("Back to main");
            }
            break;
            
        case MENU_PROFILE:
            if (backButton.wasPressed()) {
                currentState = MENU_SETTINGS;
                Serial.println("Back to settings");
            }
            break;
    }
}
```

## Hardware Setup

### Digital Button Wiring

For a simple tactile button:

```
VCC (3.3V/5V) ----[10kΩ Resistor]----+
                                     |
                                     [Button]----[GND]
                                     |
Arduino Pin -------------------------+
```

### Analog Joystick Wiring

For a typical analog joystick module:

```
VCC (3.3V/5V) ----[Joystick VCC]
GND ------------[Joystick GND]
Arduino A0 -----[Joystick X-Axis]
Arduino A1 -----[Joystick Y-Axis]
```

## Configuration Tips

### Debounce Time
- **50ms**: Good for most applications
- **25ms**: For fast response applications
- **100ms**: For noisy environments or mechanical switches

### Invert Logic
- **0 (active high)**: Button connects to ground when pressed
- **1 (active low)**: Button connects to VCC when pressed

### Analog Thresholds
The library uses these default thresholds for analog inputs:
- **Full press**: > 3900 (out of 4095 for 12-bit ADC)
- **Half press**: 1500-2000
- **No press**: < 1500

## Troubleshooting

### Button Not Responding
1. Check wiring connections
2. Verify pin number in constructor
3. Check if pull-up resistor is needed
4. Verify invert logic setting

### False Triggers
1. Increase debounce time
2. Check for electrical noise
3. Add external pull-up/pull-down resistors
4. Verify button quality

### Analog Values Unstable
1. Add filtering capacitors
2. Check power supply stability
3. Verify analog reference voltage
4. Consider using averaging

## License

This library is based on the original Arduino Button Library v1.0 by Jack Christensen (Mar 2012), licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.

## Contributing

Feel free to submit issues, feature requests, or pull requests to improve this library.

## Version History

- **v1.0**: Initial release with digital button support
- **v1.1**: Added analog axis support and enhanced functionality
