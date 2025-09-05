#include "config_parse.h"

std::vector<TimeSchedule> weekSchedule[7];

const String weekdayJsonKeys[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

// Parse Time String to Hour and Minute
void parseTimeString(String timeStr, uint8_t &hour, uint8_t &minute) {
    int colonIndex = timeStr.indexOf(':');
    if (colonIndex != -1) {
        hour = timeStr.substring(0, colonIndex).toInt();
        minute = timeStr.substring(colonIndex + 1).toInt();
        
        if (hour > 23) hour = 23;
        if (minute > 59) minute = 59;
    } else {
        hour = 0;
        minute = 0;
        Serial.printf("[Time parsing error] Invalid format file: %s\n", timeStr.c_str());
    }
}

// Parse light schedule JSON
void parseLightScheduleJson(String jsonStr) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr);
    
    if (error) {
        Serial.printf("[JSON parse error] %s\n", error.c_str());
        return;
    }
    for (int i = 0; i < 7; i++) {
        weekSchedule[i].clear();
    }
    
    // Analyze the schedule for each day
    for (int dayIndex = 0; dayIndex < 7; dayIndex++) {
        String dayKey = weekdayJsonKeys[dayIndex];
        
        if (doc[dayKey].is<JsonArray>()) {
            JsonArray daySchedule = doc[dayKey];
            
            Serial.printf("[parse%s] find %d time point\n", dayKey.c_str(), daySchedule.size());
            
            std::vector<TimeSchedule> tempSchedules;
            
            for (JsonObject item : daySchedule) {
                if (item["time"].is<const char*>()) {
                    TimeSchedule schedule;
                    String timeStr = item["time"].as<String>();
                    
                    parseTimeString(timeStr, schedule.hour, schedule.minute);
                    schedule.triggered = false;
                    schedule.commands.clear();
                    
                    if (item["cmd"].is<JsonArray>()) {
                        JsonArray cmdArray = item["cmd"];
                        
                        for (JsonObject cmdItem : cmdArray) {
                            if (cmdItem["ch"].is<int>() && cmdItem["onoff"].is<const char*>()) {
                                ServoCommand cmd;
                                cmd.channel = cmdItem["ch"].as<int>();
                                cmd.action = cmdItem["onoff"].as<String>();
                                if (cmd.channel >= 1 && cmd.channel <= 5) {
                                    schedule.commands.push_back(cmd);
                                }
                            }
                        }
                    } else {
                        // If there is no cmd field, generate the default alternate switch command
                        ServoCommand defaultCmd;
                        defaultCmd.channel = 1; 
                        static bool nextShouldBeOn = true;
                        defaultCmd.action = nextShouldBeOn ? "on" : "off";
                        nextShouldBeOn = !nextShouldBeOn;
                        
                        schedule.commands.push_back(defaultCmd);
                    }
                    
                    tempSchedules.push_back(schedule);

                    Serial.printf("  %02d:%02d -> ", schedule.hour, schedule.minute);
                    if (schedule.commands.empty()) {
                        Serial.println("no command");
                    } else {
                        for (const auto& cmd : schedule.commands) {
                            Serial.printf("Servo%d:%s ", cmd.channel, cmd.action.c_str());
                        }
                        Serial.println();
                    }
                }
            }
            
            // Ordering in Time
            if (!tempSchedules.empty()) {
                std::sort(tempSchedules.begin(), tempSchedules.end(),
                         [](const TimeSchedule& a, const TimeSchedule& b) {
                             return (a.hour * 60 + a.minute) < (b.hour * 60 + b.minute);
                         });
                
                weekSchedule[dayIndex] = tempSchedules;
            }
        }
    }
    
    Serial.println("[light_schedule analysis completed]");
}

// Convert the schedule to a JSON string
String lightScheduleToJson() {
    JsonDocument doc;
    
    for (int dayIndex = 0; dayIndex < 7; dayIndex++) {
        String dayKey = weekdayJsonKeys[dayIndex];
        JsonArray dayArray = doc[dayKey].to<JsonArray>();
        
        for (const auto& schedule : weekSchedule[dayIndex]) {
            JsonObject item = dayArray.add<JsonObject>();
            char timeStr[6];
            sprintf(timeStr, "%02d:%02d", schedule.hour, schedule.minute);
            item["time"] = timeStr;
            
            JsonArray cmdArray = item["cmd"].to<JsonArray>();
            for (const auto& cmd : schedule.commands) {
                JsonObject cmdItem = cmdArray.add<JsonObject>();
                cmdItem["ch"] = cmd.channel;
                cmdItem["onoff"] = cmd.action;
            }
        }
    }
    
    String result;
    serializeJson(doc, result);
    return result;
}
