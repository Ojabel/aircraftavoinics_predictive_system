#include <WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

// Replace with your WiFi credentials
const char* ssid     = "OJTECH";
const char* password = "OJTECH4199";

// Create a WebSocket server on port 81
WebSocketsServer webSocket = WebSocketsServer(81);

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize the WebSocket server and set callback
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
  // Seed random number generator (optional but recommended)
  randomSeed(analogRead(0));
}

void loop() {
  webSocket.loop();
  
  // Send flight sensor data every 1 second
  static unsigned long lastTime = 0;
  if (millis() - lastTime > 1000) {
    lastTime = millis();
    sendFlightData();
  }
}

// Function to generate and send flight indicator data as a JSON object
void sendFlightData() {
  // Create a JSON document
  DynamicJsonDocument doc(256);
  
  // Simulate sensor values:
  // Roll: -90 to 90 degrees
  doc["roll"] = random(-90, 91);
  // Pitch: -30 to 30 degrees
  doc["pitch"] = random(-30, 31);
  // Heading: 0 to 360 degrees
  doc["heading"] = random(0, 361);
  // Altitude: 0 to 50000 feet
  doc["altitude"] = random(0, 50001);
  // Airspeed: 0 to 700 knots
  doc["airspeed"] = random(0, 701);
  // Turn: -30 to 30 degrees per second (simulate turn coordinator reading)
  doc["turn_coordinator"] = random(-30, 31);

  // Serialize the JSON document to a string
  String jsonString;
  serializeJson(doc, jsonString);
  
  // Broadcast the JSON string to all connected WebSocket clients
  webSocket.broadcastTXT(jsonString);
  Serial.println("Sent: " + jsonString);
}

// WebSocket event callback function
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Serial.println("Client connected");
      break;
    case WStype_DISCONNECTED:
      Serial.println("Client disconnected");
      break;
    default:
      break;
  }
}
