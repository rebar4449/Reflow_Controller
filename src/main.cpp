#include <Arduino.h>
#include <config.h>
#include <Adafruit_ILI9341.h>
#include <WiFi.h>
#include <SD.h>
#include <SPIFFS.h>
#include <WiFiManager.h>
#include <SPI.h>
#include "LCD.h"
#include "OTA.h"
#include "ProfileManager.h"
#include "reflow_logic.h"
#include <Adafruit_MCP9600.h>
#include <XPT2046_Touchscreen.h>
#include "TouchInterface.h"
#include "UIManager.h"

// Function prototypes
void updatePreferences();
void processButtons();
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void readFile(fs::FS & fs, String path, const char * type);
void wifiSetup();

// MCP9600 Thermocouple sensor (I2C)
Adafruit_MCP9600 mcp9600;

// Use hardware SPI
//Adafruit_ILI9341 display = Adafruit_ILI9341(display_cs, display_dc, display_rst);
Adafruit_ILI9341 display = Adafruit_ILI9341(display_CS, display_DC, display_MOSI, display_CLK, display_RST);

// Touchscreen instance
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);

// Touch interface and UI manager instances
TouchInterface* touchInterface = nullptr;
UIManager* uiManager = nullptr;

#define FORMAT_SPIFFS_IF_FAILED true

Preferences preferences;
WiFiManager wm;
char msg_buf[10];

// Physical buttons are no longer used - touch interface replaces them
// #define DEBOUNCE_MS 100
// // Create JoystickAxis objects for ButtonNav compatibility
// JoystickAxis AXIS_Y(BUTTON_AXIS_Y, 512);
// JoystickAxis AXIS_X(BUTTON_AXIS_X, 512);

// int digitalButtonPins[] = {BUTTON_SELECT, BUTTON_MENU, BUTTON_BACK};

// #define numDigButtons sizeof(digitalButtonPins)

// Physical button variables are no longer used
// int buttonState;             // the current reading from the input pin
// int lastButtonState = LOW;
// unsigned long lastDebounceTime_ = 0;  // the last time the output pin was toggled
// unsigned long debounceDelay = 200;    // the debounce time; increase if the output flicker

String activeStatus = "";
bool menu = 0;
bool isFault = 0;
bool connected = 0;
bool horizontal = 0;
bool fan = 0;
bool buttons = 0;
bool buzzer = 0;
bool useOTA = 0;
bool debug = 0;
bool verboseOutput = 1;
bool disableMenu = 0;
bool profileIsOn = 0;
bool updataAvailable = 0;
bool testState = 0;
bool useSPIFFS = 0 ;

// Physical button variables are no longer used - touch interface replaces them
// All button handling is now done through TouchInterface
int oldTemp = 0;

byte numOfPointers = 0;
byte state = 0; // 0 = boot, 1 = main menu, 2 = select profile, 3 = change profile, 4 = add profile, 5 = settings, 6 = info, 7 = start reflow, 8 = stop reflow, 9 = test outputs
byte previousState = 0;

byte settings_pointer = 0;
byte previousSettingsPointer = 0;
bool   SD_present = false;
//char* json = "";
int profileNum = 0;
String jsonName[NUM_OF_PROFILES];
int profileUsed = 0;
char spaceName[] = "profile00";

// Profile structure is defined in ProfileManager.h

profile_t paste_profile[NUM_OF_PROFILES]; //declaration of struct type array

// OTA variable definitions
String version_url = "http://czechmaker.com/roc_version.txt";
int contentLength = 0;
bool isValidContentType = false;
String host = "yourdomain.com"; // Host => bucket-name.s3.region.amazonaws.com
int port = 80; // Non https. For HTTPS 443. As of today, HTTPS doesn't work.
String bin = "/some.bin"; // bin file name with a slash in front.
String readString;
String readVersion;

// Library instances
// ButtonHandler buttonHandler; // No longer used - touch interface replaces physical buttons
// LCD lcd(display); // TODO: Fix LCD compatibility
OTA ota("", "", ""); // TODO: Add proper URLs
ProfileManager profileManager;

// Variables for reflow logic
int windowSize;
unsigned long nextCheck;
unsigned long nextRead;

// PID control variables
double setpoint;
double input;
double output;
double kp = PID_KP_PREHEAT;
double ki = PID_KI_PREHEAT;
double kd = PID_KD_PREHEAT;
int inputInt;
unsigned long windowStartTime;
unsigned long timerSoak;
unsigned long buzzerPeriod;

// Reflow state variables
ReflowState reflowState;
ReflowStatus reflowStatus;
DebounceState debounceState;
long lastDebounceTime;
Switch switchStatus;
int timerSeconds;

