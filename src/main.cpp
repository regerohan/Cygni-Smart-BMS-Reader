// Import required libraries
#ifdef ESP32
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
  #include <SPIFFS.h>
#else
  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <Hash.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <FS.h>
#endif
#include <Wire.h>
// #include <Adafruit_Sensor.h>
// #include <Adafruit_BME280.h>

// Replace with your network credentials
const char* ssid = "Android";
const char* password = "atmega328p";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readRS485soc() {
  
  float t = random(56);
  
  if (isnan(t)) {    
    Serial.println("Failed to read from Battery");
    return "";
  }
  else {
  //  Serial.println(t);
    return String(t);
  }
}

String readRS485current() {
  float h = random(99);
  if (isnan(h)) {
    Serial.println("Failed to read from battery");
    return "";
  }
  else {
  //  Serial.println(h);
    return String(h);
  }
}

String readRS485temp() {
  float p = random(200);
  if (isnan(p)) {
    Serial.println("Failed to read from battery");
    return "";
  }
  else {
    Serial.println(p);
    return String(p);
  }
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
server.on("/highcharts.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/highcharts.js");
  });
  server.on("/soc", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readRS485soc().c_str());
  });
  server.on("/current", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readRS485current().c_str());
  });
  server.on("/temp", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readRS485temp().c_str());
  });

  // Start server
  server.begin();
}
 
void loop(){ 
}