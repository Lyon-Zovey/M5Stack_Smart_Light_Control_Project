#include "nvs_save.h"

Preferences preferences;

// weekly default light schedule
String lightScheduleJsonString = R"({
  "Mon": [
    {"time": "08:40", "onoff": "on"},
    {"time": "13:00", "onoff": "off"},
    {"time": "14:00", "onoff": "on"},
    {"time": "21:00", "onoff": "off"}
  ],
  "Tue": [
    {"time": "08:40", "onoff": "on"},
    {"time": "13:00", "onoff": "off"},
    {"time": "14:00", "onoff": "on"},
    {"time": "21:00", "onoff": "off"}
  ],
  "Wed": [
    {"time": "08:40", "onoff": "on"},
    {"time": "13:00", "onoff": "off"},
    {"time": "14:00", "onoff": "on"},
    {"time": "21:00", "onoff": "off"}
  ],
  "Thu": [
    {"time": "08:40", "onoff": "on"},
    {"time": "13:00", "onoff": "off"},
    {"time": "14:00", "onoff": "on"},
    {"time": "21:00", "onoff": "off"}
  ],
  "Fri": [
    {"time": "08:40", "onoff": "on"},
    {"time": "13:00", "onoff": "off"},
    {"time": "14:00", "onoff": "on"},
    {"time": "21:00", "onoff": "off"}
  ],
  "Sat": [
    {"time": "08:40", "onoff": "on"},
    {"time": "13:00", "onoff": "off"},
    {"time": "14:00", "onoff": "on"},
    {"time": "21:00", "onoff": "off"}
  ],
  "Sun": [
    {"time": "08:40", "onoff": "on"},
    {"time": "13:00", "onoff": "off"},
    {"time": "14:00", "onoff": "on"},
    {"time": "21:00", "onoff": "off"}
  ]
})";

// save config to nvs
void saveConfigToNVS() {
    preferences.putString("light_schedule", lightScheduleJsonString);
    preferences.putULong("last_update", millis());
    Serial.println("[NVS] The light_schedule configuration has been saved.");
}

// load config from nvs
void loadConfigFromNVS() {
    String saved_schedule = preferences.getString("light_schedule", "");
    if (saved_schedule.length() > 0) {
        lightScheduleJsonString = saved_schedule;
        parseLightScheduleJson(lightScheduleJsonString);
        Serial.println("[NVS] The light_schedule configuration has been loaded.");
    } else {
        parseLightScheduleJson(lightScheduleJsonString);
        Serial.println("[NVS] Use the default light_schedule configuration.");
    }
}

