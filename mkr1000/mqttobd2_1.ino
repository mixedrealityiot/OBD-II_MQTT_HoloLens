// WiFi101 - Version: Latest 
#include <WiFi101.h>

// PubSubClient - Version: Latest 
#include <PubSubClient.h>

char ssid[] = "your_ssid";        // your network SSID (name)
char pass[] = "your_wifi_password";    // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;

WiFiClient wificlient;
PubSubClient mqttclient(wificlient);

//const char* server = "test.mosquitto.org";
const char* mqtt_server = "192.168.1.5";
char mqtt_clientname[] = "odbreader";
char mqtt_topic[] = "vehicle/telemetry";

unsigned long lastConnectionTime = 0;            
const unsigned long uploadInterval = 500L; // Delay between publishing

int engineTemp = 0;
int engineRpm = 0;
int fuel = 0;

char rxData[20];
char rxIndex = 0;

void setup() {
  
  pinMode(6, OUTPUT);
  
  // Connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
  
    // Wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to WiFi");
  
  mqttclient.setServer(mqtt_server, 1883);  // Set the MQTT broker details.
  mqttclient.setKeepAlive(0);  //Disable keep alive
  
  // External serial port to OBD
  Serial1.begin(9600);
  Serial1.flush(); // Flush ODB input port
  
}
void loop() {

  if (!mqttclient.connected()) {
    reconnect();
  }
  else
  {

    if(Serial1.available()) {   
      
      //Serial.println("Connected to Serial1.");
      digitalWrite(6, HIGH);   // Turn the LED on to indicate success (HIGH is the voltage level)
      
      // After uploadInterval, do a new transmission
      if (millis() - lastConnectionTime > uploadInterval) {
  
        getCoolantTemp();
        getEngineRpm();
        //getFuel();
        
        Serial.println("Publishing...");
        publish();
        lastConnectionTime = millis();
        
      }
      
    }
    else
    {
      Serial.println("Attempting Serial1 connection...retrying");
      delay(5000);
    }
    
  }

}

void publish() {
  
  // Build up message and send it to MQTT broker.
  String payload = "{ \"engineTemp\": ";
  payload.concat(engineTemp);
  payload.concat(",");
  payload.concat(" \"engineRpm\": ");
  payload.concat(engineRpm);
  payload.concat(",");
  payload.concat(" \"fuel\": ");
  payload.concat(fuel);
  payload.concat(" }");
  
  // Convert to const char *
  const char * message = payload.c_str();

  mqttclient.publish(mqtt_topic, message);
  
}

void reconnect() {
  
  // Loop until we're reconnected
  while (!mqttclient.connected()) {
    digitalWrite(6, LOW);    // Turn the LED off by making the voltage LOW
    Serial.print("Attempting MQTT connection...");

    if (mqttclient.connect(mqtt_clientname)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttclient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// The getResponse function collects incoming data from the UART into the rxData buffer
// and only exits when a carriage return character is seen. Once the carriage return
// string is detected, the rxData buffer is null terminated (so we can treat it as a string)
// and the rxData index is reset to 0 so that the next string can be copied.
void getResponse(void){
  char inChar=0;
  //Keep reading characters until we get a carriage return
  while(inChar != '\r'){
    //If a character comes in on the serial port, we need to act on it.
    if(Serial1.available() > 0){
      //Start by checking if we've received the end of message character ('\r').
      if(Serial1.peek() == '\r'){
        //Clear the Serial buffer
        inChar=Serial1.read();
        //Put the end of string character on our data string
        rxData[rxIndex]='\0';
        //Reset the buffer index so that the next character goes back at the beginning of the string.
        rxIndex=0;
      }
      //If we didn't get the end of message character, just add the new character to the string.
      else{
        //Get the new character from the Serial port.
        inChar = Serial1.read();
        //Add the new character to the string, and increment the index variable.
        rxData[rxIndex++]=inChar;
      }
    }
  }
}

// Read Rpm
void getEngineRpm() {

  //Serial.println("getEngineRpm");
  Serial1.println("010C"); // Request engine RPM
  getResponse(); // Get the echo
  //Serial.print("OBD Echo:  ");
  //Serial.println(rxData);
  getResponse(); // Get the SEARCHING
  //Serial.print("OBD Response1:  ");
  //Serial.println(rxData);
  getResponse(); // Get the real response
  //Serial.print("OBD Response 2: ");
  //Serial.println(rxData);
  engineRpm = ((strtol(&rxData[6],0,16)*256) + strtol(&rxData[9],0,16))/4;
  Serial.print("Engine RPM: ");
  Serial.println(engineRpm);
  
}

// Read temp (C)
void getCoolantTemp() {
  
  //Serial.println("getCoolantTemp");
  Serial1.println("0105"); // Request engine coolant
  getResponse(); // Get the echo
  //Serial.print("OBD Echo:  ");
  //Serial.println(rxData);
  getResponse(); // Get the SEARCHING
  //Serial.print("OBD Response1:  ");
  //Serial.println(rxData);
  getResponse(); // Get the real response
  //Serial.print("OBD Response 2: ");
  //Serial.println(rxData);
  engineTemp = strtol(&rxData[6],0,16);
  engineTemp = engineTemp - 40;
  Serial.print("Coolant temp: ");
  Serial.println(engineTemp);

}

// Read fuel %
void getFuel(void) {

  //Serial1.println("012F");
  getResponse(); // Get the echo
  //Serial.print("OBD Echo:  ");
  //Serial.println(rxData);
  getResponse(); // Get the SEARCHING
  //Serial.print("OBD Response1:  ");
  //Serial.println(rxData);
  getResponse(); // Get the real response
  //Serial.print("OBD Response 2: ");
  //Serial.println(rxData);
  fuel = strtol(&rxData[6], 0, 16); // in the scale of 255
  fuel = 1.0* fuel / 255 *100; // in the scale of 100
  Serial.print("Engine Fuel: ");
  Serial.println(fuel);
  
}