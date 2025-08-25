#include "ButtonNav.h"

// Global instance
ButtonHandler buttonHandler;

// Constructor
ButtonHandler::ButtonHandler() {
    // Initialize all member variables
    numDigButtons = 0;
    debounce = DEBOUNCE_TIME;
    holdTime = HOLD_TIME;
    analogLastDebounceTime = 0;
    
    // Initialize arrays
    for (int i = 0; i < MAX_DIGITAL_BUTTONS; i++) {
        digitalButtonPins[i] = 0;
        buttonVal[i] = LOW;
        buttonLast[i] = LOW;
        btnDnTime[i] = 0;
        btnUpTime[i] = 0;
        ignoreUp[i] = false;
        menuMode[i] = false;
    }
    
    // Initialize callback pointers to null
    event1Callback = nullptr;
    event2Callback = nullptr;
    
    // Initialize external dependencies to null
    AXIS_X = nullptr;
    AXIS_Y = nullptr;
    
    // Initialize configuration variables
    verboseOutput = 0;
    horizontal = 0;
    buttons = 0;
    state = 0;
    previousState = 0;
    settings_pointer = 0;
    previousSettingsPointer = 0;
    numOfPointers = 0;
    profileIsOn = 0;
    disableMenu = 0;
    buzzer = 0;
    useOTA = 0;
    useSPIFFS = 0;
    fan = 0;
    testState = LOW;
    activeStatus = "Idle";
    
    // Initialize all callback function pointers to null
    startReflowScreen = nullptr;
    stopReflowScreen = nullptr;
    mainMenuScreen = nullptr;
    loopScreen = nullptr;
    showSelectProfile = nullptr;
    showChangeProfile = nullptr;
    showAddProfile = nullptr;
    showSettings = nullptr;
    showInfo = nullptr;
    updateFirmware = nullptr;
    saveSelectedProfile = nullptr;
    UpdateSettingsPointer = nullptr;
    setBuzzer = nullptr;
    setDisplay = nullptr;
    setOTA = nullptr;
    setButtons = nullptr;
    setStorage = nullptr;
    setFan = nullptr;
    updatePreferences = nullptr;
    wifiSetup = nullptr;
    testOutputs = nullptr;
    testBuzzer = nullptr;
    testFan = nullptr;
    testSSR = nullptr;
    testLED = nullptr;
    startScreen = nullptr;
}

void ButtonHandler::begin(const int* pins, int numButtons, unsigned long debounceTime, unsigned long holdTimeMs) {
    numDigButtons = min(numButtons, MAX_DIGITAL_BUTTONS);
    debounce = debounceTime;
    holdTime = holdTimeMs;
    
    // Copy pin numbers
    for (int i = 0; i < numDigButtons; i++) {
        digitalButtonPins[i] = pins[i];
        pinMode(digitalButtonPins[i], INPUT_PULLUP);
    }
}

void ButtonHandler::setJoystickAxes(JoystickAxis* axisX, JoystickAxis* axisY) {
    AXIS_X = axisX;
    AXIS_Y = axisY;
}

void ButtonHandler::setConfiguration(int verbose, int horiz, int btn, int st, int prevSt, int setPtr, int prevSetPtr, int numPtr, int profile, int disable, int bzr, int ota, int spiffs, int fn, int testSt, String status) {
    verboseOutput = verbose;
    horizontal = horiz;
    buttons = btn;
    state = st;
    previousState = prevSt;
    settings_pointer = setPtr;
    previousSettingsPointer = prevSetPtr;
    numOfPointers = numPtr;
    profileIsOn = profile;
    disableMenu = disable;
    buzzer = bzr;
    useOTA = ota;
    useSPIFFS = spiffs;
    fan = fn;
    testState = testSt;
    activeStatus = status;
}

void ButtonHandler::setEvent1Callback(void (*callback)(int pin)) {
    event1Callback = callback;
}

void ButtonHandler::setEvent2Callback(void (*callback)(int pin)) {
    event2Callback = callback;
}

