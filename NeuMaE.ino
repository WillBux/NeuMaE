#include <ESP8266WiFi.h>
#include <aREST.h>
#include <Arduino_JSON.h>

#define SCL D2 //GPIO4
#define SDA D1 //GPIO5
#define MOTOR D6 //GPIO12
#define LED D7 //GPIO13

// Create aREST instance
aREST rest = aREST();

bool blinkLed = false;
bool isLedOn = false;
unsigned long lastTime;

// WiFi parameters
const char* ssid = "GTother";
const char* password = "GeorgeP@1927";

// The port to listen for incoming TCP connections
#define LISTEN_PORT 80

// Create an instance of the server
WiFiServer server(LISTEN_PORT);

// Declare functions to be exposed to the API
int ledOn(String command);
int ledOff(String command);
int motorOn(String command);
int motorOff(String command);
int battery(String command);

void setup(void) {
    // Start Serial
    Serial.begin(115200);

    // Function to be exposed
    rest.function("ledOn",ledOn);
    rest.function("ledOff",ledOff);
    rest.function("motorOn", motorOn);
    rest.function("motorOff", motorOff);
    rest.function("battery", battery);

    pinMode(LED, OUTPUT);
    pinMode(MOTOR, OUTPUT);
    pinMode(A0, INPUT);

    // Give name & ID to the device (ID should be 6 characters long)
    rest.set_id("1");
    rest.set_name("esp8266");

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    // Start the server
    server.begin();
    Serial.println("Server started");

    // Print the IP and MAC address
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
}

void loop() {
    if (blinkLed) {
        if (millis() - lastTime > 100) {
            isLedOn = !isLedOn;
            digitalWrite(LED, isLedOn);
            lastTime = millis();
        }
    }
    // Handle REST calls
    WiFiClient client = server.available();
    if (!client) return;
    while(!client.available()) delay(1);
    rest.handle(client);

}

// API Functions
int ledOn(String command) {
    Serial.println("LEDON");
    blinkLed = true;
    lastTime = millis();
    isLedOn = true;
    digitalWrite(LED, HIGH);
    return 1;
}

int ledOff(String command) {
    Serial.println("LEDOFF");
    blinkLed = false;
    digitalWrite(LED, LOW);
    return 1;
}

int motorOn(String command) {
    Serial.println("MOTORON");
    digitalWrite(MOTOR, HIGH);
    return 1;
}

int motorOff(String command) {
    Serial.println("MOTOROFF");
    digitalWrite(MOTOR, LOW);
    return 1;
}

int battery(String command) {
    int val = analogRead(A0);
    Serial.print("BATTERY ");
    Serial.println(val);
    return val;
    
}
