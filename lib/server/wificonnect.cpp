#include <WiFi.h>

bool setupWiFi(IPAddress const *ip, IPAddress const *gateway, IPAddress const *subnet, char const *ssid, char const *password)
{
    Serial.print("WiFi connection: ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    WiFi.config(*ip, *gateway, *subnet);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("WiFi Failed");
    
        return false;
    }
    
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    return true;
}