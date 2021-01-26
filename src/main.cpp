// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <Wire.h>

// Replace with your network credentials
const char* ssid = "Android";
const char* password = "atmega328p";

// Assign Rs485DDIR to GPIO15 (RE DE)
#define Rs485DDIR 15

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Read the State of Charge from BMS
String readRS485soc() {
  // Host command for Volatage, Pack Current & SOC
  byte Omessage[] = {0xA5, 0x40, 0x90, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7D};
  byte Imessage[] = {0x05, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  
/*
 0xA5, 0x40, 0x90, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7D
    |      |    |     |     |     |     |     |     |     |     |     |     |
    |      |    |     |     |     |     |     |     |     |     |     |     ------ Checksum
    |      |    |     |     |     |     |     |     |     |     |     ------------ SOC
    |      |    |     |     |     |     |     |     |     |     ------------------ SOC
    |      |    |     |     |     |     |     |     |     ------------------------ Current
    |      |    |     |     |     |     |     |     ------------------------------ Current
    |      |    |     |     |     |     |     ------------------------------------ Reserved
    |      |    |     |     |     |     ------------------------------------------ Reserved
    |      |    |     |     |     ------------------------------------------------ Pack Voltage
    |      |    |     |     ------------------------------------------------------ Pack Voltage
    |      |    |     ------------------------------------------------------------ Data size
    |      |    ------------------------------------------------------------------ Data ID
    |      ----------------------------------------------------------------------- Add.
    ------------------------------------------------------------------------------ Start Flag
*/

  digitalWrite(Rs485DDIR, HIGH);
  delay(2);
  Serial.write(Omessage, sizeof(Omessage));
  delay(1);
  digitalWrite(Rs485DDIR, LOW);
  Serial.readBytes(Imessage, 13);
  Imessage[10] >>= 8;
  uint8_t t = Imessage[10] + Imessage[11];
  t = 0.1*t; 
  if (isnan(t)) {    
    Serial.println("Failed to read from Battery");
    return "";
  }
  else {
  //  Serial.println(t);
    return String(t);
  }
}

// Read Instantaneous Current from the BMS
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

// Read the value of temp sensor
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
  // Define DDIR Pin as output
  pinMode(Rs485DDIR, OUTPUT);
  digitalWrite(Rs485DDIR, LOW);

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