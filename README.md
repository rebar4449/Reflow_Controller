# ESP32 Reflow Oven Controller

A sophisticated reflow oven controller built for ESP32 that converts a standard toaster oven into a precision reflow soldering system. This project provides precise temperature control for surface mount device (SMD) soldering using industry-standard reflow profiles.

## Project Overview

This reflow oven controller is designed to automate the soldering process for electronic components using temperature profiles that follow industry standards (IPC-J-STD-020D). It features a touchscreen interface, multiple reflow profiles, PID temperature control, and WiFi connectivity for remote monitoring and firmware updates.

## Hardware Components

### Core Components
- **ESP32 Development Board**: Main microcontroller (ESP32-DOIT-DevKit-V1)
- **ILI9341 TFT Display**: 2.4" 320x240 touchscreen for user interface
- **MCP9600 Thermocouple Amplifier**: High-precision temperature sensor (I2C)
- **Solid State Relay (SSR)**: Controls oven heating element
- **K-Type Thermocouple**: Temperature sensing probe

### Control Interface
- **Touchscreen Interface**: Full touch control with on-screen buttons
- **XPT2046 Touch Controller**: Integrated with ILI9341 display
- **Buzzer**: Audio feedback for alerts and completion
- **Status LED**: System status indicator
- **Cooling Fan**: Optional cooling system (pin 12)

### Storage
- **SPIFFS**: Internal flash storage for profiles and settings
- **SD Card**: Optional external storage (CS pin 22)

## Software Architecture

### Core Libraries

#### 1. **TouchInterface Library** (`lib/TouchInterface/`)
- Handles touch input from XPT2046 touchscreen
- Provides on-screen button functionality
- Supports button callbacks with data
- Visual feedback for button presses
- Touch coordinate calibration

#### 2. **UIManager Library** (`lib/UIManager/`)
- Manages multiple screens and navigation
- Handles screen transitions and state management
- Integrates with reflow logic for automatic screen changes
- Provides temperature and status display updates

#### 3. **LCD Library** (`lib/LCD/`)
- TFT display driver using TFT_eSPI library
- Menu system with multiple states
- Color-coded interface elements
- Real-time temperature and status display

#### 4. **OTA Library** (`lib/OTA/`)
- Over-the-air firmware updates
- Version checking and automatic updates
- HTTP-based update mechanism
- Callback system for update progress

#### 5. **ProfileManager Library** (`lib/ProfileManager/`)
- JSON-based reflow profile management
- Profile storage in ESP32 preferences
- Profile comparison and validation
- Support for multiple alloy types

### External Dependencies

#### Display and Graphics
- **TFT_eSPI**: TFT display driver
- **Adafruit GFX Library**: Graphics primitives
- **Adafruit ILI9341**: Display-specific driver
- **LVGL**: Advanced GUI framework (v9.2.0)

#### Communication and Storage
- **ArduinoJson**: JSON parsing and generation
- **WiFiManager**: WiFi configuration portal
- **SPIFFS**: File system for ESP32
- **SD**: SD card interface

#### Temperature Control
- **Adafruit MCP9600**: Thermocouple amplifier library
- **PID**: Proportional-Integral-Derivative control algorithm

## Reflow Process

### Temperature Profile Stages

The reflow process follows a four-stage temperature curve:

1. **Preheat Stage** (25°C → 150°C)
   - Gradual temperature ramp-up
   - Prevents thermal shock to components
   - Duration: 60-90 seconds

2. **Soak Stage** (150°C → 180°C)
   - Temperature stabilization
   - Activates flux and removes moisture
   - Duration: 90-120 seconds

3. **Reflow Stage** (180°C → 217°C)
   - Peak temperature for solder melting
   - Critical for proper solder joint formation
   - Duration: 90-120 seconds

4. **Cooling Stage** (217°C → 150°C)
   - Controlled cooling
   - Prevents thermal stress
   - Duration: Variable

### PID Control System

The system uses PID (Proportional-Integral-Derivative) control for precise temperature management:

- **Preheat PID**: Kp=100, Ki=0.025, Kd=20
- **Soak PID**: Kp=300, Ki=0.05, Kd=250
- **Reflow PID**: Kp=300, Ki=0.05, Kd=350

### State Machine

The reflow process operates through a state machine:

- `REFLOW_STATE_IDLE`: System ready
- `REFLOW_STATE_PREHEAT`: Heating to soak temperature
- `REFLOW_STATE_SOAK`: Temperature stabilization
- `REFLOW_STATE_REFLOW`: Peak temperature phase
- `REFLOW_STATE_COOL`: Controlled cooling
- `REFLOW_STATE_COMPLETE`: Process finished
- `REFLOW_STATE_TOO_HOT`: Safety check
- `REFLOW_STATE_ERROR`: Fault condition

## User Interface

### Touch Interface System

The interface provides a modern touch-based user experience:

1. **Main Screen**
   - Large temperature display
   - Start button to begin reflow process
   - Settings button for configuration
   - Info button for system information

