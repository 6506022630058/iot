#include <Wire.h>
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include "DHT.h"
#include <time.h> // For NTP time
#include <BH1750.h>
BH1750 lightMeter;

// Pin and constants
#define DHTPIN D4 // Pin connected to the DHT22 data pin
#define DHTTYPE DHT22 // DHT 22 (AM2302)

// Firebase credentials
char FIREBASE_AUTH[] = "el1toHNMrUaHcWWNSiUyq6X1TgnbfE14MaqZUVwE"; // Your Firebase Web API Key
char FIREBASE_HOST[] = "https://fir-dht-2024-ce7d7-default-rtdb.asia-southeast1.firebasedatabase.app"; // Your Firebase Host URL

// WiFi credentials
char WIFI_SSID[] = "HomeOjo"; // Your WiFi SSID
char WIFI_PASSWORD[] = "0852494177"; // Your WiFi password
const int ledPin = 13;
int counter = 0;
// Firebase and DHT setup
FirebaseData firebaseData;
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor

unsigned long convertToSeconds(const char* datetimeStr) {
    int year, month, day, hour, minute, second;

    // Parse the datetime string
    sscanf(datetimeStr, "%d/%d/%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);

    // Calculate days since epoch (January 1, 1970)
    unsigned long totalDays = (year - 1970) * 365 + (year - 1970) / 4; // leap years
    static const int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    for (int m = 1; m < month; m++) {
        totalDays += daysInMonth[m];
    }
    // Adjust for leap year
    if (month > 2 && (year % 4 == 0) && (year % 100 != 0 || year % 400 == 0)) {
        totalDays++;
    }
    totalDays += day - 1; // Subtract 1 for the current day

    // Calculate total seconds
    unsigned long totalSeconds = totalDays * 86400 + hour * 3600 + minute * 60 + second;
    return totalSeconds;
}

void setup() {
Serial.begin(115200);
Wire.begin();
lightMeter.begin();
pinMode(ledPin,OUTPUT);
// Start WiFi connection
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("\nWi-Fi connected");

// Start Firebase connection
Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

// Start DHT sensor
dht.begin();

// Start NTP to get the time (UTC+7 for Thailand)
configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov"); // Time offset for Thailand
Serial.println("\nWaiting for NTP time sync...");
while (!time(nullptr)) {
delay(1000);
Serial.print(".");
}
Serial.println("\nTime synced!");
}

String getFormattedTime() {
time_t now = time(nullptr);
struct tm* timeinfo = localtime(&now);

char buffer[20];
strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S", timeinfo);

return String(buffer);
}

void loop() {
// Get temperature and humidity from DHT22
float temperature = dht.readTemperature();
float humidity = dht.readHumidity();

// Check if readings are valid
if (isnan(temperature) || isnan(humidity)) {
Serial.println("Failed to read from DHT sensor!");
} else {
// Get the current timestamp as a formatted string
String timestamp = getFormattedTime();

// Create a new log entry using the timestamp
String logPath = "/logs/log_" + String(convertToSeconds(timestamp.c_str())); // Unique log path



// Print values to Serial Monitor
Serial.print("Temperature: ");
Serial.print(temperature);
Serial.println("°C");
Serial.print("Humidity: ");
Serial.print(humidity);
Serial.println("%");
float lux = lightMeter.readLightLevel();
Serial.print("Light: ");
Serial.print(lux);
Serial.println(" lx");
Serial.print("Timestamp: ");
Serial.println(timestamp);
Serial.print("Logpath: ");
Serial.println(logPath);
// Send temperature, humidity, and timestamp to Firebase
if (counter % 2 == 0){
  Firebase.setString(firebaseData, logPath + "/led", "OFF");
  digitalWrite(ledPin, LOW);
}
if (counter % 2 == 1){
  Firebase.setString(firebaseData, logPath + "/led", "ON");
  digitalWrite(ledPin, HIGH);
}
Firebase.setFloat(firebaseData, logPath + "/temperature", temperature);
Firebase.setFloat(firebaseData, logPath + "/humidity", humidity);
Firebase.setFloat(firebaseData, logPath + "/light", lux);
Firebase.setString(firebaseData, logPath + "/time", timestamp);
Firebase.setString(firebaseData, logPath + "/times", String(convertToSeconds(timestamp.c_str())));
// Wait for 5 seconds before next loop
delay(5000);
counter++;
}
}