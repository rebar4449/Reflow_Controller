#include "LCD.h"

// Constructor
LCD::LCD(TFT_eSPI& tft) : display(tft) {
    // Initialize member variables
    state = 0;
    previousState = 0;
    settings_pointer = 0;
    previousSettingsPointer = 0;
    numOfPointers = 0;
    activeMenu = 0;
    
    // Initialize configuration flags
    horizontal = false;
    buzzer = false;
    buttons = false;
    fan = false;
    useOTA = false;
    useSPIFFS = false;
    updataAvailable = false;
    disableMenu = false;
    connected = false;
    isFault = false;
    testState = false;
    
    // Initialize data
    inputInt = 0;
    activeStatus = "Ready";
    fwVersion = "1.0.0";
    profileUsed = 0;
    profileNum = 0;
    paste_profile = nullptr;
    
    // Initialize pins
    pinMode(buzzerPin, OUTPUT);
    pinMode(fanPin, OUTPUT);
    pinMode(ssrPin, OUTPUT);
    pinMode(ledPin, OUTPUT);
    
    digitalWrite(buzzerPin, LOW);
    digitalWrite(fanPin, LOW);
    digitalWrite(ssrPin, LOW);
    digitalWrite(ledPin, LOW);
}

// Destructor
LCD::~LCD() {
    // Clean up if needed
}

// Helper method to convert String to char*
char* LCD::string2char(String command) {
    if (command.length() != 0) {
        char *p = const_cast<char*>(command.c_str());
        return p;
    }
    return nullptr;
}

// Text rendering methods
void LCD::centeredText(String text, uint16_t color, int yCord, int xCord) {
    int16_t x1, y1;
    uint16_t w, h;
    int offSet = 10;
    display.getTextBounds(string2char(text), 0, 0, &x1, &y1, &w, &h);
#ifdef DEBUG
    Serial.print("Text bounds for: \"" + text + "\"");
    Serial.print(" x1:");  Serial.print(x1);
    Serial.print(" y1:");  Serial.print(y1);
    Serial.print(" w:");  Serial.print(w);
    Serial.print(" h:");  Serial.println(h);
#endif
    display.setTextColor(color);
    display.fillRect(((display.width() - w) / 2), (yCord - (h / 2)), (w + offSet) , (h + offSet), ILI9341_BLACK);
    display.setCursor(((display.width() - w) / 2), (yCord + (h / 2)));
    display.println(text);
}

void LCD::rightText(String text, uint16_t color, int yCord, int xCord) {
    int16_t x1, y1;
    uint16_t w, h;
    int offSet = 10;
    display.getTextBounds(string2char(text), 0, 0, &x1, &y1, &w, &h);
#ifdef DEBUG
    Serial.print("Text bounds for: \"" + text + "\"");
    Serial.print(" x1:");  Serial.print(x1);
    Serial.print(" y1:");  Serial.print(y1);
    Serial.print(" w:");  Serial.print(w);
    Serial.print(" h:");  Serial.println(h);
#endif
    display.setTextColor(color);
    display.setCursor(((display.width() - w) - 10), yCord);
    display.println(text);
}

void LCD::leftText(String text, uint16_t color, int yCord, int xCord) {
    int16_t x1, y1;
    uint16_t w, h;
    int offSet = 10;
    display.getTextBounds(string2char(text), 0, 0, &x1, &y1, &w, &h);
#ifdef DEBUG
    Serial.print("Text bounds for: \"" + text + "\"");
    Serial.print(" x1:");  Serial.print(x1);
    Serial.print(" y1:");  Serial.print(y1);
    Serial.print(" w:");  Serial.print(w);
    Serial.print(" h:");  Serial.println(h);
#endif
    display.setTextColor(color);
    display.setCursor(xCord + 30, yCord);
    display.print(text);
}

void LCD::ShowMenuOptions(bool clearAll) {
    int buttonPosY[] = { 45, 100, 145, 200 };
    int buttonHeight = 16;
    int buttonWidth = 4;

    display.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    display.setTextSize(2);

    if (state >= 1 || state <= 9) {
        UpdateSettingsPointer();
    }
}

