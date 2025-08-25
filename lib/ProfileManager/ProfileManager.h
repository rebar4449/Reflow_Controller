#ifndef PROFILE_MANAGER_H
#define PROFILE_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <FS.h>
#include <Wire.h>
#include <Adafruit_MCP9600.h>

// Profile structure definition
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

class ProfileManager {
private:
  Preferences preferences;
  char spaceName[16];
  
  // Helper function to convert data to bytes
  template <typename T> 
  unsigned int convert_to_byte(const T& value, uint8_t output[sizeof(profile_t)], int multiplier);

public:
  ProfileManager();
  ~ProfileManager();
  
  // JSON parsing and profile management
  void parseJsonProfile(fs::FS &fs, String fileName, int profileIndex, profile_t* profile);
  
  // Profile storage and retrieval
  void saveProfiles(int profileIndex, profile_t profile);
  void loadProfiles(int profileIndex, profile_t* profile);
  
  // Profile selection and comparison
  void saveSelectedProfile(int profileIndex);
  void compareProfiles(profile_t profile_new, profile_t profile_saved, int profileIndex);
  
  // Utility functions
  int getSelectedProfile();
  void printProfileInfo(profile_t profile, int profileIndex);
};

// MCP9600 Thermocouple Manager Class
class MCP9600Manager {
private:
  Adafruit_MCP9600 mcp9600;
  uint8_t i2c_address;
  bool sensor_initialized;
  
public:
  MCP9600Manager(uint8_t address = 0x67);
  ~MCP9600Manager();
  
  // Initialization
  bool begin();
  bool begin(TwoWire *theWire);
  
  // Temperature reading
  float readThermocoupleTemperature();
  float readAmbientTemperature();
  
  // Configuration
  bool setThermocoupleType(uint8_t type);
  bool setFilterCoefficient(uint8_t coefficient);
  bool setADCResolution(uint8_t resolution);
  bool setAlertTemperature(uint8_t alert_num, float temperature);
  bool setAlertHysteresis(uint8_t alert_num, float hysteresis);
  bool setAlertMode(uint8_t alert_num, uint8_t mode);
  
  // Status and fault checking
  bool checkFault();
  uint8_t getFaultStatus();
  String getFaultDescription(uint8_t fault);
  bool isConnected();
  
  // Utility functions
  void printSensorInfo();
  void reset();
};

#endif // PROFILE_MANAGER_H
