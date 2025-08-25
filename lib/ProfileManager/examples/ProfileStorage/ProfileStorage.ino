/*
  ProfileStorage example for ProfileManager library
  
  This example demonstrates advanced profile storage and management
  functionality for ESP32 reflow oven controllers.
  
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

// Sample profile data
void createSampleProfiles() {
  // Profile 1: Lead-free solder
  strcpy(profiles[0].title, "Lead-Free SAC305");
  strcpy(profiles[0].alloy, "SAC305");
  profiles[0].melting_point = 217;
  profiles[0].temp_range_0 = 25;
  profiles[0].temp_range_1 = 250;
  profiles[0].time_range_0 = 0;
  profiles[0].time_range_1 = 300;
  strcpy(profiles[0].reference, "IPC-J-STD-020D");
  profiles[0].stages_preheat_0 = 25;
  profiles[0].stages_preheat_1 = 150;
  profiles[0].stages_soak_0 = 150;
  profiles[0].stages_soak_1 = 180;
  profiles[0].stages_reflow_0 = 180;
  profiles[0].stages_reflow_1 = 217;
  profiles[0].stages_cool_0 = 217;
  profiles[0].stages_cool_1 = 150;
  
  // Profile 2: Lead solder
  strcpy(profiles[1].title, "Lead Sn63/Pb37");
  strcpy(profiles[1].alloy, "Sn63/Pb37");
  profiles[1].melting_point = 183;
  profiles[1].temp_range_0 = 25;
  profiles[1].temp_range_1 = 220;
  profiles[1].time_range_0 = 0;
  profiles[1].time_range_1 = 280;
  strcpy(profiles[1].reference, "IPC-J-STD-020D");
  profiles[1].stages_preheat_0 = 25;
  profiles[1].stages_preheat_1 = 120;
  profiles[1].stages_soak_0 = 120;
  profiles[1].stages_soak_1 = 150;
  profiles[1].stages_reflow_0 = 150;
  profiles[1].stages_reflow_1 = 183;
  profiles[1].stages_cool_0 = 183;
  profiles[1].stages_cool_1 = 120;
  
  // Profile 3: Low-temperature solder
  strcpy(profiles[2].title, "Low-Temp BiSn");
  strcpy(profiles[2].alloy, "BiSn");
  profiles[2].melting_point = 138;
  profiles[2].temp_range_0 = 25;
  profiles[2].temp_range_1 = 180;
  profiles[2].time_range_0 = 0;
  profiles[2].time_range_1 = 240;
  strcpy(profiles[2].reference, "Custom Profile");
  profiles[2].stages_preheat_0 = 25;
  profiles[2].stages_preheat_1 = 100;
  profiles[2].stages_soak_0 = 100;
  profiles[2].stages_soak_1 = 120;
  profiles[2].stages_reflow_0 = 120;
  profiles[2].stages_reflow_1 = 138;
  profiles[2].stages_cool_0 = 138;
  profiles[2].stages_cool_1 = 100;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("ProfileManager Storage Example");
  Serial.println("==============================");
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  
  // Create sample profiles
  Serial.println("\nCreating sample profiles...");
  createSampleProfiles();
  
  // Save all profiles to preferences
  Serial.println("\nSaving all profiles to preferences...");
  for (int i = 0; i < NUM_PROFILES; i++) {
    profileManager.saveProfiles(i, profiles[i]);
  }
  
  // Load and display all profiles
  Serial.println("\nLoading and displaying all profiles...");
  for (int i = 0; i < NUM_PROFILES; i++) {
    profileManager.loadProfiles(i, profiles);
    profileManager.printProfileInfo(profiles[i], i);
  }
  
  // Demonstrate profile selection
  Serial.println("\nDemonstrating profile selection...");
  profileManager.saveSelectedProfile(1); // Select lead solder profile
  int selected = profileManager.getSelectedProfile();
  Serial.println("Currently selected profile: " + String(selected));
  Serial.println("Profile name: " + String(profiles[selected].title));
  
  // Demonstrate profile comparison
  Serial.println("\nDemonstrating profile comparison...");
  profile_t modifiedProfile = profiles[0];
  strcpy(modifiedProfile.title, "Modified Lead-Free");
  profileManager.compareProfiles(modifiedProfile, profiles[0], 0);
  
  // Load the modified profile
  profileManager.loadProfiles(0, profiles);
  Serial.println("Modified profile title: " + String(profiles[0].title));
  
  Serial.println("\nStorage example completed!");
}

void loop() {
  // Nothing to do in loop for this example
  delay(1000);
}
