#include "servo.h"

void Servo_Init() { 
    ledcSetup(SERVO_CHANNEL_0, SERVO_FREQ, 12);
    ledcSetup(SERVO_CHANNEL_1, SERVO_FREQ, 12);
    ledcSetup(SERVO_CHANNEL_2, SERVO_FREQ, 12);
    ledcSetup(SERVO_CHANNEL_3, SERVO_FREQ, 12);
    ledcSetup(SERVO_CHANNEL_4, SERVO_FREQ, 12);
    
    ledcAttachPin(Servo_Pin[0], SERVO_CHANNEL_0);  
    ledcAttachPin(Servo_Pin[1], SERVO_CHANNEL_1);  
    ledcAttachPin(Servo_Pin[2], SERVO_CHANNEL_2); 
    ledcAttachPin(Servo_Pin[3], SERVO_CHANNEL_3); 
    ledcAttachPin(Servo_Pin[4], SERVO_CHANNEL_4); 
    
    Serial.println("[Servo initialization] The initialization of the five servo channels has been completed");
}

void executeServoControl(uint8_t channel, String action) {

    if (channel < 0 || channel > 5) {
        Serial.printf("[The servo control is incorrect] Ineffective channel: %d\n", channel);
        return;
    }
    uint8_t angle = 90; 
    
    if (action == "on") {
        angle = 130;  
    } else if (action == "off") {
        angle = 50; 
    } else if (action == "idle") {
        angle = 90;  
        Serial.printf("[Servo%d] Remain idle\n", channel);
        return;
    }
    
    Serial.printf("[Servo%d] %s Operation, rotate to %d°\n", channel, action.c_str(), angle);
    switch (channel) {
        case 1:
            ledcWrite(SERVO_CHANNEL_0, map(angle, 0, 180, SERVO_MIN_PULSE, SERVO_MAX_PULSE));
            break;
        case 2:
            ledcWrite(SERVO_CHANNEL_1, map(angle, 0, 180, SERVO_MIN_PULSE, SERVO_MAX_PULSE));
            break;
        case 3:
            ledcWrite(SERVO_CHANNEL_2, map(angle, 0, 180, SERVO_MIN_PULSE, SERVO_MAX_PULSE));
            break;
        case 4:
            ledcWrite(SERVO_CHANNEL_3, map(angle, 0, 180, SERVO_MIN_PULSE, SERVO_MAX_PULSE));
            break;
        case 5:
            ledcWrite(SERVO_CHANNEL_4, map(angle, 0, 180, SERVO_MIN_PULSE, SERVO_MAX_PULSE));
            break;
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
    if (action != "idle") {
        switch (channel) {
            case 1:
                ledcWrite(SERVO_CHANNEL_0, map(90, 0, 180, SERVO_MIN_PULSE, SERVO_MAX_PULSE));
                break;
            case 2:
                ledcWrite(SERVO_CHANNEL_1, map(90, 0, 180, SERVO_MIN_PULSE, SERVO_MAX_PULSE));
                break;
            case 3:
                ledcWrite(SERVO_CHANNEL_2, map(90, 0, 180, SERVO_MIN_PULSE, SERVO_MAX_PULSE));
                break;
            case 4:
                ledcWrite(SERVO_CHANNEL_3, map(90, 0, 180, SERVO_MIN_PULSE, SERVO_MAX_PULSE));
                break;
            case 5:
                ledcWrite(SERVO_CHANNEL_4, map(90, 0, 180, SERVO_MIN_PULSE, SERVO_MAX_PULSE));
                break;
        }
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}

// Execute all the servo commands in the time plan
void executeScheduleCommands(const std::vector<ServoCommand>& commands) {
    if (commands.empty()) {
        Serial.println("[executive command] No servo command");
        return;
    }
    
    Serial.printf("[executive command] There are a total of %d servo commands\n", commands.size());
    
    for (const auto& cmd : commands) {
        executeServoControl(cmd.channel, cmd.action);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}