void ButtonHandler::setMainCallbacks(
    void (*startReflow)(), void (*stopReflow)(), void (*mainMenu)(), void (*loop)(),
    void (*selectProfile)(), void (*changeProfile)(), void (*addProfile)(), void (*settings)(),
    void (*info)(), void (*firmware)(), void (*saveProfile)(int), void (*updatePtr)(),
    void (*setBzr)(int), void (*setDisp)(int), void (*setOta)(int), void (*setBtn)(int),
    void (*setStor)(int), void (*setFn)(int), void (*updatePref)(), void (*wifi)(),
    void (*testOut)(), void (*testBzr)(int), void (*testFn)(int), void (*testSsr)(int),
    void (*testLed)(int), void (*startScr)()
) {
    startReflowScreen = startReflow;
    stopReflowScreen = stopReflow;
    mainMenuScreen = mainMenu;
    loopScreen = loop;
    showSelectProfile = selectProfile;
    showChangeProfile = changeProfile;
    showAddProfile = addProfile;
    showSettings = settings;
    showInfo = info;
    updateFirmware = firmware;
    saveSelectedProfile = saveProfile;
    UpdateSettingsPointer = updatePtr;
    setBuzzer = setBzr;
    setDisplay = setDisp;
    setOTA = setOta;
    setButtons = setBtn;
    setStorage = setStor;
    setFan = setFn;
    updatePreferences = updatePref;
    wifiSetup = wifi;
    testOutputs = testOut;
    testBuzzer = testBzr;
    testFan = testFn;
    testSSR = testSsr;
    testLED = testLed;
    startScreen = startScr;
}

byte ButtonHandler::processDigitalButtons() {
    for (int i = 0; i < numDigButtons; i++) {
        // Read the state of the button
        buttonVal[i] = digitalRead(digitalButtonPins[i]);

        // Test for button pressed and store the down time
        if (buttonVal[i] == HIGH && buttonLast[i] == LOW && (millis() - btnDnTime[i]) > debounce) {
            btnUpTime[i] = millis();
        }

        // Test for button release and store the up time
        if (buttonVal[i] == LOW && buttonLast[i] == HIGH && (millis() - btnUpTime[i]) > debounce) {
            if (ignoreUp[i] == false) {
                if (event1Callback) {
                    event1Callback(digitalButtonPins[i]);
                } else {
                    event1(digitalButtonPins[i]);
                }
            } else {
                ignoreUp[i] = false;
            }
            btnDnTime[i] = millis();
        }

        // Test for button held down for longer than the hold time
        if (buttonVal[i] == LOW && menuMode[i] == false && (millis() - btnDnTime[i]) > long(holdTime)) {
            ignoreUp[i] = true;
            btnDnTime[i] = millis();
            menuMode[i] = true;
        }
        
        if (buttonVal[i] == HIGH && buttonLast[i] == LOW && (millis() - btnDnTime[i]) > debounce) {
            if (ignoreUp[i] == false && menuMode[i] == true) {
                if (event2Callback) {
                    event2Callback(digitalButtonPins[i]);
                } else {
                    event2(digitalButtonPins[i]);
                }
            } else {
                ignoreUp[i] = false;
            }
            btnUpTime[i] = millis();
        }
        
        if (buttonVal[i] == LOW && menuMode[i] == true && (millis() - btnDnTime[i]) > long(holdTime)) {
            ignoreUp[i] = true;
            btnDnTime[i] = millis();
            menuMode[i] = false;
        }
        
        buttonLast[i] = buttonVal[i];
    }
    return 0; // Return value for compatibility
}

