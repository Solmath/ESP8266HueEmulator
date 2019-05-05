#include "update.h"

// #include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library
#include <ArduinoOTA.h>

void initUpdate(ESP8266WebServer *httpServer, ESP8266HTTPUpdateServer *httpUpdater, const char *hostname){
    
    httpUpdater->setup(httpServer);

    httpServer->on("/", HTTP_GET, [httpServer]() {
        httpServer->send(200, "text/html", "<html><body>Hello world! <br><br> Firmware updates can be uploaded <a href=\"/update\">here</a>.</body></html>");
    });

    // Port defaults to 8266
    // ArduinoOTA.setPort(8266);

    // Hostname for MDNS. Defaults to esp8266-[ChipID]
    ArduinoOTA.setHostname(hostname);

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else // U_SPIFFS
            type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating " + type);
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

    ArduinoOTA.begin();
}

void handleOTA(){
    ArduinoOTA.handle();
}