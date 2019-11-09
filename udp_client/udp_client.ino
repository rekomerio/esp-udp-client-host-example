#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define CLIENT_PORT   7001
#define HOST_PORT     7171
#define PING_INTERVAL 500

#define NETWORK_ID    0

struct Message {
  uint8_t brightness;
  uint8_t sparking;
  uint8_t cooling;
};

Message message;
WiFiUDP UDP;

const char *ssid = "UDP_host";
const char *pass = "topsecret343";

const uint8_t id = NETWORK_ID;
const uint8_t messageSize = sizeof(message);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  WiFi.mode(WIFI_STA);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting...");
    delay(100);
  }
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  // Start listening to this port for incoming messages
  if (UDP.begin(CLIENT_PORT)) {
    Serial.println("UDP setup successful");
  } else {
    Serial.println("UDP setup unsuccessful");
  }
}

void loop() {
  static uint32_t lastSent = 0;
  readIncomingData();
  // Ping server every 500 ms to keep the connection alive
  if ((uint32_t)(millis() - lastSent) > PING_INTERVAL) {
    lastSent = millis();
    pingHost();
  }
}

void readIncomingData() {
  uint8_t packetSize = UDP.parsePacket();

  if (packetSize) {
    int16_t len = UDP.read((char *)&message, messageSize);

    if (len != -1) {
      Serial.printf("Brightness: %d\n", message.brightness);
      Serial.printf("Cooling: %d\n",    message.cooling);
      Serial.printf("Sparking: %d\n",   message.sparking);
    }
  }
}

void pingHost() {
  IPAddress host(192, 168, 4, 1);
  if (UDP.beginPacket(host, HOST_PORT)) {
    UDP.write(&id, 1);
    UDP.endPacket();
  } else {
    Serial.println("Problem with the connection");
  }
}
