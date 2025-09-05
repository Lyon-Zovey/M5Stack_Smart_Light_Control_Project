#include "network.h"

std::map<String, String> lastSentValue;
SemaphoreHandle_t mqttSendMutex;
QueryState queryState;
const uint32_t QUERY_TIMEOUT = 10000;  

// config of WiFi
const char* ssid = "***********";                   // Change to your WiFi SSID
const char* password = "***********";               // Change to your WiFi password

// deviceToken
String deviceToken = "******************";          // Change to your device token from ezdata
std::map<String, bool> uploadedNames;

// config of MQTT
WiFiClient espClient;
PubSubClient mqttClient(espClient);
const char* mqtt_server = "******************";    // Change to your MQTT server
int mqtt_port = 1883;

static bool data_synced_from_server = false;
static uint32_t last_query_time = 0;

// mqtt reconnect
void mqttReconnect() {
    while (!mqttClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        String clientId = "ez" + String("*********") + "ez";            // Change to your device mac address without ':'
        if (mqttClient.connect(clientId.c_str(), deviceToken.c_str(), "")) {
            Serial.println("connected");
            String downTopic = "$ezdata/" + deviceToken + "/down";
            mqttClient.subscribe(downTopic.c_str());
            Serial.println("Subscribed to: " + downTopic);
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            vTaskDelay(3000 / portTICK_PERIOD_MS);
        }
    }
}

void sendMQTTPing() {
    JsonDocument doc;  
    doc["deviceToken"] = deviceToken;
    doc["body"] = "ping";
    char buffer[128];
    size_t n = serializeJson(doc, buffer);
    String upTopic = "$ezdata/" + deviceToken + "/up";
    mqttClient.publish(upTopic.c_str(), buffer, n);
    Serial.println("[MQTT Heartbeat] ping");
}

// MQTT data uplink (sender deduplication + thread safety)
void sendMQTTData(String name, String value) {
    xSemaphoreTake(mqttSendMutex, portMAX_DELAY); 
    if (lastSentValue[name] == value) {
        Serial.printf("[MQTT sending skipped] %s = %s (The value remains unchanged.)\n",
                        name.c_str(), value.c_str());
        xSemaphoreGive(mqttSendMutex);
        return;
    }
    lastSentValue[name] = value;
    
    JsonDocument doc;
    doc["deviceToken"] = deviceToken;
    JsonObject body = doc["body"].to<JsonObject>();
    body["name"] = name;
    body["value"] = value;
    body["requestType"] = 101;
    
    char* buffer = new char[2048];          // Increase the buffer size to support large JSON
    size_t n = serializeJson(doc, buffer, 2048);
    
    if (n >= 2048) {
        Serial.println("[MQTT send wrong] The JSON is too large and exceeds the buffer");
        delete[] buffer;
        xSemaphoreGive(mqttSendMutex);
        return;
    }
    
    String upTopic = "$ezdata/" + deviceToken + "/up";
    bool published = mqttClient.publish(upTopic.c_str(), buffer, n);
    
    Serial.printf("[MQTT send] %s\n", published ? "succeed" : "fail");
    if (published) {
        Serial.printf("[content] %s\n", buffer);
    }
    
    delete[] buffer;
    xSemaphoreGive(mqttSendMutex);
}