void LCD::UpdateSettingsPointer() {
    if (state == 1) { // main menu
        display.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
        display.setTextSize(1);
        display.fillRect(0, 40, 20, display.height() - 20, ILI9341_BLACK);
        display.setCursor(10, (54 + (20 * settings_pointer)));
        display.println(">");

        display.setTextSize(0);
        display.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
        display.fillRect(0, display.height() - 40, display.width(), 40, ILI9341_BLACK);
        switch (settings_pointer) {
            case 0:
                if (disableMenu != 0) {
                    centeredText("Show info menu", ILI9341_GREEN, 300);
                } else {
                    centeredText("Select which profile to reflow", ILI9341_GREEN, 300);
                }
                break;
            case 1:
                centeredText("Change selected profile", ILI9341_GREEN, 300);
                break;
            case 2:
                centeredText("Add reflow profile", ILI9341_GREEN, 300);
                break;
            case 3:
                centeredText("Show settings menu", ILI9341_GREEN, 300);
                break;
            case 4:
                centeredText("Show info menu", ILI9341_GREEN, 300);
                break;
            case 5:
                if (updataAvailable == 1) {
                    centeredText("Press Select to start update", ILI9341_GREEN, 300);
                }
                break;
        }
        display.setTextSize(2);
        previousSettingsPointer = settings_pointer;
    }
    else if (state == 2) { // select profile
        display.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
        display.setTextSize(1);
        display.fillRect(0, 40, 20, display.height() - 20, ILI9341_BLACK);
        display.setCursor(10, (54 + (40 * settings_pointer)));
        display.println(">");

        display.setTextSize(0);
        display.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
        display.fillRect(0, display.height() - 50, display.width(), 40, ILI9341_BLACK);
        switch (settings_pointer) {
            case 0:
                centeredText("Set buzzer on/off.", ILI9341_GREEN, 300);
                break;
        }
    }
    else if (state == 5) { //settings menu
        display.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
        display.setTextSize(1);
        display.fillRect(0, 40, 20, display.height() - 20, ILI9341_BLACK);
        display.setCursor(10, (54 + (20 * settings_pointer)));
        display.println(">");

        display.setTextSize(0);
        display.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
        display.fillRect(0, display.height() - 50, display.width(), 40, ILI9341_BLACK);
        switch (settings_pointer) {
            case 0:
                centeredText("Set buzzer on/off.", ILI9341_GREEN, 300);
                break;
            case 1:
                centeredText("Orientation of the screen.", ILI9341_GREEN, 300);
                break;
            case 2:
                centeredText("Set Yes if you want to", ILI9341_GREEN, 280);
                centeredText("use FW updates from web.", ILI9341_GREEN, 300);
                break;
            case 3:
                centeredText("Set Yes if you have them.", ILI9341_GREEN, 300);
                break;
            case 4:
                centeredText("Set Yes to use SPIFFS", ILI9341_GREEN, 280);
                centeredText("for profiles.", ILI9341_GREEN, 300);
                break;
            case 5:
                if (buttons != 0) {
                    centeredText("Set Yes to use fan.", ILI9341_GREEN, 300);
                } else {
                    centeredText("WiFi Setup", ILI9341_GREEN, 300);
                }
                break;
            case 6:
                if (buttons != 0) {
                    centeredText("WiFi Setup", ILI9341_GREEN, 300);
                } else {
                    centeredText("Enter Test menu.", ILI9341_GREEN, 300);
                }
                break;
            case 7:
                if (buttons != 0) {
                    centeredText("Enter Test menu.", ILI9341_GREEN, 300);
                }
                break;
        }
    }
    else if (state == 9) { //settings/test menu
        display.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
        display.setTextSize(1);
        display.fillRect(0, 40, 20, display.height() - 20, ILI9341_BLACK);
        display.setCursor(10, (54 + (20 * settings_pointer)));
        display.println(">");

        display.setTextSize(0);
        display.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
        display.fillRect(0, display.height() - 50, display.width(), 40, ILI9341_BLACK);
        switch (settings_pointer) {
            case 0:
                centeredText("Test buzzer.", ILI9341_GREEN, 300);
                break;
            case 1:
                centeredText("Test fan.", ILI9341_GREEN, 300);
                break;
            case 2:
                centeredText("Test SSR.", ILI9341_GREEN, 300);
                break;
            case 3:
                centeredText("Test LED.", ILI9341_GREEN, 300);
                break;
        }
    }
}

