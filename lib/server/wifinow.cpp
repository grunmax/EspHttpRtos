#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

uint8_t slaveAddress[] = {0x4C, 0xEB, 0xD6, 0x6D, 0xA5, 0x36};
esp_now_peer_info_t slaveInfo;
bool lastNowStatus = false;

typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;

struct_message myData;

void printMac(uint8_t* macaddr) {
  Serial.print("{");
  for (int i = 0; i < 6; i++) {
    Serial.print("0x");
    Serial.print(macaddr[i], HEX);
    if (i < 5) Serial.print(',');
  }
  Serial.println("};");
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.print(status);
  Serial.print("\t");
  lastNowStatus = status == ESP_NOW_SEND_SUCCESS;
  Serial.println(lastNowStatus ? "Delivery Success" : "Delivery Fail");
}

bool setupWiFiNow()
{
    Serial.print("ESP-NOW..");
    Serial.print(WiFi.channel());
    Serial.print("..");
    esp_wifi_set_channel(WiFi.channel(), WIFI_SECOND_CHAN_NONE);

    if (esp_now_init() != ESP_OK) 
    {
        Serial.println("initializing eror");
        return false;
    }
    esp_now_register_send_cb(OnDataSent);
    Serial.println("OK");
    return true;
}

bool setupPeer()
{
    Serial.print("Add peer..");
    memcpy(slaveInfo.peer_addr, slaveAddress, 6);
    slaveInfo.channel = WiFi.channel();  
    slaveInfo.encrypt = false;
    slaveInfo.ifidx=WIFI_IF_AP;
     
    if (esp_now_add_peer(&slaveInfo) != ESP_OK)
    {
        Serial.println("registering slave error");
        return false;
    }
    // esp_now_register_recv_cb(OnDataRecv);
    Serial.println("OK");
    return true;
}

bool sendWiFiNow(String message)
{
    printMac(slaveInfo.peer_addr);
    strcpy(myData.a, "Welcome!");
    myData.b = random(1,20);
    myData.c = 1.3;
    myData.d = true;

    esp_err_t result = esp_now_send(slaveAddress, (uint8_t *) &myData, sizeof(myData));

    Serial.print("ESPNOW-> "); 
    if(result == ESP_OK) {
        Serial.println("sent OK");
        return true;
    } 
    else if(result == ESP_ERR_ESPNOW_NOT_INIT) {
        Serial.println("is not initialized error");
        return false;
    }
    else if(result == ESP_ERR_ESPNOW_ARG) {
        Serial.println("invalid argument error");
        return false;
    }
    else if(result == ESP_ERR_ESPNOW_NO_MEM) {
        Serial.println("no memory error");
        return false;
    }
    else if(ESP_ERR_ESPNOW_INTERNAL) {
        Serial.println("internal error");
        return false;    
    }
    else if(result == ESP_ERR_ESPNOW_NOT_FOUND) {
        Serial.println("no peer error");
        return false;
    } 
    else
    {
        Serial.println("unknown error");
        return false;
    }
}