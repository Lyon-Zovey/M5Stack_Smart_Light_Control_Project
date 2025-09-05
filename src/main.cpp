#include <M5AtomS3.h>
#include "M5Unified.h"
#include "ntp_server.h"
#include "servo.h"
#include "nvs_save.h"
#include "network.h"
#include "config_parse.h"
#include <sys/time.h> 
#include <algorithm>

// use to transmit light schedule to MQTT
QueueHandle_t ntp_time_Queue;

void Task_LED (void *pvParameters);
void Task_Servo (void *pvParameters);
void Task_MQTT (void *pvParameters);
void Task_TimerPWR (void *pvParameters);
void Task_NTP_RTC (void *pvParameters);

void setup() {
    M5.begin();
    AtomS3.begin(true);  
    AtomS3.dis.setBrightness(30);
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.println("IP address: " + WiFi.localIP().toString());
    Serial.printf("MAC address: %s\n", WiFi.macAddress().c_str());

    // hardware init
    initNTP_RTC();
    preferences.begin("NVS_config", false);
    loadConfigFromNVS();
    timer_pwr.begin(&Wire, TIMERPWR_ADDR, 2, 1, 400000U);
    timer_pwr.set_cycle_sleep(false);
    Servo_Init();

    // mqtt config
    mqttClient.setServer(mqtt_server, mqtt_port);
    mqttClient.setCallback(mqttCallback);
    mqttClient.setBufferSize(2048 * 10);

    ntp_time_Queue = xQueueCreate(1, sizeof(TimeData));
    mqttSendMutex = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(Task_Servo, "Servo_Move", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(Task_MQTT, "MQTT_RT", 8192, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(Task_TimerPWR, "Timer Power", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(Task_NTP_RTC, "NTP_RTC", 4096, NULL, 1, NULL, 1);
}

void loop() { }

void Task_NTP_RTC (void *pvParameters) {
    uint32_t last_ntp_rtc_time = 0;
    uint32_t now = 0;
    while(1) {
        now = millis();
        if(now - last_ntp_rtc_time > 2000){
            TimeData current_time = NTP_RTC_Time_Get();
            xQueueOverwrite(ntp_time_Queue, &current_time);
            last_ntp_rtc_time = now;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void Task_Servo (void *pvParameters) {
    while(1){
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void Task_MQTT(void *pvParameters) {
    uint32_t lastPing = 0;
    uint32_t lastSendSchedule = 0;
    static bool initial_sync_completed = false;
    
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    while(1) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[WiFi] The connection is disconnected. Try to reconnect...");
            WiFi.reconnect();
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        
        if (!mqttClient.connected()) {
            mqttReconnect();
            initial_sync_completed = false;
            queryState.all_queries_completed = false;
        }
        mqttClient.loop();
        
        uint32_t now = millis();
        
        if (now - lastPing > 30000) {
            sendMQTTPing();
            lastPing = now;
        }
        
        if (!initial_sync_completed && mqttClient.connected()) {
            Serial.println("[MQTT] Start querying the latest configuration from the server...");
            String downTopic = "$ezdata/" + deviceToken + "/down";
            mqttClient.subscribe(downTopic.c_str());                
            
            // reset query state
            queryState = QueryState();
            queryState.query_start_time = now;
            // query light_schedule and update nvs
            queryEzdata("light_schedule");
            queryState.light_schedule_queried = true;
            vTaskDelay(500 / portTICK_PERIOD_MS);
            sendMQTTData("battery_voltage", String(timer_pwr.get_battery_voltage() / 100.0f));
            
            initial_sync_completed = true;
            lastSendSchedule = now;
        }
        
        // Check the completion status of the query
        if (initial_sync_completed && !queryState.all_queries_completed) {
            if (checkAllQueriesCompleted()) {
                queryState.all_queries_completed = true;
                Serial.println("[Initialization completed] light_schedule has been obtained from the server or used the local configuration");
                AtomS3.dis.drawpix(0x00ff00); 
                AtomS3.update();
                // vTaskDelay(200 / portTICK_PERIOD_MS);
            } else {
                sendLocalConfigIfQueryFailed();
                AtomS3.dis.drawpix(0xff0000); 
                AtomS3.update();
                // vTaskDelay(200 / portTICK_PERIOD_MS);
            }
        }
        
        if (queryState.all_queries_completed && (now - lastSendSchedule > 300000)) { // update local data to the ezdata each five minutes
            Serial.println("[MQTT] Regular synchronizate light_schedule...");
            sendMQTTData("light_schedule", lightScheduleJsonString);
            sendMQTTData("battery_voltage", String(timer_pwr.get_battery_voltage() / 100.0f));
            lastSendSchedule = now;
        }
        
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void Task_TimerPWR(void *pvParameters) {
    static bool cycle_sleep_disabled = false;
    static uint32_t last_dynamic_sleep_update = 0;
    const uint32_t DYNAMIC_SLEEP_UPDATE_INTERVAL = 20000;
    
    TimeData current_time;
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    
    if (cycle_sleep_disabled == 0){
        timer_pwr.set_cycle_sleep(false);
        cycle_sleep_disabled = 1;
    }
    
    // Display the weekly schedule
    Serial.println("[One-week schedule]");
    for (int dayIndex = 0; dayIndex < 7; dayIndex++) {
        Serial.printf("  %s: ", weekdayJsonKeys[dayIndex].c_str());
        if (weekSchedule[dayIndex].empty()) {
            Serial.println("no plan");
        } else {
            Serial.println();
            for (const auto& schedule : weekSchedule[dayIndex]) {
                Serial.printf("    %02d:%02d -> ", schedule.hour, schedule.minute);
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
    }
    
    while(1){
        if(xQueueReceive(ntp_time_Queue, &current_time, 0) == pdTRUE) {
            const char* weekday_str[] = {"周日", "周一", "周二", "周三", "周四", "周五", "周六"};
            
            // Handle the schedule for the day
            int dayIndex = current_time.weekday;
            
            for (auto& schedule : weekSchedule[dayIndex]) {
                if (!schedule.triggered) {
                    if (current_time.hour == schedule.hour && 
                        current_time.minute == schedule.minute && 
                        current_time.second <= 5) {
                        
                        Serial.printf("[%stiming] %02d:%02d trigger!\n", 
                                     weekdayJsonKeys[dayIndex].c_str(),
                                     schedule.hour, schedule.minute);
                        executeScheduleCommands(schedule.commands);
                        schedule.triggered = true;
                    }
                }

                if (current_time.second <= 5 && schedule.triggered) {
                    if (current_time.hour != schedule.hour || 
                        current_time.minute != schedule.minute) {
                        schedule.triggered = false;
                    }
                }
            }
            
            // Dynamically update the sleep time
            uint32_t now = millis();
            if (now - last_dynamic_sleep_update > DYNAMIC_SLEEP_UPDATE_INTERVAL) {
                setDynamicSleepTime(current_time);
                last_dynamic_sleep_update = now;
            }
            
            Serial.printf("current time: %s %02d:%02d:%02d\n",
                         weekday_str[current_time.weekday],
                         current_time.hour, current_time.minute, current_time.second);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS); 
    }
}