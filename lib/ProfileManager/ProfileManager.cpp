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

// MCP9600Manager implementation moved to separate library