// Main menu processing
void LCD::processMenu() {
    if (state == 0) { // home screen
        return;
    } else if (state == 1) { // main menu
        return;
    } else if (state == 2) { // select profile
        // Implementation for select profile
    } else if (state == 3) { // change profile
        // Implementation for change profile
    } else if (state == 4) { // add profile
        // Implementation for add profile
    } else if (state == 5) { // settings
        // Implementation for settings
    } else if (state == 6) { // info
        // Implementation for info
    }
}

// Screen display methods
void LCD::startScreen() {
    display.fillScreen(ILI9341_BLACK);
    int y;
    if (horizontal != 0) {
        display.setRotation(3);
        y = 40;
        display.setFont(&FreeSans9pt7b);
        display.setTextSize(2);
        centeredText("ESP32", ILI9341_WHITE, y);
        centeredText("Reflow", ILI9341_WHITE, y + 32);
        centeredText("Controller", ILI9341_WHITE, y + 64);
        display.setTextSize(1);
        centeredText("by", ILI9341_WHITE, y + 134);
        centeredText("Czech maker", ILI9341_WHITE, y + 154);
        centeredText("www.czechmaker.com", ILI9341_WHITE, y + 174);
    } else {
        display.setRotation(2);
        y = 100;
        display.setFont(&FreeSans9pt7b);
        display.setTextSize(2);
        centeredText("ESP32", ILI9341_WHITE, y);
        centeredText("Reflow", ILI9341_WHITE, y + 32);
        centeredText("Controller", ILI9341_WHITE, y + 64);
        display.setTextSize(1);
        centeredText("by", ILI9341_WHITE, y + 164);
        centeredText("Czech maker", ILI9341_WHITE, y + 184);
        centeredText("www.czechmaker.com", ILI9341_WHITE, y + 204);
    }
    delay(2000);
    loopScreen();
}

void LCD::loopScreen() {
    state = 0;
#ifdef DEBUG
    Serial.println("State is :" + String(state));
#endif
    display.fillScreen(ILI9341_BLACK);

    display.setFont(&FreeSans9pt7b);
    display.setTextSize(1);
    if (updataAvailable != 0) {
        centeredText("!UPDATE AVAILABLE!", ILI9341_GREEN, 10);
    }
    if (horizontal != 0) {
        int tempTextPos = 180;
        int infoText = 30;
        centeredText("Status:", ILI9341_WHITE, infoText);
        display.setTextSize(2);
        centeredText(activeStatus, ILI9341_WHITE, infoText + 32);
        String temp = ("Temp : " + String(inputInt));

        if (isFault != 0) {
            display.setTextSize(1);
            centeredText("Thermocouple error", ILI9341_RED, tempTextPos);
        } else if (inputInt == 1372) {
            display.setTextSize(1);
            centeredText("Thermocouple not", ILI9341_RED, tempTextPos);
            centeredText("connected", ILI9341_RED, tempTextPos + 20);
        } else if (inputInt < 50) {
            centeredText(temp, ILI9341_GREEN, tempTextPos);
        } else if ((inputInt > 50) && (inputInt < 100)) {
            centeredText(temp, ILI9341_ORANGE, tempTextPos);
        } else if (inputInt > 100) {
            centeredText(temp, ILI9341_RED, tempTextPos);
        }
        display.setTextSize(1);
        if (paste_profile != nullptr) {
            centeredText("Profile in use:" + String(paste_profile[profileUsed].title), ILI9341_WHITE, tempTextPos + 50);
        }
#ifdef DEBUG
        Serial.println(temp);
#endif
    } else {
        int tempTextPos = 240;
        int infoText = 50;
        centeredText("Status:", ILI9341_WHITE, infoText);
        display.setTextSize(2);
        centeredText(activeStatus, ILI9341_WHITE, infoText + 32);
        String temp = ("Temp : " + String(inputInt));

        if (isFault != 0) {
            display.setTextSize(1);
            centeredText("Thermocouple error", ILI9341_RED, tempTextPos);
        } else if (inputInt == 1372) {
            display.setTextSize(1);
            centeredText("Thermocouple not", ILI9341_RED, tempTextPos);
            centeredText("connected", ILI9341_RED, tempTextPos + 20);
        } else if (inputInt < 50) {
            centeredText(temp, ILI9341_GREEN, tempTextPos);
        } else if ((inputInt > 50) && (inputInt < 100)) {
            centeredText(temp, ILI9341_ORANGE, tempTextPos);
        } else if (inputInt > 100) {
            centeredText(temp, ILI9341_RED, tempTextPos);
        }
        display.setTextSize(1);
        if (paste_profile != nullptr) {
            centeredText("Profile in use:" + String(paste_profile[profileUsed].title), ILI9341_WHITE, tempTextPos + 50);
        }
#ifdef DEBUG
        Serial.println(temp);
#endif
    }
}

