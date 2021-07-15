# Overview
This repo contains a prototype that demonstrates how to stream a vehicle's OBD-II data to HoloLens using [Azure Sphere](https://azure.microsoft.com/en-us/services/azure-sphere/) over [MQTT](https://en.wikipedia.org/wiki/MQTT), without the latency involved with sending the data to the cloud and back to the HoloLens.

# Digital Field Worker
***We want to set the Digital Field Worker of the future free of serial cables, diagnostic ports and telnet!***

The Digital Field Worker of the future will be required to diagnose and repair complex, expensive, heavily instrumented equipment. Existing solutions such as HoloLens Guides and Remote Assist are extremely helpful. But what if we could also infuse real-time equipment data and the ability to interact with that equipment into a Holographic application?

By adding real-time data, we empower the Digital Field Worker to read equipment trouble codes, see real-time performance data, power off equipment, reset equipment, and interact with the equipment all through the HoloLens, even when disconnected from Azure!

# Scenario
This prototype demonstrates a scaled-down version of the Digital Field Worker scenario; specifically, an automotive mechanic diagnosing vehicle issues using a HoloLens. Real time engine data from the OBD-II port is streamed to a HoloLens application using Azure Sphere over the MQTT protocol to an MQTT broker and displayed holographically while the mechanic views the engine. The mechanic can potentially clear the check engine light and perform other commands on the vehicle holographically. Dynamics 365 guides can be added to provide repair instructions for mechanic.

This specific scenario can be expanded to many other types of equipment and industry verticals. For example, an aircraft mechanic can holographically view real-time data streaming from an idling aircraft engine while standing at a safe distance. A repair technician for a office tower backup generator can diagnose, start, stop, and interact with the generator holographically.

The MQTT broker software can run on any device installed at the facility and receive telemetry from numerous pieces of equipment at the facility. As the worker, wearing a HoloLens, walks up to a piece of equipment, the HoloLens identifies the equipment (e.g. QR Code, etc.) and connects to the MQTT broker and MQTT topic the equipment is connected to.

# Video

