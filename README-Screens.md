# Reflow Controller UI Screen System

This document describes the UI screen design and loading system for the Reflow Controller's LCD touchscreen interface.

## Overview

The UI system is built around a modular architecture with separate components for screen design, touch handling, and state management. The system supports multiple screens with smooth transitions and real-time updates.

## Architecture Components

### 1. UIManager (`lib/UIManager/`)
**Primary Location**: `lib/UIManager/UIManager.cpp`

The UIManager is the central coordinator for all UI operations. It manages screen states, handles transitions, and coordinates between the display and touch interface.

#### Key Functions:
- **`drawMainScreen()`** - Creates the main controller screen
- **`drawProfileSelectScreen()`** - Creates the profile selection screen
- **`drawSettingsScreen()`** - Creates the settings configuration screen
- **`drawReflowRunningScreen()`** - Creates the active reflow monitoring screen
- **`drawInfoScreen()`** - Creates the system information display screen
- **`switchToScreen()`** - Manages screen transitions
- **`drawCurrentScreen()`** - Determines which screen to draw based on current state

### 2. TouchInterface (`lib/TouchInterface/`)
**Primary Location**: `lib/TouchInterface/TouchInterface.cpp`

Handles all touch input processing and button rendering. Provides a unified interface for creating and managing touch-sensitive buttons.

#### Key Functions:
- **`addButton()`** - Creates button definitions with position, size, colors, and callbacks
- **`drawButtons()`** - Draws all buttons on the current screen
- **`drawButton()`** - Renders individual buttons with proper styling and text centering
- **`processTouch()`** - Handles touch input and button press detection

### 3. LCD Library (`lib/LCD/`)
**Primary Location**: `lib/LCD/LCD.h` and `lib/LCD/LCD.cpp`

Provides utility functions for text rendering and display management. Supports various text alignment options and color schemes.

#### Key Functions:
- **`centeredText()`** - Renders centered text
- **`leftText()`** - Renders left-aligned text
- **`rightText()`** - Renders right-aligned text

## Screen States

The system uses an enum `ScreenState` defined in `UIManager.h`:

```cpp
enum ScreenState {
  SCREEN_MAIN,           // Main controller screen
  SCREEN_PROFILE_SELECT, // Profile selection screen
  SCREEN_SETTINGS,       // Settings configuration screen
  SCREEN_REFLOW_RUNNING, // Active reflow monitoring screen
  SCREEN_INFO           // System information screen
};
```

## Screen Designs

### 1. Main Screen (`SCREEN_MAIN`)
**Function**: `drawMainScreen()` in `UIManager.cpp`

**Layout**:
- Header: "Reflow Controller"
- Large temperature display (yellow text, size 4)
- Status bar showing current system status
- Three action buttons:
  - **Start** (green) - Initiates profile selection
  - **Settings** (blue) - Opens settings screen
  - **Info** (magenta) - Shows system information

**Features**:
- Real-time temperature updates
- Dynamic status display
- Touch-sensitive buttons with visual feedback

### 2. Profile Selection Screen (`SCREEN_PROFILE_SELECT`)
**Function**: `drawProfileSelectScreen()` in `UIManager.cpp`

**Layout**:
- Header: "Select Profile"
- Grid of profile buttons (up to 10 profiles)
- Each button shows: "1: Profile Name"
- Back button (red) to return to main screen

**Features**:
- Scrollable profile list
- Profile information display
- Automatic profile loading when selected

### 3. Settings Screen (`SCREEN_SETTINGS`)
**Function**: `drawSettingsScreen()` in `UIManager.cpp`

**Layout**:
- Header: "Settings"
- Toggle buttons for various settings:
  - Fan control
  - Buzzer control
  - OTA updates
- Back button (red) to return to main screen

**Features**:
- Toggle functionality for each setting
- Persistent settings storage
- Visual feedback for current state

### 4. Reflow Running Screen (`SCREEN_REFLOW_RUNNING`)
**Function**: `drawReflowRunningScreen()` in `UIManager.cpp`

**Layout**:
- Header: "Reflow Running"
- Large temperature display
- Setpoint temperature display (green)
- Status bar showing current reflow stage
- Stop button (red) to abort reflow process

**Features**:
- Real-time temperature monitoring
- Setpoint display
- Stage progression tracking
- Emergency stop functionality

### 5. Info Screen (`SCREEN_INFO`)
**Function**: `drawInfoScreen()` in `UIManager.cpp`

**Layout**:
- Header: "System Info"
- System information display:
  - Firmware version
  - Current profile
  - Alloy type
  - Current temperature
- Back button (red) to return to main screen

**Features**:
- System status overview
- Profile information
- Temperature monitoring

## Screen Loading Process

### Initialization
**Location**: `src/main.cpp` lines 158-165

