#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

// Replace with your WiFi credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Create a WebSocket client instance
WebSocketsClient webSocket;

// Callback to handle WebSocket events.
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_CONNECTED:
      Serial.println("Connected to PC server");
      break;
    case WStype_DISCONNECTED:
      Serial.println("Disconnected from PC server");
      break;
    case WStype_TEXT:
      // Optionally process text messages received from the PC server.
      Serial.printf("Received: %s\n", payload);
      break;
    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);

  // Connect to WiFi network.
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected. IP address: ");
  Serial.println(WiFi.localIP());
  
  // Connect to the local PC server (update <PC_IP> with your PC's IP address).
  webSocket.begin("<PC_IP>", 8080, "/");
  webSocket.onEvent(webSocketEvent);
  
  // Seed the random number generator.
  randomSeed(analogRead(0));
}

void loop() {
  webSocket.loop();
  
  // Send sensor data every 1 second.
  static unsigned long lastTime = 0;
  if (millis() - lastTime > 1000) {
    lastTime = millis();
    sendSensorData();
  }
}

void sendSensorData() {
  // Create a JSON document for the sensor data.
  DynamicJsonDocument doc(256);
  
  // Simulate sensor values:
  doc["roll"] = random(-90, 91);              // Roll: -90 to 90 degrees.
  doc["pitch"] = random(-30, 31);             // Pitch: -30 to 30 degrees.
  doc["heading"] = random(0, 361);            // Heading: 0 to 360 degrees.
  doc["altitude"] = random(0, 50001);         // Altitude: 0 to 50,000 feet.
  doc["airspeed"] = random(0, 701);           // Airspeed: 0 to 700 knots.
  doc["turn_coordinator"] = random(-30, 31);  // Turn Coordinator: -30 to 30 degrees/sec.
  
  // Serialize JSON document to a string.
  String jsonString;
  serializeJson(doc, jsonString);
  
  // Send the JSON string over the WebSocket.
  webSocket.sendTXT(jsonString);
  Serial.println("Sent: " + jsonString);
}
