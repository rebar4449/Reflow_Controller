# Migration Guide: MAX31856 to MCP9600

This guide helps you migrate from the MAX31856 thermocouple amplifier to the MCP9600 thermocouple amplifier in your ESP32 reflow oven controller project.

## Overview

The MCP9600 is an I2C-based thermocouple amplifier that offers several advantages over the SPI-based MAX31856:
- **I2C Communication**: Simpler wiring (only 2 data lines vs 4 for SPI)
- **Higher Resolution**: 18-bit ADC vs 16-bit
- **Better Accuracy**: ±0.5°C typical accuracy vs ±2°C
- **More Features**: Built-in alerts, multiple thermocouple types, filtering
- **Lower Power**: More efficient operation

## Hardware Changes

### MAX31856 (SPI) Wiring
```
MAX31856 Pin → ESP32 Pin
CS    → GPIO 13
DI    → GPIO 23 (MOSI)
DO    → GPIO 19 (MISO)
CLK   → GPIO 18 (SCK)
VCC   → 3.3V
GND   → GND
```

### MCP9600 (I2C) Wiring
```
MCP9600 Pin → ESP32 Pin
SDA   → GPIO 21 (SDA)
SCL   → GPIO 22 (SCL)
VDD   → 3.3V
GND   → GND
```

### Pin Changes Summary
| Function | MAX31856 | MCP9600 | Change |
|----------|----------|---------|---------|
| Data Line 1 | GPIO 13 (CS) | GPIO 21 (SDA) | ✅ Freed up GPIO 13 |
| Data Line 2 | GPIO 23 (DI) | GPIO 22 (SCL) | ✅ Freed up GPIO 23 |
| Data Line 3 | GPIO 19 (DO) | - | ✅ Freed up GPIO 19 |
| Data Line 4 | GPIO 18 (CLK) | - | ✅ Freed up GPIO 18 |

## Software Changes

### 1. Update Include Statements

**Before (MAX31856):**
```cpp
#include <Adafruit_MAX31856.h>
```

**After (MCP9600):**
```cpp
#include <Wire.h>
#include <ProfileManager.h> // Includes MCP9600 support
```

### 2. Update Object Declaration

**Before (MAX31856):**
```cpp
Adafruit_MAX31856 max31856 = Adafruit_MAX31856(max_cs);
```

**After (MCP9600):**
```cpp
MCP9600Manager mcp9600(0x67); // Default I2C address
```

### 3. Update Initialization

**Before (MAX31856):**
```cpp
max31856.begin();
max31856.setThermocoupleType(MAX31856_TCTYPE_K);
```

**After (MCP9600):**
```cpp
Wire.begin();
if (!mcp9600.begin()) {
  Serial.println("Failed to initialize MCP9600!");
  return;
}
mcp9600.setThermocoupleType(MCP9600_TYPE_K);
```

### 4. Update Temperature Reading

**Before (MAX31856):**
```cpp
float input = max31856.readThermocoupleTemperature();
uint8_t fault = max31856.readFault();
if (fault) {
  if (fault & MAX31856_FAULT_CJRANGE) // Handle fault
  if (fault & MAX31856_FAULT_TCRANGE) // Handle fault
  // ... more fault checks
}
```

**After (MCP9600):**
```cpp
float input = mcp9600.readThermocoupleTemperature();
if (mcp9600.checkFault()) {
  uint8_t fault = mcp9600.getFaultStatus();
  String faultDesc = mcp9600.getFaultDescription(fault);
  Serial.println("Fault: " + faultDesc);
}
```

### 5. Update Fault Checking

**Before (MAX31856):**
```cpp
if (input == MAX31856_FAULT_CJRANGE) {
  reflowState = REFLOW_STATE_ERROR;
  reflowStatus = REFLOW_STATUS_OFF;
}
```

**After (MCP9600):**
```cpp
if (input == -999.0 || mcp9600.checkFault()) {
  reflowState = REFLOW_STATE_ERROR;
  reflowStatus = REFLOW_STATUS_OFF;
}
```

## Complete Code Example

### Before (MAX31856)
```cpp
#include <Adafruit_MAX31856.h>

// Pin definitions
#define max_cs  13

// Object declaration
Adafruit_MAX31856 max31856 = Adafruit_MAX31856(max_cs);

void setup() {
  Serial.begin(115200);
  
  // Initialize MAX31856
  max31856.begin();
  max31856.setThermocoupleType(MAX31856_TCTYPE_K);
}

void loop() {
  // Read temperature
  float temp = max31856.readThermocoupleTemperature();
  
  // Check faults
  uint8_t fault = max31856.readFault();
  if (fault) {
    Serial.println("Thermocouple fault detected");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println("°C");
  }
  
  delay(1000);
}
```

