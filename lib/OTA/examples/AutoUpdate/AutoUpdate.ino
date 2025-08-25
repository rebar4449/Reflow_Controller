/*
  Auto Update OTA Example
  
  This example demonstrates automatic update checking with periodic checks.
  It checks for updates every hour and can be triggered manually via Serial command.
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

// Timing configuration
const unsigned long UPDATE_CHECK_INTERVAL = 3600000; // 1 hour in milliseconds
unsigned long lastUpdateCheck = 0;

// Create OTA instance
OTA ota(versionUrl, firmwareUrl, currentVersion);

void setup() {
    Serial.begin(115200);
    Serial.println("Starting OTA Auto Update Example...");
    Serial.println("Commands:");
    Serial.println("  'check' - Check for updates manually");
    Serial.println("  'update' - Perform update if available");
    Serial.println("  'status' - Show current status");
    
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
    
    // Perform initial update check
    Serial.println("Performing initial update check...");
    checkForUpdates();
}

void loop() {
    // Check for updates periodically
    if (millis() - lastUpdateCheck >= UPDATE_CHECK_INTERVAL) {
        Serial.println("Performing scheduled update check...");
        checkForUpdates();
        lastUpdateCheck = millis();
    }
    
    // Handle Serial commands
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        if (command == "check") {
            Serial.println("Manual update check requested...");
            checkForUpdates();
        } else if (command == "update") {
            Serial.println("Manual update requested...");
            if (ota.isUpdateAvailable()) {
                Serial.println("Update available! Performing update...");
                ota.performUpdate();
            } else {
                Serial.println("No update available.");
            }
        } else if (command == "status") {
            showStatus();
        } else {
            Serial.println("Unknown command. Available commands: check, update, status");
        }
    }
    
    // Your main application code here
    delay(100);
}

void checkForUpdates() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected. Cannot check for updates.");
        return;
    }
    
    Serial.println("Checking for firmware updates...");
    ota.checkForUpdate();
    
    if (ota.isUpdateAvailable()) {
        Serial.println("Update available! Use 'update' command to perform update.");
    } else {
        Serial.println("No update available.");
    }
}

void showStatus() {
    Serial.println("=== STATUS ===");
    Serial.print("WiFi Status: ");
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Connected");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("Disconnected");
    }
    
    Serial.print("Current Version: ");
    Serial.println(currentVersion);
    
    Serial.print("Update Available: ");
    Serial.println(ota.isUpdateAvailable() ? "Yes" : "No");
    
    Serial.print("Last Update Check: ");
    Serial.print((millis() - lastUpdateCheck) / 1000);
    Serial.println(" seconds ago");
    
    Serial.print("Next Scheduled Check: ");
    Serial.print((UPDATE_CHECK_INTERVAL - (millis() - lastUpdateCheck)) / 1000);
    Serial.println(" seconds from now");
}
