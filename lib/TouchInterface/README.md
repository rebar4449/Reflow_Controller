# TouchInterface Library

A touch interface library for TFT displays with XPT2046 touchscreen controllers. This library provides easy-to-use touch button functionality with visual feedback and callback support.

## Features

- Multiple touch buttons on a single screen
- Visual feedback (pressed/unpressed states)
- Button callbacks with data support
- Touch coordinate calibration
- Button enable/disable functionality
- Automatic touch debouncing

## Dependencies

- Adafruit GFX Library
- Adafruit ILI9341 Library
- XPT2046_Touchscreen Library

## Usage

### Basic Setup

```cpp
#include "TouchInterface.h"
#include <XPT2046_Touchscreen.h>
#include <Adafruit_ILI9341.h>

// Create instances
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);
Adafruit_ILI9341 display = Adafruit_ILI9341(display_cs, display_dc, display_rst);
TouchInterface touchInterface(&ts, &display);

void setup() {
  display.begin();
  touchInterface.begin();
  
  // Add buttons
  touchInterface.addButton(10, 10, 100, 40, "Button1", ILI9341_GREEN, ILI9341_BLACK, buttonCallback);
}

void loop() {
  touchInterface.processTouch();
}

void buttonCallback() {
  Serial.println("Button pressed!");
}
```

### Button with Data

```cpp
// Add button with callback data
touchInterface.addButton(10, 10, 100, 40, "Profile1", ILI9341_BLUE, ILI9341_WHITE, nullptr, 1);
```

## API Reference

### Constructor
```cpp
TouchInterface(XPT2046_Touchscreen* touchscreen, Adafruit_ILI9341* tftDisplay, int maxButtonCount = 20)
```

### Methods

#### begin()
Initialize the touch interface.

#### addButton(x, y, width, height, label, color, textColor, callback, callbackData)
Add a button to the interface.
- `x, y`: Button position
- `width, height`: Button size
- `label`: Button text
- `color`: Background color
- `textColor`: Text color
- `callback`: Function pointer to callback (optional)
- `callbackData`: Integer data for callback (optional)

#### processTouch()
Process touch input and handle button presses.

#### drawButtons()
Draw all buttons on the display.

#### clearButtons()
Remove all buttons.

#### setButtonEnabled(index, enabled)
Enable or disable a button.

## Pin Configuration

Make sure to define the touchscreen pins in your config:

```cpp
#define XPT2046_CS  16  // Touchscreen CS pin
#define XPT2046_IRQ 4   // Touchscreen IRQ pin (optional)
```

## License

This library is released under the MIT License.
