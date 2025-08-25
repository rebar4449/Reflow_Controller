#include "OTA.h"

OTA::OTA(const String& versionUrl, const String& firmwareUrl, const String& currentVersion) 
    : versionUrl(versionUrl), firmwareUrl(firmwareUrl), currentVersion(currentVersion), updateAvailable(false) {
    // Set default callbacks
    onUpdateStart = defaultUpdateStart;
    onUpdateSuccess = defaultUpdateSuccess;
    onUpdateError = defaultUpdateError;
}

void OTA::checkForUpdate() {
    getVersion();
    parseVersion(currentVersion, serverVersion);
}

void OTA::performUpdate() {
    if (updateAvailable) {
        updateFirmware();
    } else {
        if (onUpdateError) {
            onUpdateError("No update available");
        }
    }
}

bool OTA::isUpdateAvailable() const {
    return updateAvailable;
}

void OTA::setVersionUrl(const String& url) {
    versionUrl = url;
}

void OTA::setFirmwareUrl(const String& url) {
    firmwareUrl = url;
}

void OTA::setCurrentVersion(const String& version) {
    currentVersion = version;
}

void OTA::setUpdateStartCallback(void (*callback)()) {
    onUpdateStart = callback;
}

void OTA::setUpdateSuccessCallback(void (*callback)()) {
    onUpdateSuccess = callback;
}

void OTA::setUpdateErrorCallback(void (*callback)(const String& error)) {
    onUpdateError = callback;
}

void OTA::getVersion() {
    HTTPClient http;
    Serial.println("[HTTP] GET...\n");

    // start connection and send HTTP header
    http.begin(versionUrl);
    int httpCode = http.GET();

    Serial.println("Response from server: " + String(httpCode));
    
    // httpCode will be negative on error
    if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK) {
            serverVersion = http.getString();
            Serial.println("Server version: " + serverVersion);
        }
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}

void OTA::parseVersion(const String& current, const String& server) {
    int maxIndex = current.length() - 1;
    int new_maxIndex = server.length() - 1;
    int index = 0;
    int next_index;
    String current_word;
    String server_word;
    
    // Parse current version
    do {
        next_index = current.indexOf('.', index);
        if (next_index == -1) {
            current_word = current.substring(index);
        } else {
            current_word = current.substring(index, next_index);
        }
        index = next_index + 1;
    } while ((next_index != -1) && (index < maxIndex));

    // Reset index for server version
    index = 0;
    
    // Parse server version
    do {
        next_index = server.indexOf('.', index);
        if (next_index == -1) {
            server_word = server.substring(index);
        } else {
            server_word = server.substring(index, next_index);
        }
        index = next_index + 1;
    } while ((next_index != -1) && (index < new_maxIndex));

    int currentNumber = current_word.toInt();
    int serverNumber = server_word.toInt();
    
    Serial.println("Actual FW version: " + String(currentNumber));
    Serial.println("FW version on server: " + String(serverNumber));
    
    if (serverNumber > currentNumber) {
        Serial.println("Detected new version.");
        updateAvailable = true;
    } else {
        Serial.println("No new version, continue to boot..");
        Serial.println();
        updateAvailable = false;
    }
}

void OTA::updateFirmware() {
    HTTPClient newHttp;
    
    if (onUpdateStart) {
        onUpdateStart();
    }
    
    newHttp.begin(firmwareUrl);

    // Start pulling down the firmware binary.
    int httpCode = newHttp.GET();
    if (httpCode <= 0) {
        String error = "HTTP failed, error: " + newHttp.errorToString(httpCode);
        Serial.println(error);
        if (onUpdateError) {
            onUpdateError(error);
        }
        return;
    }
    
    // Check that we have enough space for the new binary.
    int contentLen = newHttp.getSize();
    Serial.printf("Content-Length: %d\n", contentLen);
    bool canBegin = Update.begin(contentLen);
    if (!canBegin) {
        String error = "Not enough space to begin OTA";
        Serial.println(error);
        if (onUpdateError) {
            onUpdateError(error);
        }
        return;
    }
    
    // Write the HTTP stream to the Update library.
    WiFiClient* client_ = newHttp.getStreamPtr();
    size_t written = Update.writeStream(*client_);
    Serial.printf("OTA: %d/%d bytes written.\n", written, contentLen);
    
    if (written != contentLen) {
        String error = "Wrote partial binary. Giving up.";
        Serial.println(error);
        if (onUpdateError) {
            onUpdateError(error);
        }
        return;
    }
    
    if (!Update.end()) {
        String error = "Error from Update.end(): " + String(Update.getError());
        Serial.println(error);
        if (onUpdateError) {
            onUpdateError(error);
        }
        return;
    }
    
    if (Update.isFinished()) {
        if (onUpdateSuccess) {
            onUpdateSuccess();
        }
        Serial.println("Update successfully completed. Rebooting.");
        // This line is specific to the ESP32 platform:
        ESP.restart();
    } else {
        String error = "Error from Update.isFinished(): " + String(Update.getError());
        Serial.println(error);
        if (onUpdateError) {
            onUpdateError(error);
        }
        return;
    }
    newHttp.end();
}

// Default callback implementations
void OTA::defaultUpdateStart() {
    Serial.println("Starting firmware update...");
}

void OTA::defaultUpdateSuccess() {
    Serial.println("Firmware update completed successfully!");
}

void OTA::defaultUpdateError(const String& error) {
    Serial.println("Firmware update failed: " + error);
}
