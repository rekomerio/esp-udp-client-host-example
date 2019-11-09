#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define CLIENT_PORT   7001
#define HOST_PORT     7171
#define PING_INTERVAL 1000

#define MAX_CLIENTS   1

struct Message {
  uint8_t brightness;
  uint8_t sparking;
  uint8_t cooling;
};

struct UDPClient {
  uint32_t lastReceived;
  IPAddress ip;

  UDPClient() : lastReceived(0) {}
};

Message message;
UDPClient clients[MAX_CLIENTS];

WiFiUDP UDP;

const char *ssid = "UDP_host";
const char *pass = "topsecret343";

const uint8_t messageSize = sizeof(message);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, pass);

  IPAddress myIP = WiFi.softAPIP();
  Serial.println(myIP);
  // Start listening to this port for incoming messages
  if (UDP.begin(HOST_PORT)) {
    Serial.println("UDP setup successful");
  } else {
    Serial.println("UDP setup unsuccessful");
  }
}

void loop() {
  readIncomingData();
  sendDataToClients();
  delay(50);
}

void readIncomingData() {
  uint8_t packetSize = UDP.parsePacket();

  if (packetSize) {
    uint8_t id; // Clients id in the network
    uint8_t len = UDP.read(&id, 1);

    if (len) {
      Serial.printf("UDP client id: %d\n", id);
      if (id < MAX_CLIENTS) {
        clients[id].lastReceived = millis();
        clients[id].ip = UDP.remoteIP();
      } else {
        Serial.println("Invalid id");
      }
    }
  }
}

void sendDataToClients() {
  for (uint8_t i = 0; i < MAX_CLIENTS; i++) {
    // If client has not pinged host in one second or ip is null, client is assumed to be unconnected
    bool clientIsConnected = (clients[i].ip && (uint32_t)(millis() - clients[i].lastReceived) < PING_INTERVAL);

    if (clientIsConnected) {
      message.brightness = 200;
      message.sparking   = 80;
      message.cooling    = 50;

      if (UDP.beginPacket(clients[i].ip, CLIENT_PORT)) {
        UDP.write((char *)&message, messageSize);
        UDP.endPacket();
      } else {
        Serial.println("Problem with the connection");
      }
    }
  }
}
