#include <WiFi.h>

void setupWiFi(IPAddress const *ip, IPAddress const *gateway, IPAddress const *subnet, char const *ssid, char const *password)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    WiFi.config(*ip, *gateway, *subnet);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.printf("WiFi Failed!\n");
        return;
    }
    
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}