// PID control object
PID reflowOvenPID(&input, &output, &setpoint, kp, ki, kd, DIRECT);

void setup() {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

  Serial.begin(115200);

  Serial.println(projectName);

  Serial.println("FW version is: " + String(fwVersion) + "_&_" + String(__DATE__) + "_&_" + String(__TIME__));

  preferences.begin("store", false);
  buttons = preferences.getBool("buttons", 0);
  fan = preferences.getBool("fan", 0);
  horizontal = preferences.getBool("horizontal", 0);
  buzzer = preferences.getBool("buzzer", 0);
  useOTA = preferences.getBool("useOTA", 0);
  profileUsed = preferences.getInt("profileUsed", 0);
  useSPIFFS = preferences.getBool("useSPIFFS", 0);
  preferences.end();

  Serial.println();
  Serial.println("Buttons: " + String(buttons));
  Serial.println("Fan is: " + String(fan));
  Serial.println("Horizontal: " + String(horizontal));
  Serial.println("Buzzer: " + String(buzzer));
  Serial.println("OTA: " + String(useOTA));
  Serial.println("Used profile: " + String(profileUsed));
  Serial.println();
  // load profiles from ESP32 memory
  for (int i = 0; i < NUM_OF_PROFILES; i++) {
    profileManager.loadProfiles(i, &paste_profile[i]);
  }
  display.begin();
  
  // Initialize touch interface and UI manager
  touchInterface = new TouchInterface(&ts, &display);
  touchInterface->begin();
  
  uiManager = new UIManager(touchInterface, &display);
  uiManager->begin();
  
  // Update LCD data with current state
  uiManager->setLCDData();
  
  // Physical button initialization is no longer needed
  // buttonHandler.begin(digitalButtonPins, numDigButtons);
  // buttonHandler.setJoystickAxes(&AXIS_X, &AXIS_Y);
  // buttonHandler.setConfiguration(verboseOutput, horizontal, buttons, state, previousState, 
  //                               settings_pointer, previousSettingsPointer, numOfPointers, 
  //                               profileIsOn, disableMenu, buzzer, useOTA, useSPIFFS, fan, testState, activeStatus);
  
  // lcd.startScreen(); // TODO: Fix LCD compatibility

  if ( !SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("Error mounting SPIFFS");
    return;
  }

  // SSR pin initialization to ensure reflow oven is off

  pinMode(SSR_PIN, OUTPUT);
  digitalWrite(SSR_PIN, LOW);

  // Buzzer pin initialization to ensure annoying buzzer is off
  //digitalWrite(BUZZER_PIN, LOW);
  //pinMode(BUZZER_PIN, OUTPUT);

  // LED pins initialization and turn on upon start-up (active low)
  pinMode(RGB_LED_R, OUTPUT);
  pinMode(RGB_LED_G, OUTPUT);
  pinMode(RGB_LED_B, OUTPUT);
  digitalWrite(RGB_LED_R, LOW);
  digitalWrite(RGB_LED_G, LOW);
  digitalWrite(RGB_LED_B, LOW);

  delay(100);

  // Turn off LED (active low)
  //digitalWrite(LED_PIN, LOW);

  // Physical button initialization is no longer needed - touch interface replaces buttons
  // pinMode(BUTTON_AXIS_Y, INPUT_PULLDOWN);
  // pinMode(BUTTON_AXIS_X, INPUT_PULLDOWN);

  // for (byte i = 0; i < numDigButtons - 1 ; i++) {
  //   // Set button input pin
  //   if (digitalButtonPins[i] > 20  && digitalButtonPins[i] < 40) {
  //     pinMode(digitalButtonPins[i], INPUT_PULLUP);
  //     digitalWrite(digitalButtonPins[i], LOW  );
  //     Serial.println(digitalButtonPins[i]);
  //   }
  // }

  wifiSetup();

  if (WiFi.status() == WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    Serial.println("\nConnected to " + WiFi.SSID() + "; IP address: " + WiFi.localIP().toString()); // Report which SSID and IP is in use
    connected = 1;

    if (useOTA != 0) {
      ota.checkForUpdate();
    }
  }

  // Initialize MCP9600 thermocouple sensor
  if (!mcp9600.begin()) {
    Serial.println("MCP9600 sensor not found!");
  } else {
    Serial.println("MCP9600 sensor initialized successfully");
    mcp9600.setThermocoupleType(MCP9600_TYPE_K);
    mcp9600.setADCresolution(MCP9600_ADCRESOLUTION_18);
  }

  // Set window size
  windowSize = 2000;
  // Initialize time keeping variable
  nextCheck = millis();
  // Initialize thermocouple reading variable
  nextRead = millis();
  
  // Initialize reflow state variables
  reflowState = REFLOW_STATE_IDLE;
  reflowStatus = REFLOW_STATUS_OFF;
  debounceState = DEBOUNCE_STATE_IDLE;
  switchStatus = SWITCH_NONE;
  timerSeconds = 0;

  if (useSPIFFS != 0) {
    profileNum = 0;
    listDir(SPIFFS, "/profiles", 0);
  } else {
    Serial.print(F("Initializing SD card..."));
    if (!SD.begin(SD_CS_PIN)) { // see if the card is present and can be initialised. Wemos SD-Card CS uses D8
      Serial.println(F("Card failed or not present, no SD Card data logging possible..."));
      SD_present = false;
    } else {
      Serial.println(F("Card initialised... file access enabled..."));
      SD_present = true;
      // Reset number of profiles for fresh load from SD card
      profileNum = 0;
      listDir(SD, "/profiles", 0);
    }
  }
  // Load data from selected storage
  if ((SD_present == true) || (useSPIFFS != 0)) {
    profile_t paste_profile_load[NUM_OF_PROFILES];
    // Scan all profiles from source

    for (int i = 0; i < profileNum; i++) {
      if (useSPIFFS != 0) {
        profileManager.parseJsonProfile(SPIFFS, jsonName[i], i, &paste_profile_load[i]);
      } else {
        profileManager.parseJsonProfile(SD, jsonName[i], i, &paste_profile_load[i]);
      }
    }
    //Compare profiles, if they are already in memory
    for (int i = 0; i < profileNum; i++) {
      profileManager.compareProfiles(paste_profile_load[i], paste_profile[i], i);
    }
  }

  Serial.println();
  Serial.print("Number of profiles: ");
  Serial.println(profileNum);

  Serial.println("Titles and alloys: ");
  for (int i = 0; i < profileNum; i++) {
    Serial.print((String)i + ". ");
    Serial.print(paste_profile[i].title);
    Serial.print(", ");
    Serial.println(paste_profile[i].alloy);
  }
  Serial.println();
}

