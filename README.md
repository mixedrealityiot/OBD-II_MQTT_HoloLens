# Overview
This repo contains a prototype that demonstrates how to stream a vehicle's OBD-II data to HoloLens using MQTT, without the latency involved with sending the data to the cloud and back to the HoloLens.

# Scenario
The prototype demonstrates the scenario of an automotive mechanic diagnosing vehicle issues using a HoloLens. Real time engine data from the OBD-II port is streamed to a HoloLens application using the MQTT protocol and an MQTT broker and displayed holographically while the mechanic views the engine. The mechanic can potentially clear the check engine light and perform other commands on the vehicle holographically.

This specific scenario can be expanded to many other types of equipment and industry verticals. For example, an aircraft mechanic can holographically view real-time data streaming from an idling aircraft engine while standing at a safe distance. A repair technician for a office tower backup generator can diagnose, start, stop, and interact with the generator holographically. 

The MQTT broker software would run on a device installed at the facility and receive telemetry from numerous pieces of equipment over MQTT. As the worker wearing a HoloLens walks up to a piece of equipment, the HoloLens identifies the equipment (e.g. QR Code, Spatial Anchors, etc.) and connects to the MQTT broker and topic the equipment is sending to. ***TODO detail this out. Maybe a sequence diagram?*** See https://docs.microsoft.com/en-us/windows/mixed-reality/coordinate-systems

# Video

ToDo (Kevin & Steven) - Embed link to Youtube video

# Architecture

The architecture for this project is depicted below and described in the following sections:

![Architecture](/media/overview.png "Architecture")

## HoloLens

ToDo (Steven) - Describe HL app with Paho MQTT client. Include build env info and any workarounds to get MQTT client working.

## OBD-II Reader

The OBD-II reader is comprised of:

- [Arduino MKR1000](https://www.sparkfun.com/products/14394) running the ```mkr1000\mqttobd2_1.ino``` sketch.
- [SparkFun Car Diagnostics Kit](https://www.sparkfun.com/products/10769) an OBD to UART interpreter
- [OBD-II to DB9 Cable](https://www.sparkfun.com/products/10087)
- [Logic Level Converter](https://www.sparkfun.com/products/12009) (optional) to protect the MKR1000 when connecting a 3.3V device to a 5V system
- Breadboard

The following is a schematic view of the wiring.

![Schematic](/media/mkr1000_OBD.png "Schematic")

Here are some pictures of the assembled OBD-II reader.

**Front**
![Front](/media/obdfront.jpg "Front")

**Back**
![Back](/media/obdback.jpg "Back")

## Arduino Sketch

Before deploying the sketch to the MKR1000, update the Wi-Fi and MQTT connection information.

```C
char ssid[] = "your ssid";        // your network SSID (name)
char pass[] = "your wifi password";    // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;

WiFiClient wificlient;
PubSubClient mqttclient(wificlient);

const char* mqtt_server = "IP or DNS name of your local MQTT broker";
char mqtt_clientname[] = "odbreader";
char mqtt_topic[] = "vehicle/telemetry";

unsigned long lastConnectionTime = 0;
const unsigned long uploadInterval = 500L; // Delay between publishing
```

The code publishes coolant temperature, RPM and fuel to the ```vehicle/telemetry``` MQTT topic every 500ms. You can change the polling interval by changing ```uploadInterval```. Note: polling too frequently or for too much information will make the OBD-II reader return erratic results or hang.

The MQTT client library used in the sketch comes from [https://github.com/knolleary/pubsubclient/](https://github.com/knolleary/pubsubclient/) and has some limitations (e.g. can only publish using QoS 0, no MQTT 5 support, etc).

## MQTT Broker

The MQTT broker consists of the following:

- [Raspberry Pi](https://www.raspberrypi.org/) device running Raspian
- [Eclipse Mosquitto](https://mosquitto.org/) MQTT broker

In reality, the MQTT broker can be any MQTT 3.1.1 or 3.1 compliant broker running on virtually any type of device. Note, the MQTT client lib in the sketch does not support MQTT 5.

# Folder Structure

ToDo (Kevin & Steven) - As folders are created, list here and describe folder structure.

- **mkr1000** - Contains the Arduino sketch (.ino) file that polls a vehicle's OBD-II port for Coolant Temperature, Current RPM and Fuel and sends the data to an MQTT broker over Wi-Fi.

# References

- [CarSmart](https://www.hackster.io/pilgrimbill/carsmart-88164a)
- [IoT4Car](https://www.hackster.io/frankzhao/iot4car-1b07f1)

# Who we are and why

For more background information, visit the [starthere](https://github.com/mixedrealityiot/starthere/blob/master/README.md) repo.