// use 104 to query light schedule from ezdata
void queryEzdata(String name) {
    if (!mqttClient.connected()) {
        Serial.println("[query error] MQTT ununited");
        return;
    }
    
    xSemaphoreTake(mqttSendMutex, portMAX_DELAY);
    
    JsonDocument doc;  
    doc["deviceToken"] = deviceToken;
    JsonObject body = doc["body"].to<JsonObject>();  
    body["name"] = name;
    body["requestType"] = 104; 
    
    char buffer[256] = {0};
    size_t n = serializeJson(doc, buffer);
    String upTopic = "$ezdata/" + deviceToken + "/up";
    bool published = mqttClient.publish(upTopic.c_str(), buffer);
    
    Serial.printf("[MQTT query] %s\n", buffer);
    Serial.printf("[publish status] %s\n", published ? "succeed" : "fail");
    
    if (!published) {
        Serial.println("[Query failed] Message publishing failed and local configuration will be used");
        if (name == "week_schedule") {
            queryState.light_schedule_received = true;
        }
    }
    
    xSemaphoreGive(mqttSendMutex);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String message;
    for (unsigned int i = 0; i < length; i++) message += (char)payload[i];
    Serial.printf("[MQTT receive] Topic: %s Message: %s\n", topic, message.c_str());
    
    if (message == "fail" || message == "\"fail\"") {
        Serial.println("[MQTT] Upon receiving a fail response, ignore the handling");
        return;
    }
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, message);
    
    if (error) {
        Serial.printf("[JSON parse error] %s\n", error.c_str());
        return;
    }
    
    int cmd = doc["cmd"];
    int code = doc["code"];
    
    Serial.printf("[response analysis] cmd: %d, code: %d\n", cmd, code);
    
    // Handle query responses (cmd 104)
    if (cmd == 104) {
        if (code == 200) {
            if (doc["body"].is<JsonObject>()) {
                JsonObject body = doc["body"];
                
                if (body["name"].is<const char*>() && body["value"].is<JsonVariant>()) {
                    String name = body["name"].as<String>();
                    // two situations: 1.json 2.str
                    if (name == "light_schedule") { 
                        if (body["value"].is<JsonObject>()) {
                            String value;
                            serializeJson(body["value"], value);
                            Serial.println("[Server query successful] light_schedule：" + value);
                            lightScheduleJsonString = value;
                            parseLightScheduleJson(lightScheduleJsonString);
                            queryState.light_schedule_received = true;
                            saveConfigToNVS();
                        }
                        else if (body["value"].is<const char*>()) {
                            String value = body["value"].as<String>();
                            Serial.println("[Server query successful] light_schedule：" + value);
                            lightScheduleJsonString = value;
                            parseLightScheduleJson(lightScheduleJsonString);
                            queryState.light_schedule_received = true;
                            saveConfigToNVS();
                        }
                    }
                }
            }
        } else {
            Serial.printf("[Server query failed] code: %d, Use local configuration\n", code);
            if (!queryState.light_schedule_received) {
                queryState.light_schedule_received = true;
                Serial.println("[Use local configuration] light_schedule");
            }
        }
    }
    else if (cmd == 500) {
        Serial.printf("[server erro] code: %d, Use local configuration\n", code);
        if (!queryState.light_schedule_received) {
            queryState.light_schedule_received = true;
            Serial.println("[The server does not have this configuration] Use local configuration");
        }
    }
    // 处理实时更新
    else if (doc["body"].is<JsonObject>()) {
        JsonObject body = doc["body"];
        
        if (body["name"].is<const char*>() && body["value"].is<JsonVariant>()) {
            String name = body["name"].as<String>();
            
            if (name == "light_schedule") {
                String value;
                if (body["value"].is<const char*>()) {
                    value = body["value"].as<String>();
                } else {
                    serializeJson(body["value"], value);
                }
                
                static String lastReceivedSchedule;
                if (lastReceivedSchedule == value) {
                    Serial.println("[MQTT receive] The schedule remains unchanged. Ignore the handling");
                    return;
                }
                lastReceivedSchedule = value;
                
                Serial.println("[real-time update] light_schedule：" + value);
                lightScheduleJsonString = value;
                parseLightScheduleJson(lightScheduleJsonString);
                saveConfigToNVS();
            }
        }
    }
}

bool checkAllQueriesCompleted() {
    return queryState.light_schedule_received;
}

void sendLocalConfigIfQueryFailed() {
    uint32_t now = millis();
    
    if (now - queryState.query_start_time > QUERY_TIMEOUT) {
        Serial.println("[Request time out] Start using the local configuration");
        
        if (!queryState.light_schedule_received) {
            Serial.println("[Use local configuration] light_schedule");
            String compactJson = lightScheduleToJson();
            queryState.light_schedule_received = true;
        }
        queryState.all_queries_completed = true;
    }
}

