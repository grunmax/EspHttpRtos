#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <SoftwareSerial.h>
#include "wificonnect.h"
#include "websrv.h"
#include "worker.h"
#include "blesrv.h"
#include "settings.h"

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>


#include <Arduino.h>

const IPAddress ip(192, 168, 1, 199);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);

void setup()
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    
    getSsidSettings();
    bleServerSetup();

    if (!setupWiFi(&ip, &gateway, &subnet, ssid.c_str(), password.c_str()))
    {
        delay(60000);
        ESP.restart();
    }

    setupWorker();
    setupServer();
}

void loop() {}