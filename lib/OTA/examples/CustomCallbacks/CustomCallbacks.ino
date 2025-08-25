/*
  Custom Callbacks OTA Example
  
  This example demonstrates how to use custom callback functions
  to provide user interface feedback during the update process.
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

// Custom callback functions
void onUpdateStart() {
    Serial.println("=== UPDATE STARTING ===");
    Serial.println("Please do not power off the device!");
    Serial.println("Update in progress...");
    
    // You could also control LEDs, LCD display, etc. here
    // digitalWrite(LED_PIN, HIGH);
    // lcd.clear();
    // lcd.print("Updating...");
}

void onUpdateSuccess() {
    Serial.println("=== UPDATE SUCCESSFUL ===");
    Serial.println("Firmware has been updated successfully!");
    Serial.println("Device will restart in 3 seconds...");
    
    // You could also control LEDs, LCD display, etc. here
    // digitalWrite(LED_PIN, LOW);
    // lcd.clear();
    // lcd.print("Update Complete!");
    // lcd.setCursor(0, 1);
    // lcd.print("Restarting...");
    
    delay(3000);
}

void onUpdateError(const String& error) {
    Serial.println("=== UPDATE ERROR ===");
    Serial.print("Error: ");
    Serial.println(error);
    Serial.println("Please check your connection and try again.");
    
    // You could also control LEDs, LCD display, etc. here
    // digitalWrite(ERROR_LED_PIN, HIGH);
    // lcd.clear();
    // lcd.print("Update Failed!");
    // lcd.setCursor(0, 1);
    // lcd.print(error);
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting OTA Custom Callbacks Example...");
    
    // Set custom callbacks
    ota.setUpdateStartCallback(onUpdateStart);
    ota.setUpdateSuccessCallback(onUpdateSuccess);
    ota.setUpdateErrorCallback(onUpdateError);
    
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
