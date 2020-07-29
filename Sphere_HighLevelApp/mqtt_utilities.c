#include "mqtt.h"

#include <applibs/log.h>
//#include "eventloop_timer_utilities.h"
#include <unistd.h>
#include <sys/timerfd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "stdbool.h"
#include <errno.h>
#include <poll.h>
#include <stdlib.h>
#include "common.h"

static void emptyCallback(const char* topic, const char* msg) {};

static bool isThreadUp = false;
static bool killThreadRequest = false;

static pthread_mutex_t sockMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t killMutex = PTHREAD_MUTEX_INITIALIZER;

static int socketFd = -1;

static struct mqtt_client client;

static pthread_t client_daemon;

static uint8_t sendbuf[512];
static uint8_t recvbuf[512];

static void(*subCallback)(const char* topic, const char* msg) = emptyCallback;

/* external functions */
int MQTTInit(const char* addr, const char* port, const char* subTopic);
void MQTTStop();
void MQTTKillSubthread();
bool MQTTIsActiveConnection();
int MQTTPublish(const char* topic, const char* msg);
void MQTTRegisterSubscribeCallback(void(*cb)(const char*topic, const char* msg));

/* helpers */
/**
* @brief Called when message is received from MQTT topic.
* 
* @param unused Unused.
* @param published Structure with data about received message.
*/
static void publish_callback(void** unused, struct mqtt_response_publish* published);

/**
* @brief Main function of MQTT refresher subthread.
*
* @param client Pointer to MQTT client.
*/
static void client_refresher(void* client);

/**
* @brief Open a non blocking socket.
*
* @param addr IP address of server to connect.
* @param port port of said server
* @return Socket file descriptor or -1 if failed to open.
*/
int open_nb_socket(const char* addr, const char* port);

/* declarations */
int MQTTInit(const char* addr, const char* port, const char* subTopic) {

	pthread_mutex_lock(&sockMutex);
	socketFd = open_nb_socket(addr, port);

	if (socketFd == -1) {
		Log_Debug("Failed to open socket.\n");
		pthread_mutex_unlock(&sockMutex);
		MQTTStop();
		return -1;
	}
	
	mqtt_init(&client, socketFd, sendbuf, sizeof(sendbuf), recvbuf, sizeof(recvbuf), publish_callback);

	pthread_mutex_unlock(&sockMutex);

	mqtt_connect(&client, NULL, NULL, NULL, 0, NULL, NULL, MQTT_CONNECT_CLEAN_SESSION, 400);

	if (client.error != MQTT_OK) {
		Log_Debug("Failed to connect to MQTT broker\n");
		MQTTStop();
		return -1;
	}

	if (!isThreadUp) {
		if (pthread_create(&client_daemon, NULL, client_refresher, &client)) {
			Log_Debug("Failed to start client daemon.\n");
			MQTTStop();
			return -1;
		}
		isThreadUp = true;
	}

//	mqtt_subscribe(&client, subTopic, MQTT_PUBLISH_QOS_1);

	Log_Debug("MQTT client initialized.\n");

	return 0;
}

void MQTTStop() {
	pthread_mutex_lock(&sockMutex);

	if (socketFd != -1) {
		close(socketFd);
		Log_Debug("MQTT client stopped\n");
	}
	socketFd = -1;

	pthread_mutex_unlock(&sockMutex);
	
}

void MQTTKillSubthread() {
	pthread_mutex_lock(&killMutex);

	killThreadRequest = true;

	pthread_mutex_unlock(&killMutex);

	pthread_join(client_daemon, NULL);
	isThreadUp = false;
}

bool MQTTIsActiveConnection() {
	pthread_mutex_lock(&sockMutex);

	bool res = (socketFd != -1);

	pthread_mutex_unlock(&sockMutex);

	return res;
}

int MQTTPublish(const char* topic, const char* msg) {
	if (!MQTTIsActiveConnection())
		return -1;
	mqtt_publish(&client, topic, msg, strlen(msg) + 1, MQTT_PUBLISH_QOS_1);

	if (client.error != MQTT_OK) {
		Log_Debug("%d\n", client.error);
		Log_Debug("Failed to publish MQTT message\n");
		MQTTStop();
		return -1;
	}
	return 0;
}

void MQTTRegisterSubscribeCallback(void(*cb)(const char* topic, const char* msg)) {
	subCallback = cb;
}

/* declarations helpers*/

static void publish_callback(void** unused, struct mqtt_response_publish* published) {

	char* topic = malloc(published->topic_name_size + 1);
	memcpy(topic, published->topic_name, published->topic_name_size);
	topic[published->topic_name_size] = '\0';

	char* msg = malloc(published->application_message_size + 1);
	memcpy(msg, published->application_message, published->application_message_size);
	msg[published->application_message_size] = '\0';

	subCallback(topic, msg);

	free(topic);
	free(msg);

	return;
}

static void client_refresher(void* cl)
{
	while (1)
	{
		if (MQTTIsActiveConnection()) {
			int err = mqtt_sync((struct mqtt_client*) cl);
		}
		waitMs(100);

		pthread_mutex_lock(&killMutex);
		if (killThreadRequest) {
			killThreadRequest = false;
			pthread_mutex_unlock(&killMutex);
			return;
		}
		pthread_mutex_unlock(&killMutex);
	}
	return;
}

int open_nb_socket(const char* addr, const char* port) {
	struct addrinfo hints = { 0 };

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	int sockfd = -1;
	int rv;
	struct addrinfo *servinfo;

	rv = getaddrinfo(addr, port, &hints, &servinfo);

	if (rv == -1) {
		return -1;
	}

	sockfd = socket(servinfo->ai_family, hints.ai_socktype | SOCK_NONBLOCK, servinfo->ai_protocol);
	if (sockfd == -1) {
		return -1;
	}

	rv = connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);

	struct pollfd pfd = {
		.fd = sockfd,
		.events = 255,
		.revents = 255
	};

	int res = poll(&pfd, 1, 500);// wait for an event from socket
	if (res <= 0) {
		Log_Debug("\nNo response from socket\n");
		close(sockfd);
		return -1;
	}

	freeaddrinfo(servinfo);

	return sockfd;
}