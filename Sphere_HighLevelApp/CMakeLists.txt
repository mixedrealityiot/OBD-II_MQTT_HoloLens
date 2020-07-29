#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required(VERSION 3.10)

project(UART_HighLevelApp C)

azsphere_configure_tools(TOOLS_REVISION "20.04")
azsphere_configure_api(TARGET_API_SET "5")


add_executable(${PROJECT_NAME} main.c eventloop_timer_utilities.c 
                ./thirdparty/MQTT-C/src/mqtt.c
                ./thirdparty/MQTT-C/src/mqtt_pal.c
                "mqtt_utilities.c"
              #  "parson.c"
                "common.c"
                )
             #   "uart_utilities.c")


TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC
                           ./thirdparty/MQTT-C/include
                           ./)

target_link_libraries(${PROJECT_NAME} applibs pthread gcc_s c)

azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "Hardware/mt3620_rdb" TARGET_DEFINITION "sample_hardware.json")

azsphere_target_add_image_package(${PROJECT_NAME})