void updatePreferences() {
  preferences.begin("store", false);
  preferences.putBool("buttons", buttons);
  preferences.putBool("fan", fan);
  preferences.putBool("horizontal", horizontal);
  preferences.putBool("buzzer", buzzer);
  preferences.putBool("useOTA", useOTA);
  preferences.putBool("useSPIFFS", useSPIFFS);
  preferences.end();

  if (verboseOutput != 0) {
    Serial.println();
    Serial.println("Buttons is: " + String(buttons));
    Serial.println("Fan is: " + String(fan));
    Serial.println("Horizontal is: " + String(horizontal));
    Serial.println("OTA is : " + String(useOTA));
    Serial.println("Use SPIFFS is : " + String(useSPIFFS));
    Serial.println("Buzzer is: " + String(buzzer));
    Serial.println();
  }
}

void processButtons() {
  // Process touch interface instead of physical buttons
  if (uiManager) {
    uiManager->update();
  }
}

void loop() {
  wm.process();
  if (state != 9) { // if we are in test menu, disable LED & SSR control in loop
    reflow_main();
  }
  processButtons();
  
  // Update UI with current temperature and status
  if (uiManager) {
    uiManager->updateTemperature(input);
    uiManager->updateStatus(activeStatus);
    uiManager->setLCDData();  // Keep LCD data in sync
  }
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  String tempFileName;
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      tempFileName = file.name();
      if (tempFileName.endsWith("json")) {
        Serial.println("Find this JSON file: "  + tempFileName);
        jsonName[profileNum] = tempFileName;
        profileNum++;
      }
    }
    file = root.openNextFile();
  }
}

void readFile(fs::FS & fs, String path, const char * type) {
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }
  Serial.print("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void wifiSetup() {
  wm.setConfigPortalBlocking(false);
  if (wm.autoConnect("ReflowOvenAP")) {
    Serial.println("connected...yeey :)");
  }
  else {
    Serial.println("Configportal running");
  }
}

