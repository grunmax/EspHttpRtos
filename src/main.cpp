#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <SoftwareSerial.h>
#include "wificonnect.h"
#include "websrv.h"
#include "worker.h"
#include "blesrv.h"

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <Arduino.h>

const char *ssid = "tplink-34";
const char *password = "***";
const IPAddress ip(192, 168, 1, 199);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);

void setup()
{
    Serial.begin(115200);
    setupOnBoardLed();
    bleServerSetup();
    setupWiFi(&ip, &gateway, &subnet, ssid, password);
    setupWorker();
    setupServer();
}

void loop() {}