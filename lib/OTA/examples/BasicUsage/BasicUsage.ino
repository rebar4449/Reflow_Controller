/*
  Basic OTA Example
  
  This example demonstrates the basic usage of the OTA library.
  It connects to WiFi, checks for updates, and performs the update if available.
*/

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
    Serial.println("Starting OTA Basic Example...");
    
    // Connect to WiFi
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    
    Serial.println();
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    // Check for updates
    Serial.println("Checking for firmware updates...");
    ota.checkForUpdate();
    
    if (ota.isUpdateAvailable()) {
        Serial.println("Update available! Performing update...");
        ota.performUpdate();
    } else {
        Serial.println("No update available. Continuing with normal operation...");
    }
}

void loop() {
    // Your main application code here
    Serial.println("Main application running...");
    delay(5000);
}
