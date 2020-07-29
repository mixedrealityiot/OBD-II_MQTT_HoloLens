#pragma once

/**
* @brief Start MQTT client and create subthread if necessary.
*/
int MQTTInit(const char* addr, const char* port, const char* subTopic);

/**
* @brief Stop MQTT client but keep subthread running.
*/
void MQTTStop();

/**
* @brief Stop MQTT subthread.
*/
void MQTTKillSubthread();

/**
* @brief Check wherhet there is active connection with MQTT broker.
*
* @return True if there is active connection.
*/
bool MQTTIsActiveConnection();

/**
* @brief Queue message to be published on given topic.
*
* @param topic Topic string.
* @param msg Message to be published.
* @return 0 on success, -1 on failute.
*/
int MQTTPublish(const char* topic, const char* msg);

/**
* @brief Add function that will be called when MQTT client receives message from subscribed topic.
*
* @param cn Callback function.
*/
void MQTTRegisterSubscribeCallback(void(*cb)(const char* topic, const char* msg));