#include "ProfileManager.h"

// ============================================================================
// ProfileManager Implementation
// ============================================================================

// Constructor
ProfileManager::ProfileManager() {
  // Initialize spaceName buffer
  memset(spaceName, 0, sizeof(spaceName));
}

// Destructor
ProfileManager::~ProfileManager() {
  // Cleanup if needed
}

// Helper function to convert data to bytes
template <typename T> 
unsigned int ProfileManager::convert_to_byte(const T& value, uint8_t output[sizeof(profile_t)], int multiplier) {
  const byte * p = (const byte*) &value;
  unsigned int i;
  for (i = 0; i < sizeof value; i++)
    output[multiplier * i] = (*p++);
  return i;
}

// Parse JSON profile from file
void ProfileManager::parseJsonProfile(fs::FS &fs, String fileName, int profileIndex, profile_t* profile) {
  StaticJsonDocument<500> newDoc;
  JsonArray array = newDoc.to<JsonArray>();

  Serial.println();
  Serial.println("Starting to parse " + fileName + " file.");

  // Open file for reading
  File file = fs.open(fileName);
  if (!file) {
    Serial.println("Failed to open file: " + fileName);
    return;
  }

  // Allocate a temporary JsonDocument
  StaticJsonDocument<1280> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);

  // Test if parsing succeeds
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    file.close();
    return;
  }

  // Extract profile data from JSON
  strcpy(profile[profileIndex].title, doc["title"]);             // "Lead 183"
  strcpy(profile[profileIndex].alloy, doc["alloy"]);             // "Sn63/Pb37"
  profile[profileIndex].melting_point    = doc["melting_point"]; // 183

  profile[profileIndex].temp_range_0     = doc["temp_range"][0]; // 30
  profile[profileIndex].temp_range_1     = doc["temp_range"][1]; // 235

  profile[profileIndex].time_range_0     = doc["time_range"][0]; // 0
  profile[profileIndex].time_range_1     = doc["time_range"][1]; // 340

  strcpy(profile[profileIndex].reference, doc["reference"]);     // "https://www.chipquik.com/datasheets/TS391AX50.pdf"

  JsonObject stages = doc["stages"];

  profile[profileIndex].stages_preheat_0 = stages["preheat"][0]; // 30
  profile[profileIndex].stages_preheat_1 = stages["preheat"][1]; // 100

  profile[profileIndex].stages_soak_0    = stages["soak"][0];    // 120
  profile[profileIndex].stages_soak_1    = stages["soak"][1];    // 150

  profile[profileIndex].stages_reflow_0  = stages["reflow"][0];  // 150
  profile[profileIndex].stages_reflow_1  = stages["reflow"][1];  // 183

  profile[profileIndex].stages_cool_0    = stages["cool"][0];    // 240
  profile[profileIndex].stages_cool_1    = stages["cool"][1];    // 183

  file.close();

  // Print profile data for debugging
  Serial.println("Profile data: "
                 + String(profile[profileIndex].title) + ","
                 + String(profile[profileIndex].alloy) + ","
                 + String(profile[profileIndex].melting_point) + ","
                 + String(profile[profileIndex].temp_range_0) + ","
                 + String(profile[profileIndex].temp_range_1) + ","
                 + String(profile[profileIndex].time_range_0) + ","
                 + String(profile[profileIndex].time_range_1) + ","
                 + String(profile[profileIndex].reference) + ","
                 + String(profile[profileIndex].stages_preheat_0) + ","
                 + String(profile[profileIndex].stages_preheat_1) + ","
                 + String(profile[profileIndex].stages_soak_0) + ","
                 + String(profile[profileIndex].stages_soak_1) + ","
                 + String(profile[profileIndex].stages_reflow_0) + ","
                 + String(profile[profileIndex].stages_reflow_1) + ","
                 + String(profile[profileIndex].stages_cool_0) + ","
                 + String(profile[profileIndex].stages_cool_1));

  // Build array for serialization
  array.add(profile[profileIndex].title);
  array.add(profile[profileIndex].alloy);
  array.add(profile[profileIndex].melting_point);
  array.add(profile[profileIndex].temp_range_0);
  array.add(profile[profileIndex].temp_range_1);
  array.add(profile[profileIndex].time_range_0);
  array.add(profile[profileIndex].time_range_1);
  array.add(profile[profileIndex].reference);
  array.add(profile[profileIndex].stages_preheat_0);
  array.add(profile[profileIndex].stages_preheat_1);
  array.add(profile[profileIndex].stages_soak_0);
  array.add(profile[profileIndex].stages_soak_1);
  array.add(profile[profileIndex].stages_reflow_0);
  array.add(profile[profileIndex].stages_reflow_1);
  array.add(profile[profileIndex].stages_cool_0);
  array.add(profile[profileIndex].stages_cool_1);

  // Serialize the array and send the result to Serial
  serializeJson(newDoc, Serial);
  Serial.println();
  size_t len = measureJson(newDoc); // Get length of the array
  Serial.println("Length of the array is: " + String(len));
  Serial.println();
}

