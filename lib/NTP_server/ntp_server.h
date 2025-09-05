#ifndef NTP_SERVER_HPP
#define NTP_SERVER_HPP

#include "Arduino.h"
#include "m5_unit_timerpwr.hpp"
#include "config_parse.h"

extern M5UnitTimerPWR timer_pwr;

// NTP server configuration
extern const char *ntpServer;
extern const uint16_t  gmtOffset_sec; // GMT+8(3600 * 8)
extern const uint16_t  daylightOffset_sec; 

// NTP&RTC time data structure
struct TimeData {
    uint16_t year;
    uint8_t month;
    uint16_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t weekday;
    String timeString;
};

TimeData NTP_RTC_Time_Get(void);
void initNTP_RTC(void);
void setDynamicSleepTime(TimeData current_time);

#endif