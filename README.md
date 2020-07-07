# Overview
This repo contains a prototype that demonstrates how to stream a vehicle's OBD-II data to HoloLens using MQTT, without the latency involved with sending the data to the cloud and back to the HoloLens.

# Scenario
The prototype demonstrates the scenario of a vehicle mechanic diagnosing vehicle issues using OBD-II data streamed to a HoloLens application. This scenario can be expanded to other equipment and industry verticals.

# Video

ToDo (Kevin & Steven) - Embed link to Youtube video

# Architecture

ToDo (Kevin) - Finish this section

![Architecture](/media/overview.png "Architecture")

## HoloLens App

ToDo (Steven) - Describe HL app with Paho MQTT client. Include build env info.

## ODB-II Reader

ToDo (Kevin) - Finish this section

![Schematic](/media/mrk1000_OBD.png "Schematic")

## MQTT Broker

ToDo (Kevin) - Describe MQTT broker

# Folder Structure

ToDo (Kevin & Steven) - As folders are created, list here and describe folder structure.

e.g.
- **ODBII_Reader** - Contains the Arduino sketch (.ino) file that polls a vehicle's OBD-II port for Coolant Temperature, Current RPM and Fuel and sends the data to an MQTT broker over Wi-Fi.

# Who we are and why

For more background information, visit the [starthere](https://github.com/mixedrealityiot/starthere/blob/master/README.md) repo.