2. **Profile Selection Screen**
   - Touch buttons for each available profile
   - Profile name and alloy type display
   - Back button to return to main screen

3. **Settings Screen**
   - Toggle buttons for each setting
   - Fan, buzzer, OTA, and other options
   - Back button to return to main screen

4. **Reflow Running Screen**
   - Real-time temperature and setpoint display
   - Large stop button for emergency stop
   - Status information and progress

5. **Info Screen**
   - System information and firmware version
   - Current profile details
   - Temperature and status information

### Real-time Display

The main screen shows:
- Current temperature
- Target temperature
- Reflow stage
- Time elapsed
- System status
- Error conditions

## Profile Management

### JSON Profile Format

Profiles are stored in JSON format with the following structure:

```json
{
  "title": "Lead-Free SAC305",
  "alloy": "SAC305",
  "melting_point": 217,
  "temp_range": [25, 250],
  "time_range": [0, 300],
  "reference": "IPC-J-STD-020D",
  "stages": {
    "preheat": [25, 150],
    "soak": [150, 180],
    "reflow": [180, 217],
    "cool": [217, 150]
  }
}
```

### Supported Alloy Types

- **Lead-Free SAC305**: Sn96.5/Ag3.0/Cu0.5
- **Lead-Free SAC387**: Sn95.5/Ag3.8/Cu0.7
- **Lead Solder**: Sn63/Pb37
- **Custom Alloys**: User-defined profiles

## Safety Features

### Temperature Monitoring
- Real-time temperature validation
- Out-of-range detection (-200°C to 1000°C)
- Thermocouple fault detection

### System Protection
- SSR control to prevent overheating
- Emergency stop functionality
- Temperature too hot detection
- Automatic shutdown on errors

### User Safety
- Visual and audio alerts
- Status LED indicators
- Clear error messages
- Manual override capabilities

## WiFi and Connectivity

### WiFi Manager
- Automatic WiFi configuration
- Access point mode for setup
- Persistent connection management
- IP address display

### OTA Updates
- Automatic version checking
- Secure firmware updates
- Update progress indication
- Rollback capability

## Configuration

### Pin Assignments

```cpp
// Display
#define display_cs   5
#define display_dc   21
#define display_mosi 23
#define display_sclk 18
#define display_rst  -1

// Control
#define ssrPin       14
#define ledPin       2
#define buzzerPin    15
#define fanPin       12

// Touchscreen
#define XPT2046_CS  16  // Touchscreen CS pin
#define XPT2046_IRQ 4   // Touchscreen IRQ pin (optional)

// Storage
#define SD_CS_pin       22
```

### Build Configuration

The project uses PlatformIO with the following configuration:

```ini
[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino
monitor_speed = 115200
```

## Usage Instructions

### Initial Setup

1. **Hardware Assembly**
   - Connect ESP32 to display and sensors
   - Install thermocouple in oven
   - Connect SSR to heating element
   - Mount control panel

2. **Software Installation**
   - Install PlatformIO
   - Clone repository
   - Install dependencies
   - Build and upload firmware

3. **First Run**
   - Power on system
   - Configure WiFi (if needed)
   - Load default profiles
   - Test temperature sensor

### Operation

1. **Select Profile**
   - Navigate to profile selection
   - Choose appropriate alloy type
   - Verify temperature ranges

2. **Start Reflow**
   - Ensure oven is cool (<50°C)
   - Place PCB in oven
   - Start reflow process
   - Monitor progress

3. **Completion**
   - Wait for cooling phase
   - Remove PCB when safe
   - Check solder joints
   - Clean up

## Troubleshooting

### Common Issues

- **Temperature Reading Errors**: Check thermocouple connections
- **Display Issues**: Verify SPI connections and power
- **Touch Response**: Check touchscreen calibration and connections
- **WiFi Problems**: Use WiFiManager to reconfigure

### Error Codes

- `TC Error`: Thermocouple fault
- `Too Hot`: Oven temperature too high to start
- `Fault`: General system error

## Development

### Adding New Features

1. **New Profiles**: Add JSON files to `/profiles/`
2. **UI Changes**: Modify LCD library
3. **Hardware Support**: Update pin definitions in `config.h`
4. **New Sensors**: Add sensor libraries and update main loop

### Testing

- Use test mode for output verification
- Monitor serial output for debugging
- Validate temperature accuracy
- Test safety features

## License

This project is released under the Creative Commons Share Alike v3.0 license. See individual library licenses for specific components.

## Contributing

Contributions are welcome! Please ensure:
- Code follows existing style
- New features are tested
- Documentation is updated
- Safety considerations are addressed

## Version History

- **v0.3.0.0**: Current version with full feature set
- **v0.2.0.0**: Added profile management
- **v0.1.0.0**: Initial release with basic functionality

## Support

For issues and questions:
- Check troubleshooting section
- Review serial monitor output
- Verify hardware connections
- Consult component datasheets

---

**Warning**: This project involves high voltage and high temperatures. Ensure proper safety precautions and electrical knowledge before use. The authors are not liable for any damage or injury resulting from the use of this project.
