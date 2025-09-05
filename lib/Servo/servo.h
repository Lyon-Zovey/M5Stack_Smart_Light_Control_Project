#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>
#include <Arduino.h>
#include "config_parse.h"

#define SERVO_CHANNEL_0 0
#define SERVO_CHANNEL_1 1
#define SERVO_CHANNEL_2 2
#define SERVO_CHANNEL_3 3
#define SERVO_CHANNEL_4 4

#define SERVO_FREQ 50 
#define SERVO_MIN_PULSE 102
#define SERVO_MAX_PULSE 512
static const uint8_t Servo_Pin[5] = {38, 7, 8, 5, 6};

void Servo_Init(void);
void executeServoControl(uint8_t channel, String action);
void executeScheduleCommands(const std::vector<ServoCommand>& commands);

#endif 

