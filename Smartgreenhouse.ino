/**
 * @file Smartgreenhouse.ino
 * @brief Main application wiring together sensor/actuator modules.
 *
 * Flow:
 *  - setup(): init modules + set targets
 *  - loop(): read sensors -> controller -> actuate
 */

#include <Arduino.h>
#include "sharedState.h"
#include "dht11Sensor.h"
#include "Lightsensor.h"
#include "heater.h"
#include "mister.h"
#include "climateControl.h"

// ****************** Server start ****************************
#include "credentials.h"
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>
#include <ThingSpeak.h>
#include <DHT.h>
#include "webFiles.h"

// ****************** Server end ****************************
/**
 * @brief Shared system state containing sensor readings and targets.
 */
SharedState state;

static const uint32_t PRINT_MS = 1000;
static const uint32_t LIGHT_UPDATE = 1000; // 5 seconds

// ****************** Server start ****************************

WiFiClient client;

const unsigned long postDelay = 15000; // 15 seconds delay between ThingSpeak posts
unsigned long lastPostTime = 0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

String message = "";

JSONVar readings; // Json Variable to Hold Sensor Readings

// Timer variables for millis()
unsigned long lastTime = 0;  
unsigned long timerDelay = 1000;

/**
 * @brief Collect current sensor readings and targets.
 *
 * @details
 * return a JSON object with measured values and configured targets
 * from the shared system state.
 *
 * @return JSON string containing sensor data and targets.
 */
String getSensorReadings() {
  readings["temperature"] = round(state.tempC); // actual measured temp
  readings["humidity"] = round(state.humidityPct); // actual measured humidity
  readings["light"] = state.lampOn; // actual lamp state

  readings["tempTarget"] = state.targetTempC; // desired temp
  readings["humiTarget"] = state.targetHumidityPct ;// desired humidity
  readings["lightTarget"] = state.targetLightHoursToday; // desired lamp state

  return JSON.stringify(readings);
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void notifyClients2(String sensorReadings) {
  ws.textAll(sensorReadings);
}

/**
 * @brief Handle incoming WebSocket messages.
 *
 * @details
 * control commands received from the web client and updates
 * target values in the shared state. Also responds to data requests.
 *
 * @param arg WebSocket frame information
 * @param data Incoming message buffer
 * @param len Length of the message
 */
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (!(info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)) return;

  String msg;
  msg.reserve(len + 1);
  msg = "";                       // ensure empty

  for (size_t i = 0; i < len; i++) {
    msg.concat((char)data[i]);    // concat is usually better than operator+=
  }

  msg.trim();

  if (msg.equalsIgnoreCase("getReadings")) {
    notifyClients2(getSensorReadings());
    return;
  }

  if (msg.startsWith("T:")) state.targetTempC = msg.substring(2).toFloat();
  else if (msg.startsWith("H:")) state.targetHumidityPct = msg.substring(2).toFloat();
  else if (msg.startsWith("L:")) state.targetLightHoursToday = msg.substring(2).toFloat();
}

/**
 * @brief Handle WebSocket connection events.
 *
 * @param server WebSocket server instance
 * @param client Connected client
 * @param type Event type
 * @param arg Event-specific data
 * @param data Payload data
 * @param len Payload length
 */
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

// ****************** Server end ****************************
/**
 * @brief Arduino setup function.
 *
 * @details
 * Initializes sensors, actuators, control logic, WiFi,
 * web server, and cloud services.
 */
void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println(ESP.getResetInfo());

  dht11Begin();
  lightInit();
  heaterBegin();
  misterInit();

  climateControlBegin();

  // Setting temperature targets
  state.targetTempC = 23.0;
  state.targetHumidityPct = 55.0;

 // ****************** Server start ****************************
  ThingSpeak.begin(client);

  initWiFi();
  initWebSocket();

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", INDEX_HTML);
  });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/css", STYLE_CSS);
  });
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "application/javascript", SCRIPT_JS);
  });
  
  // Start server
  server.begin();
  // ****************** Server end ****************************
}

/**
 * @brief Main application loop.
 *
 * @details
 * Periodically updates the climate controller, sends sensor data
 * to connected clients, and publishes data to ThingSpeak.
 */
void loop() {
  dht11Read(state);
  lightUpdate(state);

  yield();
  
  climateControlUpdate(state);

  static uint32_t lastPrint = 0;
  if (millis() - lastPrint >= PRINT_MS) {
    lastPrint = millis();
    Serial.print("tempC="); Serial.print(state.tempC, 1);
    Serial.print(" hum="); Serial.print(state.humidityPct, 1);
    Serial.print(" heaterOn="); Serial.print(state.heaterOn ? "true" : "false");
    Serial.print(" misterOn="); Serial.print(state.misterOn ? "true" : "false");
    Serial.print(" lampOn="); Serial.print(state.lampOn ? "true" : "false");
    Serial.print(" lightHoursToday="); Serial.println(state.lightHoursToday);
    Serial.print("Desired hours of light="); Serial.println(state.targetLightHoursToday);

  }

  // ****************** Server start ****************************

  // Delay without stopping the CPU
  if ((millis() - lastTime) > timerDelay) {
    Serial.print("Desired temp="); Serial.println(state.targetTempC);
    Serial.print("Desired humidity="); Serial.println(state.targetHumidityPct);
    Serial.print("Desired hours of light="); Serial.println(state.targetLightHoursToday);
    
    Serial.println("_________________");
    
    String sensorReadings = getSensorReadings();
    notifyClients2(sensorReadings);
  lastTime = millis();
  }

  // ThingSpeak posting
if (millis() - lastPostTime > postDelay) {
    float dhtTemp = state.tempC;
    float dhtHumi = state.humidityPct;

    if (!isnan(dhtTemp) && !isnan(dhtHumi)) {

        ThingSpeak.setField(1, dhtTemp);
        ThingSpeak.setField(2, dhtHumi);
        ThingSpeak.writeFields(channelID, APIKey);

        Serial.print("ThingSpeak updated: ");
        Serial.print(dhtTemp); Serial.print(" °C, ");
        Serial.println(dhtHumi); Serial.println(" %");
    } else {
        Serial.println("Failed to read DHT sensor!");
    }
    lastPostTime = millis();
}

  ws.cleanupClients();
}