```cpp
// Initialize touch interface and UI manager
touchInterface = new TouchInterface(&ts, &display);
touchInterface->begin();

uiManager = new UIManager(touchInterface, &display);
uiManager->begin();

// Update LCD data with current state
uiManager->setLCDData();
```

### Screen Transitions
**Location**: `UIManager.cpp` lines 75-85

```cpp
void UIManager::switchToScreen(ScreenState screen) {
  if (screen == currentScreen) return;
  
  previousScreen = currentScreen;
  currentScreen = screen;
  
  // Clear all buttons
  touchInterface->clearButtons();
  
  // Draw new screen
  drawCurrentScreen();
}
```

### Main Loop Updates
**Location**: `src/main.cpp` lines 350-358

```cpp
// Update UI with current temperature and status
if (uiManager) {
  uiManager->updateTemperature(input);
  uiManager->updateStatus(activeStatus);
  uiManager->setLCDData();  // Keep LCD data in sync
}
```

## Button System

### Button Structure
**Location**: `TouchInterface.h` lines 18-28

```cpp
struct TouchButton {
  int x, y, width, height;
  String label;
  uint16_t color;
  uint16_t textColor;
  bool pressed;
  bool enabled;
  void (*callback)();
  int callbackData; // Additional data for callback
};
```

### Button Creation
**Location**: `TouchInterface.cpp` lines 35-55

```cpp
int TouchInterface::addButton(int x, int y, int width, int height, 
                             String label, uint16_t color, uint16_t textColor, 
                             void (*callback)() = nullptr, int callbackData = 0)
```

### Button Rendering
**Location**: `TouchInterface.cpp` lines 95-125

Features:
- Rounded rectangle backgrounds
- Centered text rendering
- Pressed state visual feedback
- Disabled state handling
- Color-coded button types

## Color Scheme

The UI uses a consistent color scheme defined in `LCD.h`:

- **ILI9341_BLACK** (0x0000) - Background
- **ILI9341_WHITE** (0xFFFF) - Default text
- **ILI9341_YELLOW** (0xFFE0) - Temperature display
- **ILI9341_GREEN** (0x07E0) - Start buttons, setpoint
- **ILI9341_RED** (0xF800) - Stop buttons, back buttons
- **ILI9341_BLUE** (0x001F) - Settings buttons
- **ILI9341_CYAN** (0x07FF) - Status text
- **ILI9341_MAGENTA** (0xF81F) - Info buttons

## Touch Handling

### Touch Processing
**Location**: `TouchInterface.cpp` lines 127-175

The touch system:
1. Reads touch coordinates from XPT2046 touchscreen
2. Converts touch coordinates to display coordinates
3. Detects button presses and releases
4. Provides visual feedback for button states
5. Executes button callbacks

### Coordinate Conversion
**Location**: `TouchInterface.cpp` lines 200-207

```cpp
void TouchInterface::convertTouchToDisplay(int touchX, int touchY, int& displayX, int& displayY) {
  displayX = map(touchX, touchCalibrationX1, touchCalibrationX2, displayCalibrationX1, displayCalibrationX2);
  displayY = map(touchY, touchCalibrationY1, touchCalibrationY2, displayCalibrationY1, displayCalibrationY2);
  
  displayX = constrain(displayX, 0, 319);
  displayY = constrain(displayY, 0, 239);
}
```

## Adding New Screens

To add a new screen:

1. **Add screen state** to `ScreenState` enum in `UIManager.h`
2. **Create drawing function** in `UIManager.cpp` (e.g., `drawNewScreen()`)
3. **Add case** in `drawCurrentScreen()` switch statement
4. **Create callback functions** for screen-specific actions
5. **Add navigation** to the new screen from existing screens

## File Structure

```
lib/
├── UIManager/
│   ├── UIManager.h      # Screen state definitions and class interface
│   └── UIManager.cpp    # Screen drawing and management logic
├── TouchInterface/
│   ├── TouchInterface.h # Touch button structure and interface
│   └── TouchInterface.cpp # Touch processing and button rendering
└── LCD/
    ├── LCD.h           # Text rendering utilities
    └── LCD.cpp         # Text rendering implementation

src/
└── main.cpp           # Main application integration
```

## Dependencies

- **Adafruit_ILI9341** - Display driver
- **XPT2046_Touchscreen** - Touch input handling
- **Adafruit_GFX** - Graphics primitives
- **ArduinoJson** - JSON parsing for profiles

## Performance Considerations

- Screen transitions clear all buttons before drawing new ones
- Temperature updates only occur on relevant screens
- Touch processing includes debouncing to prevent false triggers
- Button rendering is optimized for the ILI9341 display capabilities

## Future Enhancements

Potential improvements to the UI system:
- Animated screen transitions
- Custom fonts and icons
- More sophisticated touch gestures
- Screen caching for faster transitions
- Dynamic layout adaptation for different screen sizes