void LCD::mainMenuScreen() {
    previousState = state;
    state = 1;
    numOfPointers = 0;
#ifdef DEBUG
    Serial.println("State is :" + String(state));
#endif
    int y = 55;
    int h = 20;
    display.setFont(&FreeSans9pt7b);
    display.fillScreen(ILI9341_BLACK);
    display.setTextColor(ILI9341_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 4);
    centeredText("Main menu", ILI9341_WHITE, 10);
    if (horizontal != 0) {
        display.fillRect(0, 28, 320, 3, ILI9341_WHITE);
    } else {
        display.fillRect(0, 28, 240, 3, ILI9341_WHITE);
    }
    display.setCursor(10, y);
    if (disableMenu != 1) {
        leftText("Select profile", ILI9341_WHITE, y); rightText("->", ILI9341_WHITE, y);
        y += h;
        numOfPointers++;
        leftText("Change profile", ILI9341_WHITE, y); rightText("->", ILI9341_WHITE, y);
        y += h;
        numOfPointers++;
        leftText("Add profile", ILI9341_WHITE, y); rightText("->", ILI9341_WHITE, y);
        y += h;
        numOfPointers++;
        leftText("Settings", ILI9341_WHITE, y); rightText("->", ILI9341_WHITE, y);
        y += h;
        numOfPointers++;
    }
    leftText("Info", ILI9341_WHITE, y); rightText("->", ILI9341_WHITE, y);

    if (updataAvailable == 1) {
        y += h;
        numOfPointers++;
        leftText("Update firmware", ILI9341_GREEN, y);
    }

    ShowMenuOptions(true);
}

void LCD::showSelectProfile() {
    previousState = state;
    state = 2;
    int y = 55;
    int h = 20;
    numOfPointers = profileNum;
#ifdef DEBUG
    Serial.println("State is :" + String(state));
#endif
    display.fillScreen(ILI9341_BLACK);
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(ILI9341_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 4);
    centeredText("Select profile menu", ILI9341_WHITE, 10);
    if (horizontal != 0) {
        display.fillRect(0, 28, 320, 3, ILI9341_WHITE);
    } else {
        display.fillRect(0, 28, 240, 3, ILI9341_WHITE);
    }
    if (paste_profile != nullptr) {
        for (int i = 0; i < profileNum; i++) {
            if (i == profileUsed) {
                leftText(paste_profile[i].title, ILI9341_YELLOW, y);
            } else {
                leftText(paste_profile[i].title, ILI9341_WHITE, y);
            }
            y += h;
            leftText(paste_profile[i].alloy, ILI9341_BLUE, y, +15);
            y += h;
        }
    }

    ShowMenuOptions(true);
}

void LCD::showChangeProfile() {
    previousState = state;
    state = 3;
#ifdef DEBUG
    Serial.println("State is :" + String(state));
#endif
    int y = 55;
    int h = 20;
    display.setFont(&FreeSans9pt7b);
    display.fillScreen(ILI9341_BLACK);
    display.setTextColor(ILI9341_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 4);
    centeredText("Change profile menu", ILI9341_WHITE, 10);
    if (horizontal != 0) {
        display.fillRect(0, 28, 320, 3, ILI9341_WHITE);
    } else {
        display.fillRect(0, 28, 240, 3, ILI9341_WHITE);
    }
}

void LCD::showAddProfile() {
    previousState = state;
    state = 4;
#ifdef DEBUG
    Serial.println("State is :" + String(state));
#endif
    int y = 55;
    int h = 20;
    display.setFont(&FreeSans9pt7b);
    display.fillScreen(ILI9341_BLACK);
    display.setTextColor(ILI9341_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 4);
    centeredText("Add profile menu", ILI9341_WHITE, 10);
    if (horizontal != 0) {
        display.fillRect(0, 28, 320, 3, ILI9341_WHITE);
    } else {
        display.fillRect(0, 28, 240, 3, ILI9341_WHITE);
    }
}

void LCD::wifiSetupShow(int y) {
    display.fillRect(30, y - 18, 200, 20, ILI9341_BLACK);
    display.setTextColor(ILI9341_WHITE);
    display.setTextSize(1);
    display.setCursor(30, y);
    display.print("Setup WiFi");
}

