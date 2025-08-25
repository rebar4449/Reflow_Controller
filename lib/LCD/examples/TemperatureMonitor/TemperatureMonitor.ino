/*
  Temperature Monitor Example for LCD Library
  
  This example demonstrates basic temperature monitoring
  using the LCD library for an ESP32 reflow controller.
  
  Hardware Requirements:
  - ESP32 development board
  - TFT display (ILI9341 or compatible)
  
  Created by: Your Name
  Date: 2024
*/

#include <TFT_eSPI.h>
#include "LCD.h"

// Initialize TFT display
TFT_eSPI tft = TFT_eSPI();

// Initialize LCD library
LCD lcd(tft);

void setup() {
    Serial.begin(115200);
    Serial.println("LCD Temperature Monitor Example");
    
    // Initialize TFT display
    tft.init();
    tft.setRotation(1);  // Landscape orientation
    tft.fillScreen(TFT_BLACK);
    
    // Configure LCD library
    lcd.setHorizontal(true);      // Set horizontal orientation
    lcd.setActiveStatus("Monitoring");
    lcd.setFwVersion("1.0.0");
    lcd.setInputInt(25);          // Initial temperature
    lcd.setConnected(true);       // WiFi connected
    lcd.setFault(false);          // No faults
    
    // Show start screen
    lcd.startScreen();
    
    Serial.println("Temperature monitor started");
}

void loop() {
    // Simulate temperature reading (replace with actual sensor)
    static int temp = 25;
    static unsigned long lastUpdate = 0;
    
    // Update temperature every second
    if (millis() - lastUpdate > 1000) {
        lastUpdate = millis();
        
        // Simulate temperature changes
        temp += random(-2, 3);  // Random temperature variation
        if (temp < 20) temp = 20;
        if (temp > 100) temp = 100;
        
        // Update LCD with new temperature
        lcd.setInputInt(temp);
        
        // Update status based on temperature
        if (temp < 50) {
            lcd.setActiveStatus("Cool");
        } else if (temp < 100) {
            lcd.setActiveStatus("Warm");
        } else {
            lcd.setActiveStatus("Hot");
        }
        
        Serial.print("Temperature: ");
        Serial.print(temp);
        Serial.println("°C");
    }
    
    // Update display
    lcd.loopScreen();
    
    delay(100);
}
