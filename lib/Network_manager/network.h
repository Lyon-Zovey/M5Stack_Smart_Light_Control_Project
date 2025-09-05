#ifndef NETWORK_H
#define NETWORK_H 

#include "Arduino.h"
#include "config_parse.h"
#include "nvs_save.h"
#include <WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <map>

// Global deduplication list + mutex
extern std::map<String, String> lastSentValue;
extern SemaphoreHandle_t mqttSendMutex;

struct QueryState {
    bool light_schedule_queried = false;
    bool light_schedule_received = false;
    uint32_t query_start_time = 0;
    bool all_queries_completed = false;
};
extern QueryState queryState;

extern const uint32_t QUERY_TIMEOUT; 

// WiFi
extern const char *ssid;
extern const char *password;

// deviceToken
extern String deviceToken;
extern std::map<String, bool> uploadedNames;

extern WiFiClient espClient;
extern PubSubClient mqttClient;
extern const char* mqtt_server;
extern int mqtt_port;

void mqttReconnect(void);
void sendMQTTPing(void);
void sendMQTTData(String name, String value);
void queryEzdata(String name);
bool checkAllQueriesCompleted(void);
void sendLocalConfigIfQueryFailed(void);
void mqttCallback(char* topic, byte* payload, unsigned int length);

#endif