void LCD::showSettings(byte pointer) {
    previousState = state;
    state = 5;
    numOfPointers = 0;
    settings_pointer = 0;
#ifdef DEBUG
    Serial.println("State is :" + String(state));
#endif
    int y = 55;
    int h = 20;
    display.setFont(&FreeSans9pt7b);
    display.fillScreen(ILI9341_BLACK);
    display.setTextColor(ILI9341_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 4);
    centeredText("Settings menu", ILI9341_WHITE, 10);
    if (horizontal != 0) {
        display.fillRect(0, 28, 320, 3, ILI9341_WHITE);
    } else {
        display.fillRect(0, 28, 240, 3, ILI9341_WHITE);
    }
    display.setCursor(30, y);

    setBuzzer(y);
    y += h;
    numOfPointers++;

    setDisplay(y);
    y += h;
    numOfPointers++;

    setOTA(y);
    y += h;
    numOfPointers++;

    setButtons(y);
    y += h;
    numOfPointers++;

    setStorage(y);
    y += h;
    numOfPointers++;

    if (buttons != 0) {
        setFan(y);
        y += h;
        numOfPointers++;
    }

    wifiSetupShow(y);
    y += h;
    numOfPointers++;

    leftText("Test outputs", ILI9341_WHITE, y); rightText("->", ILI9341_WHITE, y);

    ShowMenuOptions(true);
}

void LCD::showInfo() {
    previousState = state;
    state = 6;
#ifdef DEBUG
    Serial.println("State is :" + String(state));
#endif
    int y = 55;
    int h = 20;
    display.setFont(&FreeSans9pt7b);
    display.fillScreen(ILI9341_BLACK);
    display.setTextColor(ILI9341_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 4);
    centeredText("Info menu", ILI9341_WHITE, 10);
    if (horizontal != 0) {
        display.fillRect(0, 28, 320, 3, ILI9341_WHITE);
    } else {
        display.fillRect(0, 28, 240, 3, ILI9341_WHITE);
    }
    leftText("- FW version: " + fwVersion, ILI9341_WHITE, y, -15);
    if (connected != 0) {
        String ipAddress = WiFi.localIP().toString();
        leftText("- WiFi: " + String(WiFi.SSID()), ILI9341_WHITE, y + h, -15);
        leftText("- IP: " + ipAddress, ILI9341_WHITE, y + h * 2, -15);
    }
}

void LCD::infoScreen() {
    display.fillScreen(ILI9341_BLACK);
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(ILI9341_WHITE);
    display.setTextSize(1);
    display.setCursor(5, 15);
    display.print("IP: ");
    display.println(WiFi.localIP());
    display.setCursor(5, 315);
    display.print("FW: ");
    display.println(fwVersion);
}

void LCD::startReflowScreen() {
    previousState = state;
    state = 7;
#ifdef DEBUG
    Serial.println("State is :" + String(state));
#endif
    display.fillScreen(ILI9341_BLACK);
    int tempTextPos = 240;
    int infoText = 50;
    display.setFont(&FreeSans9pt7b);
    display.setTextSize(2);
    centeredText("Start reflow?", ILI9341_RED, infoText);
    display.setTextSize(1);
    centeredText("Yes = Select", ILI9341_WHITE, infoText + 32);
    centeredText("No = Back", ILI9341_WHITE, infoText + 64);
}

void LCD::stopReflowScreen() {
    previousState = state;
    state = 8;
#ifdef DEBUG
    Serial.println("State is :" + String(state));
#endif
    display.fillScreen(ILI9341_BLACK);
    int tempTextPos = 240;
    int infoText = 50;
    display.setFont(&FreeSans9pt7b);
    display.setTextSize(2);
    centeredText("Stop reflow?", ILI9341_RED, infoText);
    display.setTextSize(1);
    centeredText("Yes = Select", ILI9341_WHITE, infoText + 32);
    centeredText("No = Back", ILI9341_WHITE, infoText + 64);
}

