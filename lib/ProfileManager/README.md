# ProfileManager Library

A comprehensive ESP32 library for managing reflow oven profiles with JSON parsing, persistent storage, profile comparison capabilities, and MCP9600 thermocouple amplifier support.

## Overview

The ProfileManager library provides a complete solution for handling reflow oven temperature profiles on ESP32-based controllers. It supports:

- **JSON Profile Parsing**: Load profiles from JSON files stored in SPIFFS
- **Persistent Storage**: Save and load profiles using ESP32 Preferences
- **Profile Management**: Compare, select, and manage multiple profiles
- **MCP9600 Thermocouple Support**: Integrated temperature reading with fault detection
- **Memory Efficient**: Optimized for embedded systems with limited memory

## Features

- ✅ JSON profile parsing from SPIFFS files
- ✅ Persistent storage using ESP32 Preferences
- ✅ Profile comparison and validation
- ✅ Multiple profile support
- ✅ MCP9600 thermocouple amplifier integration
- ✅ Comprehensive fault detection and reporting
- ✅ Memory-efficient data structures
- ✅ Comprehensive error handling
- ✅ Debug output and logging
- ✅ Easy-to-use C++ class interface

## Installation

### Using Arduino IDE Library Manager
1. Open Arduino IDE
2. Go to **Tools** → **Manage Libraries**
3. Search for "ProfileManager"
4. Click **Install**

### Manual Installation
1. Download the library from GitHub
2. Extract to your Arduino libraries folder
3. Restart Arduino IDE

## Dependencies

- **ArduinoJson** (v6.x) - For JSON parsing and serialization
- **Preferences** - For persistent storage (included with ESP32 core)
- **Adafruit MCP9600 Library** - For MCP9600 thermocouple amplifier support

## Quick Start

### Basic Profile Management
```cpp
#include <SPIFFS.h>
#include <ProfileManager.h>

// Create ProfileManager instance
ProfileManager profileManager;

// Define profile array
#define NUM_PROFILES 5
profile_t profiles[NUM_PROFILES];

void setup() {
  Serial.begin(115200);
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  
  // Parse JSON profile from file
  profileManager.parseJsonProfile(SPIFFS, "/profile1.json", 0, profiles);
  
  // Save profile to preferences
  profileManager.saveProfiles(0, profiles[0]);
  
  // Load profile from preferences
  profileManager.loadProfiles(0, profiles);
}
```

### MCP9600 Temperature Reading
```cpp
#include <Wire.h>
#include <ProfileManager.h>

// Create MCP9600Manager instance
MCP9600Manager mcp9600(0x67); // Default I2C address

void setup() {
  Serial.begin(115200);
  
  // Initialize I2C
  Wire.begin();
  
  // Initialize MCP9600
  if (!mcp9600.begin()) {
    Serial.println("Failed to initialize MCP9600!");
    return;
  }
  
  // Configure sensor
  mcp9600.setThermocoupleType(MCP9600_TYPE_K);
  mcp9600.setFilterCoefficient(3);
  mcp9600.setADCResolution(MCP9600_ADC_RESOLUTION_18);
}

void loop() {
  // Read temperature
  float temp = mcp9600.readThermocoupleTemperature();
  
  if (temp != -999.0) {
    Serial.print("Temperature: ");
    Serial.print(temp, 2);
    Serial.println("°C");
  } else {
    Serial.println("Temperature reading error");
  }
  
  delay(1000);
}
```

## Profile Structure

The library uses a `profile_t` structure to store reflow oven profile data:

```cpp
typedef struct {
  char title[32];              // Profile title (e.g., "Lead 183")
  char alloy[32];              // Alloy type (e.g., "Sn63/Pb37")
  uint16_t melting_point;      // Melting point temperature
  uint16_t temp_range_0;       // Temperature range minimum
  uint16_t temp_range_1;       // Temperature range maximum
  uint16_t time_range_0;       // Time range minimum
  uint16_t time_range_1;       // Time range maximum
  char reference[128];         // Reference URL or documentation
  uint16_t stages_preheat_0;   // Preheat stage start temperature
  uint16_t stages_preheat_1;   // Preheat stage end temperature
  uint16_t stages_soak_0;      // Soak stage start temperature
  uint16_t stages_soak_1;      // Soak stage end temperature
  uint16_t stages_reflow_0;    // Reflow stage start temperature
  uint16_t stages_reflow_1;    // Reflow stage end temperature
  uint16_t stages_cool_0;      // Cool stage start temperature
  uint16_t stages_cool_1;      // Cool stage end temperature
} profile_t;
```

## JSON Profile Format

Profiles should be stored as JSON files with the following structure:

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

## API Reference

### ProfileManager Class

#### Constructor
```cpp
ProfileManager();
```

#### JSON Parsing
```cpp
void parseJsonProfile(fs::FS &fs, String fileName, int profileIndex, profile_t* profile);
```

