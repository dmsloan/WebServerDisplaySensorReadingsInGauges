/*********
  Rui Santos
  Complete instructions at https://RandomNerdTutorials.com/esp32-web-server-gauges/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

//! Remember to upload your filesystem image to the board with the "Upload Filesystem Image" command
//! under the Platform tab under the specific board type. The files that will be uploaded are all in
//! the "data" folder.

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <Encoder.h>
#include "SPIFFS.h"
#include <Arduino_JSON.h>
//#include <Adafruit_BME280.h>
#include "Secrets.h"    //all the secret stuff goes here such as ssid, passwords, etc.

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
//   avoid using pins with LEDs attached
Encoder myEnc(17, 18);
long oldPosition  = -999;

bool updated;

#define led 19
#define over80 23
#define triggerLED 16

// Replace with your network credentials
// Defined in Secrets.h
const char* ssid = SSIDa;
const char* password = PASSWORDa;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create an Event Source on /events
AsyncEventSource events("/events");

// Json Variable to Hold Sensor Readings
JSONVar readings;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

// Create a sensor object
// Adafruit_BME280 bme; // BME280 connect to ESP32 I2C (GPIO 21 = SDA, GPIO 22 = SCL)

/* Init BME280
void initBME(){
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}
*****/

// Get Sensor Readings and return JSON object
String getSensorReadings(){
//  readings["temperature"] = String(bme.readTemperature());
//  readings["humidity"] =  String(bme.readHumidity());
  readings["temperature"] = String(22.6);
  readings["amperage"] =  String(oldPosition);
  String jsonString = JSON.stringify(readings);
  return jsonString;
}

// Initialize SPIFFS
void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

// Initialize WiFi
void initWiFi() {
  bool ledState = false;
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.printf("Connecting to %s ..", ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    ledState = !ledState;
    digitalWrite(led, ledState);
    delay(1000);
  }
  digitalWrite(led, true);
  Serial.println();
  Serial.println(WiFi.localIP());
}

void setup() {
  pinMode(triggerLED,PULLDOWN);
  pinMode(led, OUTPUT);
  pinMode(over80, OUTPUT);
  // Serial port for debugging purposes
  Serial.begin(115200);
//  initBME();
  initWiFi();
  initSPIFFS();

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.serveStatic("/", SPIFFS, "/");

  // Request for the latest sensor readings
  server.on("/readings", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = getSensorReadings();
    request->send(200, "application/json", json);
    json = String();
  });

  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);

  // Start server
  server.begin();
}

void loop() {
  long newPosition = myEnc.read()/4;
  if (newPosition != oldPosition) {
    if(newPosition > 100){newPosition = 100; myEnc.write(100*4);}
    if(newPosition < 0){newPosition = 0; myEnc.write(0);}
    oldPosition = newPosition;
    Serial.printf("Mills is: %lu\n", millis());
    lastTime = millis();
    updated = false;
    Serial.println(newPosition);
    Serial.printf("Time last update is: %lu\n", lastTime);
    Serial.printf("The webpage update status is %s\n", updated ? "true" : "false");
    Serial.printf("The webpage has %sbeen updated.\n", updated ? "" : "not ");
    if(newPosition >= 80){
      digitalWrite(over80, true);
      }
    else{digitalWrite(over80, false);
      }
  }

  if ((millis() - lastTime) > 250 && updated == false) {
    events.send("ping",NULL,millis());
    events.send(getSensorReadings().c_str(),"new_readings" ,millis());
    updated = true;
    Serial.printf("The webpage has %sbeen updated.\n", updated ? "" : "not ");
  }
/*
  if ((millis() - lastTime) > timerDelay) {
    // Send Events to the client with the Sensor Readings Every 10 seconds
    events.send("ping",NULL,millis());
    events.send(getSensorReadings().c_str(),"new_readings" ,millis());
    lastTime = millis();
  }
*/
}