void LCD::startUpdateScreen() {
    display.fillScreen(ILI9341_BLACK);
    int y;
    if (horizontal != 0) {
        display.setRotation(3);
        y = 40;
        display.setFont(&FreeSans9pt7b);
        display.setTextSize(1);
        centeredText("Update started.It may", ILI9341_GREEN, y);
        centeredText("take 2 - 5 mins to complete.", ILI9341_GREEN, y + 20);
    } else {
        display.setRotation(2);
        y = 100;
        display.setFont(&FreeSans9pt7b);
        display.setTextSize(1);
        centeredText("Update started.It may", ILI9341_GREEN, y);
        centeredText("take 2 - 5 mins to complete.", ILI9341_GREEN, y + 20);
    }
}

void LCD::updateOK() {
    display.fillScreen(ILI9341_BLACK);
    int y;
    if (horizontal != 0) {
        display.setRotation(3);
        y = 40;
        display.setFont(&FreeSans9pt7b);
        display.setTextSize(1);
        centeredText("Update successfully", ILI9341_GREEN, y);
        centeredText("completed. Rebooting.", ILI9341_GREEN, y + 32);
    } else {
        display.setRotation(2);
        y = 100;
        display.setFont(&FreeSans9pt7b);
        display.setTextSize(1);
        centeredText("Update successfully", ILI9341_GREEN, y);
        centeredText("completed. Rebooting.", ILI9341_GREEN, y + 32);
    }
    delay(2000);
}

void LCD::testOutputs() {
    previousState = state;
    state = 9;
    numOfPointers = 0;
    settings_pointer = 0;
#ifdef DEBUG
    Serial.println("State is :" + String(state));
#endif
    int y = 55;
    int h = 20;
    display.setFont(&FreeSans9pt7b);
    display.fillScreen(ILI9341_BLACK);
    display.setTextColor(ILI9341_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 4);
    centeredText("Test outputs menu", ILI9341_WHITE, 10);
    if (horizontal != 0) {
        display.fillRect(0, 28, 320, 3, ILI9341_WHITE);
    } else {
        display.fillRect(0, 28, 240, 3, ILI9341_WHITE);
    }
    display.setCursor(30, y);

    testBuzzer(y);
    y += h;
    numOfPointers++;

    testFan(y);
    y += h;
    numOfPointers++;

    testSSR(y);
    y += h;
    numOfPointers++;

    testLED(y);

    ShowMenuOptions(true);
}

// Settings display methods
void LCD::setBuzzer(int y) {
    display.fillRect(30, y - 18, 200, 20, ILI9341_BLACK);
    display.setTextColor(ILI9341_WHITE);
    display.setTextSize(1);
    display.setCursor(30, y);
    display.print("Use buzzer: ");
    if (buzzer != 0) {
        display.println("Yes");
    } else {
        display.println("No");
    }
}

void LCD::setButtons(int y) {
    display.fillRect(30, y - 18, 200, 20, ILI9341_BLACK);
    display.setTextColor(ILI9341_WHITE);
    display.setTextSize(1);
    display.setCursor(30, y);
    display.print("Menu/Back buttons: ");
    if (buttons != 0) {
        display.println("Yes");
    } else {
        display.println("No");
    }
}

void LCD::setFan(int y) {
    display.fillRect(30, y - 18, 200, 20, ILI9341_BLACK);
    display.setTextColor(ILI9341_WHITE);
    display.setTextSize(1);
    display.setCursor(30, y);
    display.print("Use fan: ");
    if (fan != 0) {
        display.println("Yes");
    } else {
        display.println("No");
    }
}

void LCD::setDisplay(int y) {
    display.fillRect(30, y - 18, 200, 20, ILI9341_BLACK);
    display.setTextColor(ILI9341_WHITE);
    display.setTextSize(1);
    display.setCursor(30, y);
    display.print("Orientation: ");
    if (horizontal != 0) {
        display.println("Horizontal");
    } else {
        display.println("Vertical");
    }
}

void LCD::setOTA(int y) {
    display.fillRect(30, y - 18, 200, 20, ILI9341_BLACK);
    display.setTextColor(ILI9341_WHITE);
    display.setTextSize(1);
    display.setCursor(30, y);
    display.print("Update FW by OTA: ");
    if (useOTA != 0) {
        display.println("Yes");
    } else {
        display.println("No");
    }
}

void LCD::setStorage(int y) {
    display.fillRect(30, y - 18, 200, 20, ILI9341_BLACK);
    display.setTextColor(ILI9341_WHITE);
    display.setTextSize(1);
    display.setCursor(30, y);
    display.print("Use SPIFFS: ");
    if (useSPIFFS != 0) {
        display.println("Yes");
    } else {
        display.println("No");
    }
}

