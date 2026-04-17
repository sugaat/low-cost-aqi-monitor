#define BLYNK_TEMPLATE_ID "TMPL6hWGjG6Ez"
#define BLYNK_TEMPLATE_NAME "Simple Weather Station"
#define BLYNK_AUTH_TOKEN "sILu5THF-OGiWdvnAwVggr6FNS5ViJgK"

#define WIFI_SSID "saroj321_fbnpa"
#define WIFI_PASS "CLFE25C6E7"

#define BLYNK_PRINT Serial  

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <BlynkSimpleEsp32.h>

// PM virtual pins
#define PM25_VPIN V0
#define PM10_VPIN V1

BlynkTimer timer; 

// PM variables
float PM25 = 0;
float PM10 = 0;

float PM25_IGNORED_DELTA = 1;
float PM10_IGNORED_DELTA = 1;


// -------------------- SEND DATA --------------------
void sendPMData() {
  Serial.println("Sending PM data");

  Blynk.virtualWrite(PM25_VPIN, PM25);
  Blynk.virtualWrite(PM10_VPIN, PM10);
}


// -------------------- GENERATE DUMMY DATA --------------------
void readAndSendPMData() {

  // Generate smooth dummy values
  float newPM25 = PM25 + random(-10, 10);
  float newPM10 = PM10 + random(-15, 15);

  // Clamp values (realistic range)
  newPM25 = constrain(newPM25, 5, 200);
  newPM10 = constrain(newPM10, 10, 300);

  float pm25Delta = abs(newPM25 - PM25) - PM25_IGNORED_DELTA;
  float pm10Delta = abs(newPM10 - PM10) - PM10_IGNORED_DELTA;

  if (pm25Delta > 0 || pm10Delta > 0) {
    PM25 = newPM25;
    PM10 = newPM10;

    Serial.printf("PM2.5: %f µg/m³ | PM10: %f µg/m³\n", PM25, PM10);

    sendPMData();
  }
}


// -------------------- MAIN LOOP FUNCTION --------------------
void reandAndSendSensorsData() {
  readAndSendPMData();
  Serial.println("Sending dummy PM sensor data");
}


// -------------------- SETUP --------------------
void setup()
{
  Serial.begin(115200);

  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);

  // Initialize starting values
  PM25 = random(20, 80);
  PM10 = random(40, 120);

  // Run every 5 sec
  timer.setInterval(5000L, reandAndSendSensorsData);
}


// -------------------- LOOP --------------------
void loop() {
  Blynk.run();  
  timer.run();
}