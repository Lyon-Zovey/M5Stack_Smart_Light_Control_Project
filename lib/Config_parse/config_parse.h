#ifndef CONFIG_PARSE_H
#define CONFIG_PARSE_H 

#include "Arduino.h"
#include <ArduinoJson.h>
#include <vector>

struct ServoCommand {
    uint8_t channel;
    String action;
};

struct TimeSchedule {
    uint8_t hour;
    uint8_t minute;
    std::vector<ServoCommand> commands;     // Support multiple servo commands
    bool triggered;
};

extern std::vector<TimeSchedule> weekSchedule[7];
extern const String weekdayJsonKeys[7];

String lightScheduleToJson(void);
void parseTimeString(String timeStr, uint8_t &hour, uint8_t &minute);
void parseLightScheduleJson(String jsonStr);

#endif