#include <ESP8266WiFi.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>

// WiFi Credentials
#define WIFI_SSID     "Vasan's Nothing !"
#define WIFI_PASS     "sabarivasan1239"

// Sinric Pro Credentials
#define APP_KEY       "32f66719-cc7d-4276-9825-af41fffd6ae1"
#define APP_SECRET    "ccbeea74-7a5d-46bd-bb8f-d3dbc80850cf-24a36cc0-c358-4fa7-bf9a-1ee4c5f01030"
#define DEVICE_ID_1   "6817181edc4a25d5c3c3b9b9"  // First Relay
#define DEVICE_ID_2   "682048d78ed485694c3416f2"  // Second Relay

// Relay Pins
#define RELAY1_PIN    5   // D1 = GPIO5
#define RELAY2_PIN    4   // D2 = GPIO4

// Callback function to handle power state changes
bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Device %s turned %s\r\n", deviceId.c_str(), state ? "ON" : "OFF");

  if (deviceId == DEVICE_ID_1) {
    digitalWrite(RELAY1_PIN, state ? LOW : HIGH);  // LOW = ON (Active Low)
  } else if (deviceId == DEVICE_ID_2) {
    digitalWrite(RELAY2_PIN, state ? LOW : HIGH);
  }

  return true;
}

// WiFi Setup
void setupWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");
}

// Sinric Pro Setup
void setupSinricPro() {
  SinricProSwitch &relay1 = SinricPro[DEVICE_ID_1];
  relay1.onPowerState(onPowerState);

  SinricProSwitch &relay2 = SinricPro[DEVICE_ID_2];
  relay2.onPowerState(onPowerState);

  SinricPro.begin(APP_KEY, APP_SECRET);
  SinricPro.restoreDeviceStates(true);
}

// Main Setup
void setup() {
  Serial.begin(115200);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);

  digitalWrite(RELAY1_PIN, HIGH); // OFF
  digitalWrite(RELAY2_PIN, HIGH); // OFF

  setupWiFi();
  setupSinricPro();
}

// Loop
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    setupWiFi();
  }
  SinricPro.handle();
}