#### Profile Storage
```cpp
void saveProfiles(int profileIndex, profile_t profile);
void loadProfiles(int profileIndex, profile_t* profile);
```

#### Profile Selection
```cpp
void saveSelectedProfile(int profileIndex);
int getSelectedProfile();
```

#### Profile Comparison
```cpp
void compareProfiles(profile_t profile_new, profile_t profile_saved, int profileIndex);
```

#### Utility Functions
```cpp
void printProfileInfo(profile_t profile, int profileIndex);
```

### MCP9600Manager Class

#### Constructor
```cpp
MCP9600Manager(uint8_t address = 0x67);
```

#### Initialization
```cpp
bool begin();
bool begin(TwoWire *theWire);
```

#### Temperature Reading
```cpp
float readThermocoupleTemperature();
float readAmbientTemperature();
```

#### Configuration
```cpp
bool setThermocoupleType(mcp9600_thermocoupletype_t type);
bool setFilterCoefficient(uint8_t coefficient);
bool setADCResolution(mcp9600_adc_resolution_t resolution);
bool setAlertTemperature(uint8_t alert_num, float temperature);
bool setAlertHysteresis(uint8_t alert_num, float hysteresis);
bool setAlertMode(uint8_t alert_num, mcp9600_alert_mode_t mode);
```

#### Status and Fault Checking
```cpp
bool checkFault();
uint8_t getFaultStatus();
String getFaultDescription(uint8_t fault);
bool isConnected();
```

#### Utility Functions
```cpp
void printSensorInfo();
void reset();
```

## Hardware Setup

### MCP9600 Wiring
- **VDD** → 3.3V
- **GND** → GND
- **SDA** → GPIO 21 (default ESP32 SDA)
- **SCL** → GPIO 22 (default ESP32 SCL)
- **Thermocouple** → Connect K-type thermocouple to thermocouple inputs

### I2C Address Configuration
The MCP9600 supports multiple I2C addresses:
- **0x67** (default) - ADDR pin floating or pulled high
- **0x60** - ADDR pin pulled low

## Examples

### Basic Usage
See `examples/BasicUsage/BasicUsage.ino` for basic profile management.

### Profile Storage
See `examples/ProfileStorage/ProfileStorage.ino` for advanced profile management.

### MCP9600 Usage
See `examples/MCP9600Usage/MCP9600Usage.ino` for temperature reading with MCP9600.

## Error Handling

The library includes comprehensive error handling:

- **File System Errors**: Checks for SPIFFS mount failures
- **JSON Parsing Errors**: Validates JSON structure and reports parsing errors
- **Memory Errors**: Checks data size consistency
- **Storage Errors**: Validates preference storage operations
- **MCP9600 Faults**: Comprehensive fault detection and reporting

## Memory Usage

- **Profile Structure**: ~256 bytes per profile
- **JSON Buffer**: ~1.3KB for parsing
- **Preferences Storage**: ~256 bytes per profile in NVS
- **MCP9600 Manager**: ~50 bytes overhead

## Performance Considerations

- **JSON Parsing**: Optimized for embedded systems with limited memory
- **Storage**: Uses efficient binary storage in ESP32 Preferences
- **I2C Communication**: Efficient I2C communication with MCP9600
- **Memory**: Minimal dynamic memory allocation

## Troubleshooting

### Common Issues

1. **SPIFFS Mount Failed**
   - Ensure SPIFFS is properly initialized
   - Check if files exist in SPIFFS

2. **JSON Parsing Errors**
   - Verify JSON file format
   - Check file size (should be < 1.3KB)
   - Ensure all required fields are present

3. **Storage Errors**
   - Check available NVS space
   - Verify profile structure size

4. **MCP9600 Connection Issues**
   - Check I2C wiring (SDA, SCL)
   - Verify I2C address (default: 0x67)
   - Check power supply (3.3V)
   - Ensure thermocouple is properly connected

5. **Temperature Reading Errors**
   - Check thermocouple connections
   - Verify thermocouple type (K-type recommended)
   - Check for fault conditions using `checkFault()`

### Debug Output

Enable Serial output to see detailed debug information:

```cpp
Serial.begin(115200);
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## License

This library is released under the MIT License. See LICENSE file for details.

## Support

For support and questions:
- Create an issue on GitHub
- Check the examples for usage patterns
- Review the API documentation

## Version History

- **v1.1.0** - Added MCP9600 thermocouple support
  - MCP9600Manager class for temperature reading
  - Comprehensive fault detection and reporting
  - I2C communication support
  - New MCP9600Usage example

- **v1.0.0** - Initial release with basic functionality
  - JSON profile parsing
  - Persistent storage
  - Profile comparison
  - Basic examples

## Acknowledgments

- Based on ESP32 Reflow Oven Controller project
- Uses ArduinoJson library for JSON handling
- Leverages ESP32 Preferences for persistent storage
- Integrates Adafruit MCP9600 Library for thermocouple support