// Save profile to preferences
void ProfileManager::saveProfiles(int profileIndex, profile_t profile) {
  sprintf(spaceName, "profile%0d", profileIndex);
  
  // Put all profiles into Preferences
  preferences.begin(spaceName);

  uint8_t content[sizeof(profile_t)];
  convert_to_byte(profile, content, 0);

  // Get the possible length of saved array
  size_t schLen = sizeof(profile_t);
  
  // Put all bytes into "profiles"
  preferences.putBytes(spaceName, content, schLen);
  
  // Test read the data back and test their correct number
  char buffer[schLen];
  preferences.getBytes(spaceName, buffer, schLen);
  
  if (schLen % sizeof(profile_t)) {
    Serial.println("Data is not correct size!");
    preferences.end();
    return;
  }
  
  profile_t *profiles = (profile_t *) buffer;
  
  // Check of Saved profiles
  Serial.println("Title from saved profile: ");
  Serial.print((String)profileIndex + ". ");
  Serial.println(profiles[0].title);
  Serial.println();
  
  preferences.end();
}

// Load profile from preferences
void ProfileManager::loadProfiles(int profileIndex, profile_t* profile) {
  int profileSize;
  sprintf(spaceName, "profile%d", profileIndex);
  
  // Extract "profiles" from memory
  preferences.begin(spaceName);
  
  // Extract the size of saved array
  size_t schLen = preferences.getBytes(spaceName, NULL, NULL);
  
  // Check, that Preferences are not empty
  if (schLen > 0) {
    if (schLen % sizeof(profile_t)) {
      Serial.println("Data is not correct size!");
      preferences.end();
      return;
    }
    
    // Get array to buffer and check the size
    char buffer[schLen];
    preferences.getBytes(spaceName, buffer, schLen);
    
    Serial.print("Buffer: ");
    Serial.println(buffer);
    
    // Save the extracted data into variable
    profile_t *profiles = (profile_t *) buffer;
    profileSize = schLen / sizeof(profile_t);
    
    // Load profiles from memory to array for program usage
    if (profileSize == 1) {
      profile[profileIndex] = profiles[0];
      
      // Print of Title names loaded from Preferences
      Serial.println("Title from loaded profile: ");
      Serial.print((String)profileIndex + ". ");
      Serial.println(profile[profileIndex].title);
      Serial.println();
    }
    else {
      Serial.println("Error during load of data in loadProfiles.");
    }
  }
  else {
    Serial.print("No data found in Preferences memory ");
    Serial.println(profileIndex);
  }
  
  preferences.end();
}

// Save selected profile index
void ProfileManager::saveSelectedProfile(int profileIndex) {
  preferences.begin("store");
  preferences.putInt("profileUsed", profileIndex);
  int profileUsed = preferences.getInt("profileUsed", 0);
  preferences.end();
  Serial.println("Saved profile # " + String(profileIndex));
}

// Get selected profile index
int ProfileManager::getSelectedProfile() {
  preferences.begin("store");
  int profileUsed = preferences.getInt("profileUsed", 0);
  preferences.end();
  return profileUsed;
}

// Compare profiles and save if different
void ProfileManager::compareProfiles(profile_t profile_new, profile_t profile_saved, int profileIndex) {
  if (profile_new.title[profileIndex] == profile_saved.title[profileIndex]) {
    Serial.println("Profile " + String(profileIndex) + " match");
  }
  else {
    Serial.println("Profile " + String(profileIndex) + " do not match");
    saveProfiles(profileIndex, profile_new);
  }
}