### After (MCP9600)
```cpp
#include <Wire.h>
#include <ProfileManager.h>

// Object declaration
MCP9600Manager mcp9600(0x67);

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
  
  // Check faults
  if (mcp9600.checkFault()) {
    uint8_t fault = mcp9600.getFaultStatus();
    String faultDesc = mcp9600.getFaultDescription(fault);
    Serial.println("Fault: " + faultDesc);
  } else {
    Serial.print("Temperature: ");
    Serial.print(temp, 2);
    Serial.println("°C");
  }
  
  delay(1000);
}
```

## Configuration Options

The MCP9600 offers more configuration options than the MAX31856:

### Thermocouple Types
```cpp
mcp9600.setThermocoupleType(MCP9600_TYPE_K);  // K-type (default)
mcp9600.setThermocoupleType(MCP9600_TYPE_J);  // J-type
mcp9600.setThermocoupleType(MCP9600_TYPE_T);  // T-type
mcp9600.setThermocoupleType(MCP9600_TYPE_N);  // N-type
mcp9600.setThermocoupleType(MCP9600_TYPE_S);  // S-type
mcp9600.setThermocoupleType(MCP9600_TYPE_E);  // E-type
mcp9600.setThermocoupleType(MCP9600_TYPE_B);  // B-type
mcp9600.setThermocoupleType(MCP9600_TYPE_R);  // R-type
```

### Filter Coefficient
```cpp
mcp9600.setFilterCoefficient(0); // No filtering
mcp9600.setFilterCoefficient(1); // Light filtering
mcp9600.setFilterCoefficient(2); // Medium filtering
mcp9600.setFilterCoefficient(3); // Heavy filtering (default)
mcp9600.setFilterCoefficient(4); // Very heavy filtering
mcp9600.setFilterCoefficient(5); // Maximum filtering
mcp9600.setFilterCoefficient(6); // Maximum filtering
mcp9600.setFilterCoefficient(7); // Maximum filtering
```

### ADC Resolution
```cpp
mcp9600.setADCResolution(MCP9600_ADC_RESOLUTION_18); // 18-bit (default)
mcp9600.setADCResolution(MCP9600_ADC_RESOLUTION_16); // 16-bit
mcp9600.setADCResolution(MCP9600_ADC_RESOLUTION_14); // 14-bit
mcp9600.setADCResolution(MCP9600_ADC_RESOLUTION_12); // 12-bit
```

### Alert Configuration
```cpp
// Set alert temperature
mcp9600.setAlertTemperature(1, 200.0); // Alert at 200°C

// Set hysteresis
mcp9600.setAlertHysteresis(1, 5.0); // 5°C hysteresis

// Set alert mode
mcp9600.setAlertMode(1, MCP9600_ALERT_HIGH);  // Alert when above threshold
mcp9600.setAlertMode(1, MCP9600_ALERT_LOW);   // Alert when below threshold
mcp9600.setAlertMode(1, MCP9600_ALERT_CRIT);  // Critical temperature alert
```

## Benefits of Migration

1. **Simplified Wiring**: Only 2 data lines vs 4
2. **Freed GPIO Pins**: 4 additional GPIO pins available
3. **Better Accuracy**: ±0.5°C vs ±2°C
4. **Higher Resolution**: 18-bit vs 16-bit ADC
5. **Built-in Features**: Alerts, filtering, multiple thermocouple types
6. **Lower Power**: More efficient operation
7. **Better Fault Detection**: More comprehensive fault reporting

## Troubleshooting

### Common Issues

1. **I2C Address Conflicts**
   - Check I2C address with I2C scanner
   - Default address is 0x67
   - Alternative address is 0x60 (ADDR pin low)

2. **No Temperature Reading**
   - Check I2C wiring (SDA, SCL)
   - Verify power supply (3.3V)
   - Check thermocouple connections

3. **Incorrect Temperature Values**
   - Verify thermocouple type setting
   - Check for fault conditions
   - Ensure proper thermocouple wiring

4. **I2C Communication Errors**
   - Check pull-up resistors on SDA/SCL
   - Verify I2C bus speed
   - Check for other I2C devices on same bus

### I2C Scanner Code
```cpp
#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  Serial.println("I2C Scanner");
  Serial.println("Scanning...");
  
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  
  Serial.println("Scan complete.");
}

void loop() {
  // Nothing to do
}
```

## Support

For additional support with the migration:
- Check the MCP9600Usage example
- Review the API documentation
- Create an issue on GitHub
- Consult the MCP9600 datasheet
