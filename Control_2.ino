#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Wi-Fi Configuration
const char* ssid = "1i";
const char* password = "12345678910";

// ThingSpeak API Information
const char* server = "http://api.thingspeak.com";
const String apiKey = "EAIVD6YRLVL7EN3V"; // ThingSpeak Write API Key

// Sensor Pins
const int sensor1Pin = D0;
const int sensor2Pin = D1;
const int sensor3Pin = D2;

// Creating a WiFiClient instance
WiFiClient client;

// Variables to store the previous status of sensors
int prevSensor1Status = HIGH;
int prevSensor2Status = HIGH;
int prevSensor3Status = HIGH;

void setup() {
  // Initializing serial communication
  Serial.begin(115200);

  // Initializing Wi-Fi connection
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  // Initializing sensor pins as inputs
  pinMode(sensor1Pin, INPUT);
  pinMode(sensor2Pin, INPUT);
  pinMode(sensor3Pin, INPUT);
}

void loop() {
  // Read sensor data
  int sensor1Status = digitalRead(sensor1Pin);
  int sensor2Status = digitalRead(sensor2Pin);
  int sensor3Status = digitalRead(sensor3Pin);

  // Check if there's a change in status
  if (sensor1Status != prevSensor1Status || sensor2Status != prevSensor2Status || sensor3Status != prevSensor3Status) {
    // Update the previous statuses
    prevSensor1Status = sensor1Status;
    prevSensor2Status = sensor2Status;
    prevSensor3Status = sensor3Status;

    // Display parking space status in Serial Monitor
    Serial.print("Sensor 1 (Space A): ");
    Serial.println(sensor1Status == LOW ? "Occupied" : "Vacant");
    Serial.println(sensor1Status == LOW ? "███████" : "/       /");

    Serial.print("Sensor 2 (Space B): ");
    Serial.println(sensor2Status == LOW ? "Occupied" : "Vacant");
    Serial.println(sensor2Status == LOW ? "███████" : "/       /");

    Serial.print("Sensor 3 (Space C): ");
    Serial.println(sensor3Status == LOW ? "Occupied" : "Vacant");
    Serial.println(sensor3Status == LOW ? "███████" : "/       /");

    // Send data to ThingSpeak
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;

      // Construct the URL for the HTTP request
      String url = String(server) + "/update?api_key=" + apiKey +
                   "&field1=" + String(sensor1Status) +
                   "&field2=" + String(sensor2Status) +
                   "&field3=" + String(sensor3Status);

      // Use the updated begin() function
      http.begin(client, url); // Pass the WiFiClient instance and URL
      int httpCode = http.GET();

      // Check the response
      if (httpCode > 0) {
        Serial.println("Data sent to ThingSpeak successfully.");
      } else {
        Serial.print("Error sending data: ");
        Serial.println(http.errorToString(httpCode).c_str());
      }

      http.end(); // End the HTTP request
    } else {
      Serial.println("Wi-Fi disconnected. Reconnecting...");
      WiFi.begin(ssid, password);
    }
  }

  delay(1000); // Wait 1 second before the next repetition
}
