#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

//4c:eb:d6:6d:a5:36
uint8_t slaveAddress[] = {0x4C, 0xEB, 0xD6, 0x6D, 0xA5, 0x36};

struct nowMessageType {
  String message;
};


void OnSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nSend message status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sent Successfully" : "Sent Failed");
}

bool setupWiFiNow()
{
    Serial.print("ESP-NOW..");
    if (esp_now_init() != ESP_OK) {
        Serial.println("initializing eror");
        return false;
    }
    esp_now_register_send_cb(OnSent);

    esp_now_peer_info_t slaveInfo;
    memcpy(slaveInfo.peer_addr, slaveAddress, 6);
    slaveInfo.channel = 0;  
    slaveInfo.encrypt = false;
     
    if (esp_now_add_peer(&slaveInfo) != ESP_OK){
        Serial.println("registering slave error");
        return false;
    }
    Serial.println("OK");
    return true;
}

bool sendWiFiNow(uint8_t *slave, String message)
{
    nowMessageType nowMessage;
    nowMessage.message = message;

    esp_err_t result = esp_now_send(slave, (uint8_t *) &nowMessage, sizeof(nowMessage));
    if (result != ESP_OK) {
        Serial.println("ESPNOW sent error");
        return false;
    }

    Serial.print("ESPNOW-> "); 
    Serial.println(message);
    return true;
}