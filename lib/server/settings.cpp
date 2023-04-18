#include <Preferences.h>

Preferences preferences;
String ssid;
String password;

void getSsidSettings()
{
    preferences.begin("appx", false);
    ssid  = preferences.getString("ssid", "foo_ssid");
    password = preferences.getString("password", "dummy123");
    preferences.end();
}

void putSsidSettings(String ssid_)
{
    preferences.begin("appx", false);
    preferences.putString("ssid", ssid_);
    preferences.end();
}

void putPasswordSettings(String pass_)
{
    preferences.begin("appx", false);
    preferences.putString("password", pass_);
    preferences.end();
}
