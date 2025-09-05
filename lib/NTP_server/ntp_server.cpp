#include "ntp_server.h"

M5UnitTimerPWR timer_pwr;

const char *ntpServer = "pool.ntp.org";
const uint16_t gmtOffset_sec = 28800; // GMT+8(3600 * 8)
const uint16_t daylightOffset_sec = 0;

// Modify the time acquisition function and add a day of the week judgment
TimeData NTP_RTC_Time_Get(void) {
    TimeData timeData;
    struct tm timeinfo;
    
    if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time");
        timeData.timeString = "Failed to obtain time";
        return timeData;
    }
    timeData.year = timeinfo.tm_year + 1900;
    timeData.month = timeinfo.tm_mon + 1;
    timeData.day = timeinfo.tm_mday;
    timeData.hour = timeinfo.tm_hour;
    timeData.minute = timeinfo.tm_min;
    timeData.second = timeinfo.tm_sec;
    timeData.weekday = timeinfo.tm_wday; 
    
    // Format the time string
    char timeStringBuffer[50];
    strftime(timeStringBuffer, sizeof(timeStringBuffer),\
            "%A, %B %d %Y %H:%M:%S", &timeinfo);
    timeData.timeString = String(timeStringBuffer);
    
    timeval tv;
    tv.tv_sec = mktime(&timeinfo);
    tv.tv_usec = 0;
    settimeofday(&tv, nullptr);
    
    return timeData;
}

// Initialize NTP & RTC time synchronization
void initNTP_RTC(void) {
    configTime(gmtOffset_sec, sizeof(daylightOffset_sec), ntpServer);

    struct tm timeinfo;
    if(getLocalTime(&timeinfo)){
        NTP_RTC_Time_Get();
    }else {
        Serial.println("Failed to obtain time");
    }
}

// calculate the next wake time
uint16_t calculateNextWakeTime(TimeData current_time) {
    uint16_t current_minutes = current_time.hour * 60 + current_time.minute;
    uint16_t next_wake_minutes = 24 * 60;
    bool has_task = false;
    
    int dayIndex = current_time.weekday;
    for (const auto& schedule : weekSchedule[dayIndex]) {
        uint16_t schedule_minutes = schedule.hour * 60 + schedule.minute;
        if (schedule_minutes > current_minutes) {
            next_wake_minutes = min(next_wake_minutes, schedule_minutes);
            has_task = true;
        }
    }
    
    if (!has_task) {
        for (int i = 1; i <= 7; i++) {
            int next_day_index = (current_time.weekday + i) % 7;
            
            if (!weekSchedule[next_day_index].empty()) {
                uint16_t earliest_time = 24 * 60;
                for (const auto& schedule : weekSchedule[next_day_index]) {
                    uint16_t schedule_minutes = schedule.hour * 60 + schedule.minute;
                    earliest_time = min(earliest_time, schedule_minutes);
                }
                if (earliest_time < 24 * 60) {
                    next_wake_minutes = earliest_time + i * 24 * 60;
                    has_task = true;
                    break;
                }
            }
        }
    }
    return next_wake_minutes;
}

void setDynamicSleepTime(TimeData current_time) {
    uint16_t next_wake_minutes = calculateNextWakeTime(current_time);
    uint16_t current_minutes = current_time.hour * 60 + current_time.minute;
    uint8_t sleep_interval[] = {120, 60, 25, 10};
    uint8_t sleep_minutes = 0;    

    // Calculate the time (in minutes) until the next wake-up
    int16_t minutes_to_wake = next_wake_minutes - current_minutes;
    if (minutes_to_wake <= 0) {
        minutes_to_wake += 24 * 60; 
    } else if (minutes_to_wake <= 8) {
        timer_pwr.set_cycle_sleep(false);
        return;
    }
    for (uint8_t interval : sleep_interval) {
        if (minutes_to_wake > interval) {
            sleep_minutes = interval;
            break;
        }
    }
    Serial.printf("[power management] Calculate the next wake-up time: the current time %02d:%02d, next task %02d:%02d, It's %d minutes away from the task\n",
                  current_time.hour, current_time.minute,
                  (next_wake_minutes / 60) % 24, next_wake_minutes % 60,
                  minutes_to_wake);
    uint16_t sleep_end_minutes = current_minutes + sleep_minutes;
    // set sleep time 
    timer_pwr.set_power_on_time(0, 0, 30);
    timer_pwr.set_power_off_time(sleep_minutes / 60, sleep_minutes % 60, 0);
    timer_pwr.set_cycle_sleep(true);

    Serial.printf("[power management] Dynamic sleep Settings: Current time %02d:%02d, next task %02d:%02d, Hibernation duration %d minute, End time of dormancy %02d:%02d\n",
                  current_time.hour, current_time.minute,(next_wake_minutes / 60) % 24, next_wake_minutes % 60,
                  sleep_minutes,(sleep_end_minutes / 60) % 24, sleep_end_minutes % 60);
}