// Print profile information
void ProfileManager::printProfileInfo(profile_t profile, int profileIndex) {
  Serial.println("=== Profile " + String(profileIndex) + " ===");
  Serial.println("Title: " + String(profile.title));
  Serial.println("Alloy: " + String(profile.alloy));
  Serial.println("Melting Point: " + String(profile.melting_point) + "°C");
  Serial.println("Temperature Range: " + String(profile.temp_range_0) + "°C - " + String(profile.temp_range_1) + "°C");
  Serial.println("Time Range: " + String(profile.time_range_0) + "s - " + String(profile.time_range_1) + "s");
  Serial.println("Reference: " + String(profile.reference));
  Serial.println("Preheat: " + String(profile.stages_preheat_0) + "°C - " + String(profile.stages_preheat_1) + "°C");
  Serial.println("Soak: " + String(profile.stages_soak_0) + "°C - " + String(profile.stages_soak_1) + "°C");
  Serial.println("Reflow: " + String(profile.stages_reflow_0) + "°C - " + String(profile.stages_reflow_1) + "°C");
  Serial.println("Cool: " + String(profile.stages_cool_0) + "°C - " + String(profile.stages_cool_1) + "°C");
  Serial.println("========================");
}

// ============================================================================
// MCP9600Manager Implementation
// ============================================================================

// Constructor
MCP9600Manager::MCP9600Manager(uint8_t address) {
  i2c_address = address;
  sensor_initialized = false;
}

// Destructor
MCP9600Manager::~MCP9600Manager() {
  // Cleanup if needed
}

// Initialize with default I2C
bool MCP9600Manager::begin() {
  return begin(&Wire);
}

// Initialize with specified I2C bus
bool MCP9600Manager::begin(TwoWire *theWire) {
  if (!mcp9600.begin(i2c_address, theWire)) {
    Serial.println("MCP9600 not found! Check wiring and I2C address.");
    sensor_initialized = false;
    return false;
  }
  
  Serial.println("MCP9600 found!");
  
  // Set default configuration
  mcp9600.setThermocoupleType(MCP9600_TYPE_K);
  mcp9600.setFilterCoefficient(3);
  mcp9600.setADCResolution(MCP9600_ADC_RESOLUTION_18);
  
  sensor_initialized = true;
  return true;
}

// Read thermocouple temperature
float MCP9600Manager::readThermocoupleTemperature() {
  if (!sensor_initialized) {
    Serial.println("MCP9600 not initialized!");
    return -999.0; // Error value
  }
  
  float temp = mcp9600.readThermocouple();
  
  // Check for fault conditions
  if (mcp9600.check()) {
    uint8_t fault = mcp9600.getFault();
    Serial.println("MCP9600 Fault: " + getFaultDescription(fault));
    return -999.0; // Error value
  }
  
  return temp;
}

// Read ambient temperature
float MCP9600Manager::readAmbientTemperature() {
  if (!sensor_initialized) {
    Serial.println("MCP9600 not initialized!");
    return -999.0; // Error value
  }
  
  return mcp9600.readAmbient();
}

// Set thermocouple type
bool MCP9600Manager::setThermocoupleType(mcp9600_thermocoupletype_t type) {
  if (!sensor_initialized) {
    Serial.println("MCP9600 not initialized!");
    return false;
  }
  
  return mcp9600.setThermocoupleType(type);
}

// Set filter coefficient
bool MCP9600Manager::setFilterCoefficient(uint8_t coefficient) {
  if (!sensor_initialized) {
    Serial.println("MCP9600 not initialized!");
    return false;
  }
  
  return mcp9600.setFilterCoefficient(coefficient);
}

// Set ADC resolution
bool MCP9600Manager::setADCResolution(mcp9600_adc_resolution_t resolution) {
  if (!sensor_initialized) {
    Serial.println("MCP9600 not initialized!");
    return false;
  }
  
  return mcp9600.setADCResolution(resolution);
}

