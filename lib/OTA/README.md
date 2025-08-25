# OTA Library

A comprehensive ESP32 Over-The-Air firmware update library that provides easy-to-use functionality for checking and performing OTA firmware updates.

## Features

- **Version Checking**: Automatically checks for new firmware versions on a remote server
- **Automatic Update Detection**: Compares current firmware version with server version
- **Customizable Callbacks**: Set custom callback functions for update progress feedback
- **Error Handling**: Comprehensive error handling with detailed error messages
- **ESP32 Optimized**: Specifically designed for ESP32 platform
- **Easy Integration**: Simple API for quick integration into existing projects

## Installation

### Arduino IDE
1. Download this library as a ZIP file
2. In Arduino IDE, go to `Sketch > Include Library > Add .ZIP Library`
3. Select the downloaded ZIP file
4. The library will be installed and ready to use

### PlatformIO
Add the following to your `platformio.ini`:
```ini
lib_deps = 
    OTA
```

## Quick Start

```cpp
#include <WiFi.h>
#include <OTA.h>

// WiFi credentials
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

// OTA configuration
const String versionUrl = "http://your-server.com/version.txt";
const String firmwareUrl = "http://your-server.com/firmware.bin";
const String currentVersion = "1.0.0";

// Create OTA instance
OTA ota(versionUrl, firmwareUrl, currentVersion);

void setup() {
    Serial.begin(115200);
    
    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    
    // Check for updates
    ota.checkForUpdate();
    
    if (ota.isUpdateAvailable()) {
        Serial.println("Update available! Performing update...");
        ota.performUpdate();
    } else {
        Serial.println("No update available. Continuing...");
    }
}

void loop() {
    // Your main code here
}
```

## Examples

### Basic Usage
See the `examples/BasicUsage` folder for a simple implementation.

### With Custom Callbacks
See the `examples/CustomCallbacks` folder for an example with custom UI feedback.

### Automatic Update Check
See the `examples/AutoUpdate` folder for automatic update checking on boot.

## API Reference

### Constructor
```cpp
OTA(const String& versionUrl, const String& firmwareUrl, const String& currentVersion)
```
- `versionUrl`: URL to a text file containing the latest version number
- `firmwareUrl`: URL to the firmware binary file
- `currentVersion`: Current firmware version string

### Main Methods

#### `void checkForUpdate()`
Checks for available updates by comparing current version with server version.

#### `void performUpdate()`
Performs the firmware update if one is available.

#### `bool isUpdateAvailable()`
Returns `true` if an update is available, `false` otherwise.

### Configuration Methods

#### `void setVersionUrl(const String& url)`
Sets the URL for version checking.

#### `void setFirmwareUrl(const String& url)`
Sets the URL for firmware download.

#### `void setCurrentVersion(const String& version)`
Sets the current firmware version.

### Callback Methods

#### `void setUpdateStartCallback(void (*callback)())`
Sets a callback function that is called when the update starts.

#### `void setUpdateSuccessCallback(void (*callback)())`
Sets a callback function that is called when the update completes successfully.

#### `void setUpdateErrorCallback(void (*callback)(const String& error))`
Sets a callback function that is called when an error occurs during update.

## Server Requirements

### Version File
Create a simple text file (e.g., `version.txt`) containing only the version number:
```
1.1.0
```

### Firmware Binary
Upload your compiled firmware binary to your server. The file should be accessible via HTTP.

## Version Format

The library expects version numbers in a simple format (e.g., "1.0.0", "2.1.5"). The comparison is done by extracting the first number before the first dot.

## Error Handling

The library provides comprehensive error handling for:
- Network connection issues
- HTTP request failures
- Insufficient storage space
- Incomplete downloads
- Update verification failures

All errors are reported through the error callback function and Serial output.

## Dependencies

- WiFi (ESP32 core)
- HTTPClient (ESP32 core)
- Update (ESP32 core)

## License

This library is licensed under the MIT License. See the LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Support

If you encounter any issues or have questions, please open an issue on the GitHub repository.
