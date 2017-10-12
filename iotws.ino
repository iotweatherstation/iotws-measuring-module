// project: IoT Weather Station, EAFIT University, Systems Engineering, 20171
// Adaptation done by: Edwin Montoya Munera - emontoya@eafit.edu.co  David Velasquez - dvelas25@eafit.edu.co and Alexander Acosta Jiménez
// github: https://github.com/iotweatherstation/iotws-testSaveSensors.git

// Description: This program make a merge between all test .

//Libraries
#include <Arduino.h>
#include "DHT.h"
#include <ESP8266WiFi.h>  
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>




ESP8266WiFiMulti WiFiMulti;

//Constants
const unsigned long SEND_Timer = 300000;  //Delay between SEND updates, 5000 milliseconds
const unsigned int SENTVARS = 4;  //Number of sensor vars sent to REST Web App, (idHome, Temp & Humid, TimeStamp)
const char* SSID = "JIMENEZ"; //WiFi SSID, change nombre-wifi por la red propia
const char* PASSWORD = "32205045"; //WiFi Pass, coloque el password real
const char* HOST = "http://iotserver1.dis.eafit.edu.co";  //REST Web Host, replace by real 'server' url

String appName = "/weather";
String serviceSaveSensors = "/saveSensors";  //Name of the service SAVESENSORS
String serviceGetTime = "/getTime";  //Name of the service GETTIME
String serviceGetStatus = "/getSwitch"; // replace by real server service
String idhome = "aacosta8"; //Replace by your own idhome
char* propertyNames[] = {"idhome", "temp", "humid", "timestamp"}; //Vector Var names
String propertyValues[SENTVARS]; //Vector for Var values
unsigned long lastConnectionTime = 0; //Last time you connected to the server, in milliseconds

#define DHTPIN D1     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

String webGetTime() {
  // variables:
  HTTPClient http;
  String url;
  int httpCode;
  String payload;

  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    url = appName;
    url += serviceGetTime;
    http.begin(HOST + url);
    Serial.print(HOST + url);
    httpCode = http.GET();
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        payload = http.getString();
        Serial.println(payload);
      }
    }
    http.end();
    return payload;
  }
}

String webGetSwitch() {
  // variables:
  HTTPClient http;
  String url;
  int httpCode;
  String payload;

  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    url = appName;
    url += serviceGetStatus;
    url += "?idhome=";
    url += idhome;
    http.begin(HOST + url);
    Serial.print(HOST + url);
    httpCode = http.GET();
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        payload = http.getString();
        Serial.println(payload);
      }
    }
    http.end();
    return payload;
  }
}

void SEND(int SENTVARS, char* sensorNames[], String values[]) {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    HTTPClient http;
    String url = appName;
    url += serviceSaveSensors;
    url += "?";
    for (int idx = 0; idx < SENTVARS; idx++)
    {
      url += propertyNames[idx];
      url += "=";
      url += values[idx];
      url += "&";
    }

    http.begin(HOST + url);
    Serial.print(HOST + url);

    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }


}

void setup() {

  Serial.begin(9600);
  dht.begin();
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFiMulti.addAP(SSID, PASSWORD);

}

void loop() {

  if (millis() - lastConnectionTime > SEND_Timer) {
    float t = dht.readTemperature(); // example, replace by real var sensor
    float h = dht.readHumidity(); // example, replace by real var sensor
    Serial.print(" Temperature: ");
    Serial.println(t);
    Serial.print("Humidity: ");
    Serial.print(h);


    propertyValues[0] = "aacosta8"; // replace by real idHome (string type)
    propertyValues[1] = t;
    propertyValues[2] = h;
    propertyValues[3] = webGetTime(); // replace by webGetTime() (string type)

    SEND(SENTVARS, propertyNames, propertyValues);
    lastConnectionTime = millis();
  }
}
