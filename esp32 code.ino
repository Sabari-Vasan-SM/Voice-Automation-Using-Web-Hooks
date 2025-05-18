#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
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

// Web server on port 80
ESP8266WebServer server(80);

// Device IP as constant
const char* deviceIP = "http://192.168.1.100";

// Callback for SinricPro
bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Device %s turned %s\r\n", deviceId.c_str(), state ? "ON" : "OFF");

  if (deviceId == DEVICE_ID_1) {
    digitalWrite(RELAY1_PIN, state ? LOW : HIGH);
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
  Serial.print("Device IP Address: ");
  Serial.println(WiFi.localIP());
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

// Handle web request for relay1
void handleRelay1Toggle() {
  digitalWrite(RELAY1_PIN, !digitalRead(RELAY1_PIN));
  server.send(200, "text/plain", String("Relay 1 is ") + (digitalRead(RELAY1_PIN) == LOW ? "ON" : "OFF"));
}

// Handle web request for relay2
void handleRelay2Toggle() {
  digitalWrite(RELAY2_PIN, !digitalRead(RELAY2_PIN));
  server.send(200, "text/plain", String("Relay 2 is ") + (digitalRead(RELAY2_PIN) == LOW ? "ON" : "OFF"));
}

// Root page
void handleRoot() {
  String html = "<html><body><h1>ESP8266 Relay Control</h1>";
  html += "<a href=\"/relay1\">Toggle Relay 1</a><br>";
  html += "<a href=\"/relay2\">Toggle Relay 2</a><br>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

// Setup
void setup() {
  Serial.begin(115200);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, HIGH); // OFF
  digitalWrite(RELAY2_PIN, HIGH); // OFF

  setupWiFi();
  setupSinricPro();

  // Setup web routes
  server.on("/", handleRoot);
  server.on("/relay1", handleRelay1Toggle);
  server.on("/relay2", handleRelay2Toggle);
  server.begin();
  Serial.println("Web server started.");
}

// Loop
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    setupWiFi();
  }
  SinricPro.handle();
  server.handleClient();  // Handle web requests
}
