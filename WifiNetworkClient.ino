#include <Arduino.h>
#include <WiFi.h>

#include <WiFiClient.h>
#include <WiFiServer.h>

const char* paSsid = "2UVTracker_0002";
const char* paPassword = "password213";


IPAddress local_IP(192,168,4,10);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_AP); //Set mode of the Wifi
  delay(250);
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");  
  delay(250);
  Serial.println(WiFi.softAP(paSsid, paPassword) ? "Ready" : "Failed!");
  delay(1000);
  // Use your network credentials as SSID and password
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  //server.begin();
}
void loop() {
  delay(1000);
  Serial.println("Server running");
  Serial.print("NetworkID:");
  String current_ssid = WiFi.SSID();
  Serial.println(current_ssid);
  /*
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Send the HTTP response
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print("<html><body><h1>Hello from ESP32!</h1></body></html>");
            client.println();
            break; // Break out of the loop when the response is sent
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected");
  }
  */
}