[![Digital Field Worker](https://img.youtube.com/vi/6SPGQblTw3o/0.jpg)](http://www.youtube.com/watch?v=6SPGQblTw3o)

[![HoloLens OBD-II Video](https://img.youtube.com/vi/nVmRYWZqGKI/0.jpg)](https://www.youtube.com/watch?v=nVmRYWZqGKI)

# Architecture

The architecture for this project is depicted below and described in the following sections:

![Architecture](/media/overview.png "Architecture")

## HoloLens

![HoloLens Application](/media/HoloLensAppPicture1.jpg "HoloLensApp")

### Prerequisites

- Unity 2019.4.X (This project used 2019.4.1f1)
- Visual Studio 2019
- Windows SDK 18362+
- Universal Windows Build Support for Unity

### How to Deploy

After cloning or downloading the repository, open the “HoloLens” project folder in Unity. More information on how to deploy to HoloLens can be found here https://microsoft.github.io/MixedRealityToolkit-Unity/Documentation/BuildAndDeploy.html

Ensure that the target platform is Universal Windows Platform:

- Open menu : File > Build Settings
- Select Universal Windows Platform in the Platform list
- Click the Switch Platform button

Ensure that Windows Mixed Reality SDK has been added
- Navigate to Edit > Project Settings, Player
- Under XR Settings in the UWP tab, make sure Virtual Reality Supported is enabled, - and the Windows Mixed Reality SDK has been added to SDKs.

Build a Visual Studio solution
- Open menu : File > Build Settings
- Press build

Build to HoloLens
- Plug the HoloLens into the computer using the USB cable
- Open Visual Studio solution
- Update Solution settings
    - Solution Configuration: Release
    - Solution Platform: X86 for HoloLens 1 or ARM64 for HoloLens 2
    - Target: Device
- Deploy to Device

Update MQTT Settings
- Update MQTT settings by navigating to the settings menu in the HoloLens app, updating the values, and pressing the connect and subscribe buttons.

### Third-Party Libraries Used
- MRTK v2.3.0 - https://github.com/microsoft/MixedRealityToolkit-Unity/releases
    - Cross-platform toolkit for building Mixed Reality experiences.
- JSON .NET For Unity - https://assetstore.unity.com/packages/tools/input-management/json-net-for-unity-11347
    - HoloLens compatible library for serialization.
- M2Mqtt - https://www.nuget.org/packages/M2Mqtt/ 
    - Library for the MQTT client. We used .net 4.5 for this project.
- Engine model - https://sketchfab.com/3d-models/disassembled-v8-engine-block-3026bd87ca3945d6829d24c86ce695f0

## OBD-II Reader

The OBD-II reader is comprised of:

- [Azure Sphere MT3620](https://www.seeedstudio.com/Azure-Sphere-MT3620-Development-Kit-US-Version-p-3052.html) running the ```Sphere_HighLevelApp\```.
- [SparkFun Car Diagnostics Kit](https://www.sparkfun.com/products/10769) an OBD to UART interpreter
- [OBD-II to DB9 Cable](https://www.sparkfun.com/products/10087)
- Breadboard (optional)

The following is a schematic view of the wiring.

![Schematic](/media/Sphere_OBDII.png "Schematic")

Here are some pictures of the assembled OBD-II reader.

![Front](/media/SphereOBDFront.jpeg "Front")

## Azure Sphere

If you are new to Azure Sphere, check out this page for [Quick Start](https://docs.microsoft.com/en-us/azure-sphere/install/overview) 

### Configure Wifi ###

You may refer [here](https://docs.microsoft.com/en-us/azure-sphere/install/configure-wifi) for  Sphere Wifi configuration

### Prepare the sample ###

1. Even if you've performed this setup previously, ensure you have Azure Sphere SDK version 20.04 or above. At the command prompt, run azsphere show-version to check. Install the Azure Sphere SDK as needed.

1. Connect your Azure Sphere device to your computer by USB.

1. Enable application development, if you have not already done so, by entering the following line at the command prompt:

    azsphere device enable-development

### Build and run the sample ###

See the following Azure Sphere Quickstarts to learn how to build and deploy this sample:

   -  [with Visual Studio](https://docs.microsoft.com/azure-sphere/install/qs-blink-application)
   -  [with VS Code](https://docs.microsoft.com/azure-sphere/install/qs-blink-vscode)
   -  [on the Windows command line](https://docs.microsoft.com/azure-sphere/install/qs-blink-cli)
   -  [on the Linux command line](https://docs.microsoft.com/azure-sphere/install/qs-blink-linux-cli)

Add MQTT broker IP address in **app_manifest.json** (```allowed_connection```) and **main.c** (```mqttConf_brokerIp```)

The code publishes coolant temperature, RPM and fuel to the ```vehicle/telemetry``` MQTT topic every 500ms. You can change the polling interval by changing ```MQTT_PUBLISH_PERIOD```. Note: polling too frequently or for too much information will make the OBD-II reader return erratic results or hang.

### Third Party Libraries Used ###
- The MQTT client library used in the Sphere comes from [https://github.com/LiamBindle/MQTT-C/](https://github.com/LiamBindle/MQTT-C/)

## MQTT Broker

- [Eclipse Mosquitto](https://mosquitto.org/) MQTT broker

In reality, the MQTT broker can be any MQTT 3.1.1 compliant broker running on virtually any type of device. MQTT 5 was not tested.

# Folder Structure

- **HoloLens** - Contains the Unity project for the HoloLens app that includes the UI and the code to receive and display data from an MQTT broker.
- **Sphere_HighLevelApp** - Contains the Azure Sphere source code that polls a vehicle's OBD-II port for Coolant Temperature, Current RPM and Fuel and sends the data to an MQTT broker over Wi-Fi.
- **mkr1000** - Contains Arduino source code for an mrk1000 that polls a vehicle's OBD-II port for Coolant Temperature, Current RPM and Fuel and sends the data to an MQTT broker over Wi-Fi. A block diagram with wiring instructions is [here](/media/mkr1000_OBD.png).

# Backlog

- Update Azure Sphere firmware to make it an MQTT subscriber so HoloLens can send additional OBD-II codes and commands back to the OBD-II port (e.g. clear check engine light).
- Incorporate QR code support to allow HoloLens to lookup vehicle information automatically, and to spatially anchor the menu (assuming QR code is a sticker, stuck in/around the engine compartment).
- Incorporate Dynamics 365 Guides into the HoloLens app to provide repair instructions for mechanic.

# References

- [CarSmart](https://www.hackster.io/pilgrimbill/carsmart-88164a)
- [IoT4Car](https://www.hackster.io/frankzhao/iot4car-1b07f1)
- [Azure-Sphere-Home-Process-Controller](https://github.com/Tai-Min/Azure-Sphere-Home-Process-Controller)
# Who we are and why

For more background information, visit the [starthere](https://github.com/mixedrealityiot/starthere/blob/master/README.md) repo.
