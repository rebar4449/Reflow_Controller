#ifndef CONFIG_H
#define CONFIG_H

#ifndef ARDUINO_H
#include <Arduino.h>
#endif

#define DEBUG

#define VERBOSE 1 

// RGB LED pin definitions
#define RGB_LED_R 4
#define RGB_LED_G 16
#define RGB_LED_B 17

// LCD pin definition
// Display pin definitions (ILI9341)
#define display_CS   15   // goes to TFT CS
#define display_DC    2  // goes to TFT DC
#define display_RST  -1  // goes to TFT RESET
#define display_MOSI 13  // goes to TFT MOSI
#define display_CLK  14  // goes to TFT SCK/CLK
#define display_MISO 12    // Not connected
#define display_BL   21  // goes to TFT BL

// Touchscreen pin definitions (XPT2046)
#define XPT2046_CLK 25
#define XPT2046_MISO 39
#define XPT2046_MOSI 32
#define XPT2046_CS  33  // Touchscreen CS pin
#define XPT2046_IRQ 36   // Touchscreen IRQ pin (optional)
//       3.3V     // Goes to TFT LED
//       5v       // Goes to TFT Vcc
//       Gnd      // Goes to TFT Gnd

// I2C pin definitions
#define I2C_SDA 22
#define I2C_SCL 27

// Profile configuration
#define NUM_OF_PROFILES 10

// Output pin definitions
#define SSR_PIN 26      // Solid State Relay pin
//#define BUZZER_PIN 26   // Buzzer pin

// SD Card pin definitions
#define SD_CS_PIN       5
#define SD_MISO_PIN     19
#define SD_MOSI_PIN     23
#define SD_CLK_PIN      18

// FW info
const String fwVersion = "0.3.0.0";
#define ServerVersion "1.0"
#define projectName "ESP32 Reflow Oven Controller"
#define WMManager

// OTA settings - extern declarations only
extern String version_url;
extern int contentLength;
extern bool isValidContentType;
extern String host;
extern int port;
extern String bin;
extern String readString;
extern String readVersion;

#endif // CONFIG_H