#ifndef NVS_SAVE_H
#define NVS_SAVE_H

#include "Arduino.h"
#include "config_parse.h"
#include <Preferences.h>

// A library used for storing configuration data
extern Preferences preferences;

// defualt light schedule
extern String lightScheduleJsonString;

void saveConfigToNVS(void);
void loadConfigFromNVS(void);

#endif