void ButtonHandler::readAnalogButtons() {
    if (!AXIS_X || !AXIS_Y) {
        return; // No joystick axes configured
    }
    else{
        AXIS_X->readAxis();
        AXIS_Y->readAxis();
        
        if (AXIS_X->wasAxisPressed() == 1) {
            if (horizontal != 0) {
                if (verboseOutput != 0) {
                    Serial.println("Right");
                }
                if (buttons != 1) { // back button if there is not dedicated Back button on board
                    if (state == 1 || state == 7 || state == 8) {
                        if (loopScreen) loopScreen();
                        settings_pointer = 0;
                    } else {
                        if (state > 0) {
                            settings_pointer = previousSettingsPointer;
                            if (mainMenuScreen) mainMenuScreen();
                        }
                    }
                }
            } else {
                if (settings_pointer < numOfPointers) {
                    settings_pointer++;
                    if (UpdateSettingsPointer) UpdateSettingsPointer();
                    if (verboseOutput != 0) {
                        Serial.println("Down; Settings pointer:  " + String(settings_pointer));
                    }
                }
            }
        } else if (AXIS_X->wasAxisPressed() == 2) {
            if (horizontal != 0) {
                if (verboseOutput != 0) {
                    Serial.println("Left");
                }
                if (buttons != 1) {
                    if (state == 0) { // menu button if there is not dedicated Menu button on board
                        if (mainMenuScreen) mainMenuScreen();
                    }
                }
            } else {
                if (settings_pointer > 0) {
                    settings_pointer--;
                    if (UpdateSettingsPointer) UpdateSettingsPointer();
                    if (verboseOutput != 0) {
                        Serial.println("Up; Settings pointer: " + String(settings_pointer));
                    }
                }
            }
        } else if (AXIS_Y->wasAxisPressed() == 1) {
            if (horizontal != 0) {
                if (settings_pointer < numOfPointers) {
                    settings_pointer++;
                    if (UpdateSettingsPointer) UpdateSettingsPointer();
                    if (verboseOutput != 0) {
                        Serial.println("Down; Settings pointer:  " + String(settings_pointer));
                    }
                }
            } else {
                if (verboseOutput != 0) {
                    Serial.println("Left");
                }
                if (buttons != 1) {
                    if (state == 0) { // menu button if there is not dedicated Menu button on board
                        if (mainMenuScreen) mainMenuScreen();
                    }
                }
            }
        } else if (AXIS_Y->wasAxisPressed() == 2) {
            if (horizontal != 0) {
                settings_pointer--;
                if (UpdateSettingsPointer) UpdateSettingsPointer();
                if (verboseOutput != 0) {
                    Serial.println("Up; Settings pointer: " + String(settings_pointer));
                }
            } else {
                if (verboseOutput != 0) {
                    Serial.println("Right");
                }
                if (buttons != 1) { // back button if there is not dedicated Back button on board
                    if (state == 1 || state == 7 || state == 8) {
                        if (loopScreen) loopScreen();
                        settings_pointer = 0;
                    } else {
                        if (state > 0) {
                            settings_pointer = previousSettingsPointer;
                            if (mainMenuScreen) mainMenuScreen();
                        }
                    }
                }
            }
        }
    }
}