// Reflow main function implementation
void reflow_main() {
  // Current time
  unsigned long now;

  // Time to read thermocouple?
  if (millis() > nextRead) {
    // Read thermocouple next sampling period
    nextRead += SENSOR_SAMPLING_TIME;
    // Read current temperature
    input = mcp9600.readThermocouple();
    // Check for reading errors (simple range check)
    if (input < -200.0 || input > 1000.0) {
      Serial.println("MCP9600 reading out of range: " + String(input));
      isFault = 1;
    }
    inputInt = input / 1;

    if (oldTemp != inputInt) {
      if (state == 0) {
        // loopScreen(); // TODO: Fix LCD compatibility
      }
      if ((input > 0) && (input <= 500)) {
        Serial.print("Float temp: " + String(input));
        Serial.print(" ; ");
        Serial.println("Integer temp: " + String(inputInt));
      }
    }
    // If thermocouple problem detected
    if (input == -999.0) { // MCP9600 error value
      // Illegal operation
      reflowState = REFLOW_STATE_ERROR;
      reflowStatus = REFLOW_STATUS_OFF;
    }
    oldTemp = inputInt;
  }

  if (millis() > nextCheck) {
    // Check input in the next seconds
    nextCheck += 1000;
    // If reflow process is on going
    if (reflowStatus == REFLOW_STATUS_ON) {
      // Toggle red LED as system heart beat
      digitalWrite(RGB_LED_R, !(digitalRead(RGB_LED_R)));
      // Increase seconds timer for reflow curve analysis
      timerSeconds++;
      // Send temperature and time stamp to serial
      Serial.print(timerSeconds);
      Serial.print(" ");
      Serial.print(setpoint);
      Serial.print(" ");
      Serial.print(input);
      Serial.print(" ");
      Serial.println(output);
    } else {
      // Turn off red LED
      digitalWrite(RGB_LED_R, LOW);
    }
    // If currently in error state
    if (reflowState == REFLOW_STATE_ERROR) {
      // No thermocouple wire connected
      Serial.println("TC Error!");
    }
  }

  // Reflow oven controller state machine
  switch (reflowState) {
    case REFLOW_STATE_IDLE:
      activeStatus = "Idle";
      // If oven temperature is still above room temperature
      if (input >= TEMPERATURE_ROOM) {
        reflowState = REFLOW_STATE_TOO_HOT;
        Serial.println("Status: Too hot to start");
      } else {
        // If switch is pressed to start reflow process
        if (profileIsOn != 0) {
          // Send header for CSV file
          Serial.println("Time Setpoint Input Output");
          // Intialize seconds timer for serial debug information
          timerSeconds = 0;
          // Initialize PID control window starting time
          windowStartTime = millis();
          // Ramp up to minimum soaking temperature
          setpoint = paste_profile[profileUsed].stages_preheat_1;
          // Tell the PID to range between 0 and the full window size
          reflowOvenPID.SetOutputLimits(0, windowSize);
          reflowOvenPID.SetSampleTime(PID_SAMPLE_TIME);
          // Turn the PID on
          reflowOvenPID.SetMode(AUTOMATIC);
          // Proceed to preheat stage
          reflowState = REFLOW_STATE_PREHEAT;
        }
      }
      break;

    case REFLOW_STATE_PREHEAT:
      activeStatus = "Preheat";
      reflowStatus = REFLOW_STATUS_ON;
      // If minimum soak temperature is achieve
      if (input >= paste_profile[profileUsed].stages_preheat_1) {
        // Chop soaking period into smaller sub-period
        timerSoak = millis() + SOAK_MICRO_PERIOD;
        // Set less agressive PID parameters for soaking ramp
        reflowOvenPID.SetTunings(PID_KP_SOAK, PID_KI_SOAK, PID_KD_SOAK);
        // Ramp up to first section of soaking temperature
        setpoint = paste_profile[profileUsed].stages_preheat_1 + SOAK_TEMPERATURE_STEP;
        // Proceed to soaking state
        reflowState = REFLOW_STATE_SOAK;
      }
      break;

    case REFLOW_STATE_SOAK:
      activeStatus = "Soak";
      // If micro soak temperature is achieved
      if (millis() > timerSoak) {
        timerSoak = millis() + SOAK_MICRO_PERIOD;
        // Increment micro setpoint
        setpoint += SOAK_TEMPERATURE_STEP;
        if (setpoint > paste_profile[profileUsed].stages_soak_1) {
          // Set agressive PID parameters for reflow ramp
          reflowOvenPID.SetTunings(PID_KP_REFLOW, PID_KI_REFLOW, PID_KD_REFLOW);
          // Ramp up to first section of soaking temperature
          setpoint = paste_profile[profileUsed].stages_reflow_1;
          // Proceed to reflowing state
          reflowState = REFLOW_STATE_REFLOW;
        }
      }
      break;

    case REFLOW_STATE_REFLOW:
      activeStatus = "Reflow";
      // We need to avoid hovering at peak temperature for too long
      // Crude method that works like a charm and safe for the components
      if (input >= (paste_profile[profileUsed].stages_reflow_1 - 5)) {
        // Set PID parameters for cooling ramp
        reflowOvenPID.SetTunings(PID_KP_REFLOW, PID_KI_REFLOW, PID_KD_REFLOW);
        // Ramp down to minimum cooling temperature
        setpoint = TEMPERATURE_COOL_MIN;
        // Proceed to cooling state
        reflowState = REFLOW_STATE_COOL;
      }
      break;

    case REFLOW_STATE_COOL:
      activeStatus = "Cool";
      // If minimum cool temperature is achieve
      if (input <= TEMPERATURE_COOL_MIN) {
        // Retrieve current time for buzzer usage
        buzzerPeriod = millis() + 1000;
        // Turn on buzzer and green LED to indicate completion
        digitalWrite(RGB_LED_B, HIGH);
        // Turn off reflow process
        reflowStatus = REFLOW_STATUS_OFF;
        // Proceed to reflow Completion state
        reflowState = REFLOW_STATE_COMPLETE;
      }
      break;

    case REFLOW_STATE_COMPLETE:
      activeStatus = "Complete";
      if (millis() > buzzerPeriod) {
        // Turn off buzzer and green LED
        digitalWrite(RGB_LED_B, LOW);
        digitalWrite(RGB_LED_G, HIGH);
        // Reflow process ended
        reflowState = REFLOW_STATE_IDLE;
        profileIsOn = 0;
        disableMenu = 0;
        Serial.println("Profile is OFF");
      }
      break;

    case REFLOW_STATE_TOO_HOT:
      // If oven temperature drops below room temperature
      if (input < TEMPERATURE_ROOM) {
        // Ready to reflow
        reflowState = REFLOW_STATE_IDLE;
      }
      break;

    case REFLOW_STATE_ERROR:
      // If thermocouple problem is still present
      if (input == -999.0) {
        // Wait until thermocouple wire is connected
        reflowState = REFLOW_STATE_ERROR;
      } else {
        // Clear to perform reflow process
        reflowState = REFLOW_STATE_IDLE;
      }
      break;
  }

  // Touch interface handles stop button - this logic is now in UIManager callbacks
  // if (switchStatus == SWITCH_1) {
  //   // If currently reflow process is on going
  //   if (reflowStatus == REFLOW_STATUS_ON) {
  //     // Button press is for cancelling
  //     // Turn off reflow process
  //     reflowStatus = REFLOW_STATUS_OFF;
  //     // Reinitialize state machine
  //     reflowState = REFLOW_STATE_IDLE;
  //   }
  // }

  // Physical button debounce logic is no longer needed - touch interface handles this
  // switch (debounceState) {
  //   case DEBOUNCE_STATE_IDLE:
  //     // No valid switch press
  //     switchStatus = SWITCH_NONE;
  //     // If switch #1 is pressed
  //     AXIS_X.readAxis();
  //     if (AXIS_X.wasAxisPressed() == 1) {
  //       // Intialize debounce counter
  //       lastDebounceTime = millis();
  //       Serial.println("Switch pressed");
  //       // Proceed to check validity of button press
  //       debounceState = DEBOUNCE_STATE_CHECK;
  //     }
  //     break;

  //   case DEBOUNCE_STATE_CHECK:
  //     AXIS_X.readAxis();
  //     if (AXIS_X.wasAxisPressed() == 1) {
  //       // If minimum debounce period is completed
  //       if ((millis() - lastDebounceTime) > DEBOUNCE_PERIOD_MIN) {
  //         // Proceed to wait for button release
  //         debounceState = DEBOUNCE_STATE_RELEASE;
  //     }
  //     } else {
  //       // Reinitialize button debounce state machine
  //       debounceState = DEBOUNCE_STATE_IDLE;
  //     }
  //     break;

  //   case DEBOUNCE_STATE_RELEASE:
  //     AXIS_X.readAxis();
  //     if (AXIS_X.wasAxisPressed() > 0) {
  //       // Valid switch 1 press
  //       switchStatus = SWITCH_1;
  //       // Reinitialize button debounce state machine
  //       debounceState = DEBOUNCE_STATE_IDLE;
  //     }
  //     break;
  // }

  // PID computation and SSR control
  if (reflowStatus == REFLOW_STATUS_ON) {
    now = millis();
    reflowOvenPID.Compute();
    if ((now - windowStartTime) > windowSize) {
      // Time to shift the Relay Window
      windowStartTime += windowSize;
    }
    if (output > (now - windowStartTime)) digitalWrite(SSR_PIN, HIGH);
    else digitalWrite(SSR_PIN, LOW);
  } else {
    // Reflow oven process is off, ensure oven is off
    digitalWrite(SSR_PIN, LOW);
  }
}