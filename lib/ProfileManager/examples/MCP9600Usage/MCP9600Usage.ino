/*
  MCP9600Usage example for ProfileManager library
  
  This example demonstrates how to use the MCP9600Manager class
  for temperature reading with the MCP9600 thermocouple amplifier.
  
  Hardware Requirements:
  - ESP32 development board
  - MCP9600 thermocouple amplifier
  - K-type thermocouple
  - I2C connections (SDA, SCL)
  
  Wiring:
  - MCP9600 VDD to 3.3V
  - MCP9600 GND to GND
  - MCP9600 SDA to ESP32 GPIO 21 (default SDA)
  - MCP9600 SCL to ESP32 GPIO 22 (default SCL)
  - Thermocouple to MCP9600 thermocouple inputs
  
  Created by Andrew Bertz
  This example is in the public domain.
*/

#include <Wire.h>
#include <ProfileManager.h>

// Create ProfileManager and MCP9600Manager instances
ProfileManager profileManager;
MCP9600Manager mcp9600(0x67); // Default I2C address 0x67

// Define profile array
#define NUM_PROFILES 5
profile_t profiles[NUM_PROFILES];

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("MCP9600 Usage Example");
  Serial.println("=====================");
  
  // Initialize I2C
  Wire.begin();
  
  // Initialize MCP9600
  Serial.println("\nInitializing MCP9600...");
  if (!mcp9600.begin()) {
    Serial.println("Failed to initialize MCP9600! Check wiring.");
    return;
  }
  
  // Print sensor information
  Serial.println("\nSensor Information:");
  mcp9600.printSensorInfo();
  
  // Configure sensor settings
  Serial.println("\nConfiguring sensor settings...");
  mcp9600.setThermocoupleType(MCP9600_TYPE_K);
  mcp9600.setFilterCoefficient(3);
  mcp9600.setADCResolution(MCP9600_ADC_RESOLUTION_18);
  
  // Set up alerts (optional)
  mcp9600.setAlertTemperature(1, 200.0); // Alert at 200°C
  mcp9600.setAlertHysteresis(1, 5.0);    // 5°C hysteresis
  mcp9600.setAlertMode(1, MCP9600_ALERT_HIGH);
  
  Serial.println("MCP9600 configured successfully!");
}

void loop() {
  static unsigned long lastRead = 0;
  const unsigned long readInterval = 1000; // Read every second
  
  if (millis() - lastRead >= readInterval) {
    lastRead = millis();
    
    // Read temperatures
    float thermocouple_temp = mcp9600.readThermocoupleTemperature();
    float ambient_temp = mcp9600.readAmbientTemperature();
    
    // Check for faults
    if (mcp9600.checkFault()) {
      uint8_t fault = mcp9600.getFaultStatus();
      Serial.println("Fault detected: " + mcp9600.getFaultDescription(fault));
    } else {
      // Print temperature readings
      Serial.print("Thermocouple: ");
      if (thermocouple_temp != -999.0) {
        Serial.print(thermocouple_temp, 2);
        Serial.print("°C");
      } else {
        Serial.print("Error");
      }
      
      Serial.print(" | Ambient: ");
      if (ambient_temp != -999.0) {
        Serial.print(ambient_temp, 2);
        Serial.print("°C");
      } else {
        Serial.print("Error");
      }
      
      Serial.println();
    }
    
    // Example: Use temperature in a simple control loop
    if (thermocouple_temp > 0 && thermocouple_temp < 500) {
      // Temperature is valid, could be used for control
      if (thermocouple_temp > 100) {
        Serial.println("Temperature is high!");
      } else if (thermocouple_temp < 50) {
        Serial.println("Temperature is low.");
      }
    }
  }
  
  delay(100);
}