void ButtonHandler::event1(int pin) {
    if (pin > 20 && pin < 40) {
        if (pin == 27) { // Select button
            if (verboseOutput != 0) {
                Serial.println("---------------------");
                Serial.println("Previous state is: " + String(previousState));
                Serial.println("State is: " + String(state));
                Serial.println("Settings pointer: " + String(settings_pointer));
                Serial.println("Previous settings pointer: " + String(previousSettingsPointer));
            }
            
            if (state == 0) {
                if (profileIsOn != 0) {
                    if (stopReflowScreen) stopReflowScreen();
                    activeStatus = "Idle";
                } else {
                    if (startReflowScreen) startReflowScreen();
                }
            } else if (state == 1) { // main menu
                if (settings_pointer == 0) {
                    if (disableMenu != 0) {
                        if (showInfo) showInfo();
                    } else {
                        if (showSelectProfile) showSelectProfile();
                    }
                } else if (settings_pointer == 1) {
                    if (showChangeProfile) showChangeProfile();
                } else if (settings_pointer == 2) {
                    if (showAddProfile) showAddProfile();
                } else if (settings_pointer == 3) {
                    if (showSettings) showSettings();
                } else if (settings_pointer == 4) {
                    if (showInfo) showInfo();
                } else if (settings_pointer == 5) {
                    if (updateFirmware) updateFirmware();
                }
            } else if (state == 2) { // select profile
                if (saveSelectedProfile) saveSelectedProfile(settings_pointer);
                if (showSelectProfile) showSelectProfile();
            } else if (state == 5) { // settings
                if (settings_pointer == 0) {
                    buzzer = !buzzer;
                    if (verboseOutput != 0) {
                        Serial.println("Buzzer value is: " + String(buzzer));
                    }
                    if (setBuzzer) setBuzzer(55);
                    if (updatePreferences) updatePreferences();
                } else if (settings_pointer == 1) {
                    horizontal = !horizontal;
                    if (verboseOutput != 0) {
                        Serial.println("Display value is: " + String(horizontal));
                    }
                    if (setDisplay) setDisplay(75);
                    if (updatePreferences) updatePreferences();
                    previousSettingsPointer = settings_pointer = 0;
                    if (startScreen) startScreen();
                } else if (settings_pointer == 2) {
                    useOTA = !useOTA;
                    if (verboseOutput != 0) {
                        Serial.println("Download FW by OTA: " + String(useOTA));
                    }
                    if (setOTA) setOTA(95);
                    if (updatePreferences) updatePreferences();
                } else if (settings_pointer == 3) {
                    buttons = !buttons;
                    if (verboseOutput != 0) {
                        Serial.println("Buttons value is: " + String(buttons));
                    }
                    if (setButtons) setButtons(115);
                    if (updatePreferences) updatePreferences();
                    if (showSettings) showSettings();
                } else if (settings_pointer == 4) {
                    useSPIFFS = !useSPIFFS;
                    if (verboseOutput != 0) {
                        Serial.println("Use SPIFFS: " + String(useSPIFFS));
                    }
                    if (setStorage) setStorage(135);
                    if (updatePreferences) updatePreferences();
                } else if (settings_pointer == 5) {
                    if (buttons != 0) {
                        fan = !fan;
                        if (verboseOutput != 0) {
                            Serial.println("Fan value is: " + String(fan));
                        }
                        if (setFan) setFan(155);
                        if (updatePreferences) updatePreferences();
                    } else {
                        if (testOutputs) testOutputs();
                    }
                } else if (settings_pointer == 6) {
                    if (buttons != 0) {
                        if (verboseOutput != 0) {
                            Serial.println("Calling WiFi setup function");
                        }
                        if (wifiSetup) wifiSetup();
                    } else {
                        if (testOutputs) testOutputs();
                    }
                } else if (settings_pointer == 7) {
                    if (testOutputs) testOutputs();
                }
            } else if (state == 7) {
                profileIsOn = 1;
                Serial.println("Profile is ON");
                disableMenu = 1;
                if (loopScreen) loopScreen();
            } else if (state == 8) {
                profileIsOn = 0;
                disableMenu = 0;
                Serial.println("Profile is OFF");
                // Button press is for cancelling
                // Turn off reflow process
                // Reinitialize state machine
                if (loopScreen) loopScreen();
            } else if (state == 9) {
                if (settings_pointer == 0) {
                    if (testState != LOW) {
                        testState = LOW;
                    } else {
                        testState = HIGH;
                    }
                    if (testBuzzer) testBuzzer(55);
                } else if (settings_pointer == 1) {
                    if (testState != LOW) {
                        testState = LOW;
                    } else {
                        testState = HIGH;
                    }
                    if (testFan) testFan(75);
                } else if (settings_pointer == 2) {
                    if (testState != LOW) {
                        testState = LOW;
                    } else {
                        testState = HIGH;
                    }
                    if (testSSR) testSSR(95);
                } else if (settings_pointer == 3) {
                    if (testState != LOW) {
                        testState = LOW;
                    } else {
                        testState = HIGH;
                    }
                    if (testLED) testLED(115);
                }
            }
            if (verboseOutput != 0) {
                Serial.println("Select");
                Serial.println("---------------------");
            }
        } else if (pin == 32) { // Menu button
            if (state == 0) {
                if (mainMenuScreen) mainMenuScreen();
            }
            if (verboseOutput != 0) {
                Serial.println("Menu");
                Serial.println("State is :" + String(state));
            }
        } else if (pin == 33) { // Back button
            if (state == 1 || state == 7 || state == 8) {
                if (loopScreen) loopScreen();
                settings_pointer = 0;
            } else if (state == 9) {
                settings_pointer = previousSettingsPointer;
                if (showSettings) showSettings();
            } else {
                if (state > 0) {
                    settings_pointer = previousSettingsPointer;
                    if (mainMenuScreen) mainMenuScreen();
                }
            }
            if (verboseOutput != 0) {
                Serial.println("---------------------");
                Serial.println("Back");
                Serial.println("State is :" + String(state));
                Serial.println("Settings pointer: " + String(settings_pointer));
                Serial.println("Previous settings pointer: " + String(previousSettingsPointer));
                Serial.println("---------------------");
            }
        } else if (verboseOutput != 0) {
            Serial.println("button 1 (" + String(pin) + ")");
        }
    }
    delay(50);
}

void ButtonHandler::event2(int pin) {
    if (pin > 20 && pin < 40) {
        Serial.println("button 2 (" + String(pin) + ")");
    }
    delay(50);
}