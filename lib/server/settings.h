#include <Preferences.h>

extern Preferences preferences;
extern String ssid;
extern String password;

void getSsidSettings();
void putSsidSettings(String ssid_);
void putPasswordSettings(String ssid_);