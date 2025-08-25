/*
  BasicUsage example for ProfileManager library
  
  This example demonstrates the basic usage of the ProfileManager library
  for ESP32 reflow oven controllers.
  
  Created by Andrew Bertz
  This example is in the public domain.
*/

#include <SPIFFS.h>
#include <ProfileManager.h>

// Create ProfileManager instance
ProfileManager profileManager;

// Define profile array
#define NUM_PROFILES 5
profile_t profiles[NUM_PROFILES];

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("ProfileManager Basic Usage Example");
  Serial.println("==================================");
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  
  // Example 1: Parse JSON profile from file
  Serial.println("\n1. Parsing JSON profile from file...");
  profileManager.parseJsonProfile(SPIFFS, "/profile1.json", 0, profiles);
  
  // Example 2: Save profile to preferences
  Serial.println("\n2. Saving profile to preferences...");
  profileManager.saveProfiles(0, profiles[0]);
  
  // Example 3: Load profile from preferences
  Serial.println("\n3. Loading profile from preferences...");
  profileManager.loadProfiles(0, profiles);
  
  // Example 4: Save selected profile
  Serial.println("\n4. Saving selected profile...");
  profileManager.saveSelectedProfile(0);
  
  // Example 5: Get selected profile
  Serial.println("\n5. Getting selected profile...");
  int selectedProfile = profileManager.getSelectedProfile();
  Serial.println("Selected profile: " + String(selectedProfile));
  
  // Example 6: Print profile information
  Serial.println("\n6. Printing profile information...");
  profileManager.printProfileInfo(profiles[0], 0);
  
  // Example 7: Compare profiles
  Serial.println("\n7. Comparing profiles...");
  profile_t newProfile = profiles[0];
  strcpy(newProfile.title, "Modified Profile");
  profileManager.compareProfiles(newProfile, profiles[0], 0);
  
  Serial.println("\nExample completed!");
}

void loop() {
  // Nothing to do in loop for this example
  delay(1000);
}
