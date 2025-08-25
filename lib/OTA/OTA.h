#ifndef OTA_H
#define OTA_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <Update.h>

class OTA {
public:
    // Constructor
    OTA(const String& versionUrl, const String& firmwareUrl, const String& currentVersion);
    
    // Main methods
    void checkForUpdate();
    void performUpdate();
    bool isUpdateAvailable() const;
    
    // Utility methods
    void setVersionUrl(const String& url);
    void setFirmwareUrl(const String& url);
    void setCurrentVersion(const String& version);
    
    // Callback functions for UI feedback
    void setUpdateStartCallback(void (*callback)());
    void setUpdateSuccessCallback(void (*callback)());
    void setUpdateErrorCallback(void (*callback)(const String& error));

private:
    String versionUrl;
    String firmwareUrl;
    String currentVersion;
    String serverVersion;
    bool updateAvailable;
    
    // Callback functions
    void (*onUpdateStart)();
    void (*onUpdateSuccess)();
    void (*onUpdateError)(const String& error);
    
    // Private helper methods
    void getVersion();
    void parseVersion(const String& current, const String& server);
    void updateFirmware();
    
    // Default callback implementations
    static void defaultUpdateStart();
    static void defaultUpdateSuccess();
    static void defaultUpdateError(const String& error);
};

#endif // OTA_H