// Test methods
void LCD::testBuzzer(int y) {
    display.fillRect(30, y - 18, 200, 20, ILI9341_BLACK);
    display.setTextColor(ILI9341_WHITE);
    display.setTextSize(1);
    display.setCursor(30, y);
    display.print("Test buzzer: ");
    if (testState != LOW) {
        display.println("On");
        digitalWrite(buzzerPin, HIGH);
    } else {
        display.println("Off");
        digitalWrite(buzzerPin, LOW);
    }
}

void LCD::testFan(int y) {
    display.fillRect(30, y - 18, 200, 20, ILI9341_BLACK);
    display.setTextColor(ILI9341_WHITE);
    display.setTextSize(1);
    display.setCursor(30, y);
    display.print("Test fan: ");
    if (testState != LOW) {
        display.println("On");
        digitalWrite(fanPin, HIGH);
    } else {
        display.println("Off");
        digitalWrite(fanPin, LOW);
    }
}

void LCD::testSSR(int y) {
    display.fillRect(30, y - 18, 200, 20, ILI9341_BLACK);
    display.setTextColor(ILI9341_WHITE);
    display.setTextSize(1);
    display.setCursor(30, y);
    display.print("Test SSR: ");
    if (testState != LOW) {
        display.println("On");
        digitalWrite(ssrPin, HIGH);
    } else {
        display.println("Off");
        digitalWrite(ssrPin, LOW);
    }
}

void LCD::testLED(int y) {
    display.fillRect(30, y - 18, 200, 20, ILI9341_BLACK);
    display.setTextColor(ILI9341_WHITE);
    display.setTextSize(1);
    display.setCursor(30, y);
    display.print("Test LED: ");
    if (testState != LOW) {
        display.println("On");
        digitalWrite(ledPin, HIGH);
    } else {
        display.println("Off");
        digitalWrite(ledPin, LOW);
    }
}

// Configuration methods
void LCD::setHorizontal(bool horizontal) { this->horizontal = horizontal; }
void LCD::setBuzzerEnabled(bool enabled) { this->buzzer = enabled; }
void LCD::setButtonsEnabled(bool enabled) { this->buttons = enabled; }
void LCD::setFanEnabled(bool enabled) { this->fan = enabled; }
void LCD::setOTAEnabled(bool enabled) { this->useOTA = enabled; }
void LCD::setSPIFFSEnabled(bool enabled) { this->useSPIFFS = enabled; }
void LCD::setUpdateAvailable(bool available) { this->updataAvailable = available; }
void LCD::setMenuDisabled(bool disabled) { this->disableMenu = disabled; }
void LCD::setConnected(bool connected) { this->connected = connected; }
void LCD::setFault(bool fault) { this->isFault = fault; }
void LCD::setTestState(bool state) { this->testState = state; }

// Data setting methods
void LCD::setInputInt(int value) { this->inputInt = value; }
void LCD::setActiveStatus(String status) { this->activeStatus = status; }
void LCD::setFwVersion(String version) { this->fwVersion = version; }
void LCD::setProfileUsed(int profile) { this->profileUsed = profile; }
void LCD::setProfileNum(int num) { this->profileNum = num; }
void LCD::setProfiles(ReflowProfile* profiles) { this->paste_profile = profiles; }

// Getter methods
int LCD::getState() const { return state; }
int LCD::getSettingsPointer() const { return settings_pointer; }
int LCD::getNumOfPointers() const { return numOfPointers; }

// Configuration getter methods
bool LCD::getHorizontal() const { return horizontal; }
bool LCD::getBuzzerEnabled() const { return buzzer; }
bool LCD::getButtonsEnabled() const { return buttons; }
bool LCD::getFanEnabled() const { return fan; }
bool LCD::getOTAEnabled() const { return useOTA; }
bool LCD::getSPIFFSEnabled() const { return useSPIFFS; }
bool LCD::getUpdateAvailable() const { return updataAvailable; }
bool LCD::getMenuDisabled() const { return disableMenu; }
bool LCD::getConnected() const { return connected; }
bool LCD::getFault() const { return isFault; }
bool LCD::getTestState() const { return testState; }

// State management
void LCD::setState(int newState) { this->state = newState; }
void LCD::setSettingsPointer(int pointer) { this->settings_pointer = pointer; }
void LCD::setNumOfPointers(int num) { this->numOfPointers = num; }
