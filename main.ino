#include <WiFi.h>
#include <HTTPClient.h>

// --- Wi-Fi & Backend Settings ---
const char* ssid = "YOUR_WIFI_SSID";          // Put your Wi-Fi name here
const char* password = "YOUR_WIFI_PASSWORD";  // Put your Wi-Fi password here

// Replace with your Raspberry Pi's local IP address and port
const char* serverName = "http://192.168.1.X:5000/api/sensor"; 
const int nodeId = 1; // Unique ID for this specific sensor

// --- Hardware Pins & Timing ---
const int sampleTimeinMs = 200;
const int serialAdress = 9600;

const int waterSensor = 34;
const int warningLed = 32;
const int wlanLed = 33;

// Track the state so we only send data when something changes
bool lastStateWasWet = false; 

void setup() {
  Serial.begin(serialAdress);

  pinMode(waterSensor, INPUT);
  pinMode(warningLed, OUTPUT);
  pinMode(wlanLed, OUTPUT);

  // 1. Start Wi-Fi connection
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  // Loop until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // 2. Connected! Turn on the WLAN LED
  Serial.println("\nConnected to WiFi!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(wlanLed, HIGH); 
}

void loop() {
  // Read sensor (Assuming LOW means water detected based on your original code)
  bool isCurrentlyWet = (digitalRead(waterSensor) == LOW);

  if (isCurrentlyWet) {
    digitalWrite(warningLed, HIGH); // Turn on local warning LED
    
    // Only send to the server if it just transitioned from Dry to Wet
    if (!lastStateWasWet) {
        Serial.println("Water detected! Sending alert to Pi...");
        sendDataToServer(true);
        lastStateWasWet = true; // Update state
    }
  } else {
    digitalWrite(warningLed, LOW); // Turn off local warning LED
    
    // Only send to the server if it just transitioned from Wet to Dry
    if (lastStateWasWet) {
        Serial.println("Sensor dried. Sending update to Pi...");
        sendDataToServer(false);
        lastStateWasWet = false; // Update state
    }
  }

  // Safety check: if Wi-Fi drops, turn off the LED so you know visually
  if (WiFi.status() != WL_CONNECTED) {
     digitalWrite(wlanLed, LOW);
  } else {
     digitalWrite(wlanLed, HIGH);
  }

  delay(sampleTimeinMs);
}

// --- Helper Function to Send Data ---
void sendDataToServer(bool isWet) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Specify the target URL
    http.begin(serverName); 
    
    // Tell the server we are sending JSON data
    http.addHeader("Content-Type", "application/json");

    // Create a simple JSON payload. Example: {"node_id": 1, "is_wet": true}
    String jsonPayload = "{\"node_id\": " + String(nodeId) + ", \"is_wet\": " + (isWet ? "true" : "false") + "}";

    // Send the HTTP POST request
    int httpResponseCode = http.POST(jsonPayload);

    // Print the response for debugging
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    
    http.end(); // Free resources
  } else {
    Serial.println("WiFi Disconnected. Cannot send data.");
  }
}