// Set alert temperature
bool MCP9600Manager::setAlertTemperature(uint8_t alert_num, float temperature) {
  if (!sensor_initialized) {
    Serial.println("MCP9600 not initialized!");
    return false;
  }
  
  return mcp9600.setAlertTemperature(alert_num, temperature);
}

// Set alert hysteresis
bool MCP9600Manager::setAlertHysteresis(uint8_t alert_num, float hysteresis) {
  if (!sensor_initialized) {
    Serial.println("MCP9600 not initialized!");
    return false;
  }
  
  return mcp9600.setAlertHysteresis(alert_num, hysteresis);
}

// Set alert mode
bool MCP9600Manager::setAlertMode(uint8_t alert_num, mcp9600_alert_mode_t mode) {
  if (!sensor_initialized) {
    Serial.println("MCP9600 not initialized!");
    return false;
  }
  
  return mcp9600.setAlertMode(alert_num, mode);
}

// Check for faults
bool MCP9600Manager::checkFault() {
  if (!sensor_initialized) {
    return true; // Consider uninitialized as fault
  }
  
  return mcp9600.check();
}

// Get fault status
uint8_t MCP9600Manager::getFaultStatus() {
  if (!sensor_initialized) {
    return 0xFF; // Return all bits set for uninitialized
  }
  
  return mcp9600.getFault();
}

// Get fault description
String MCP9600Manager::getFaultDescription(uint8_t fault) {
  String description = "";
  
  if (fault & MCP9600_FAULT_OPEN) {
    description += "Thermocouple Open, ";
  }
  if (fault & MCP9600_FAULT_SHORT_VCC) {
    description += "Thermocouple Short to VCC, ";
  }
  if (fault & MCP9600_FAULT_SHORT_GND) {
    description += "Thermocouple Short to GND, ";
  }
  if (fault & MCP9600_FAULT_SHORT_DATA) {
    description += "Thermocouple Short to Data, ";
  }
  if (fault & MCP9600_FAULT_ALERT1) {
    description += "Alert 1 Active, ";
  }
  if (fault & MCP9600_FAULT_ALERT2) {
    description += "Alert 2 Active, ";
  }
  if (fault & MCP9600_FAULT_ALERT3) {
    description += "Alert 3 Active, ";
  }
  if (fault & MCP9600_FAULT_ALERT4) {
    description += "Alert 4 Active, ";
  }
  if (fault & MCP9600_FAULT_DEVICE_DIE_TEMP) {
    description += "Device Die Temperature High, ";
  }
  
  if (description.length() > 0) {
    description = description.substring(0, description.length() - 2); // Remove last ", "
  } else {
    description = "No Fault";
  }
  
  return description;
}

// Check if sensor is connected
bool MCP9600Manager::isConnected() {
  return sensor_initialized;
}

// Print sensor information
void MCP9600Manager::printSensorInfo() {
  if (!sensor_initialized) {
    Serial.println("MCP9600 not initialized!");
    return;
  }
  
  Serial.println("=== MCP9600 Sensor Information ===");
  Serial.println("I2C Address: 0x" + String(i2c_address, HEX));
  Serial.println("Thermocouple Type: K-Type");
  Serial.println("Filter Coefficient: 3");
  Serial.println("ADC Resolution: 18-bit");
  
  float thermocouple_temp = readThermocoupleTemperature();
  float ambient_temp = readAmbientTemperature();
  
  if (thermocouple_temp != -999.0) {
    Serial.println("Thermocouple Temperature: " + String(thermocouple_temp, 2) + "°C");
  } else {
    Serial.println("Thermocouple Temperature: Error");
  }
  
  if (ambient_temp != -999.0) {
    Serial.println("Ambient Temperature: " + String(ambient_temp, 2) + "°C");
  } else {
    Serial.println("Ambient Temperature: Error");
  }
  
  if (checkFault()) {
    uint8_t fault = getFaultStatus();
    Serial.println("Fault Status: " + getFaultDescription(fault));
  } else {
    Serial.println("Fault Status: No Fault");
  }
  
  Serial.println("==================================");
}

// Reset sensor
void MCP9600Manager::reset() {
  if (sensor_initialized) {
    mcp9600.reset();
    delay(100); // Give time for reset
  }
}
