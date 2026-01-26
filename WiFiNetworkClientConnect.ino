/* Wi-Fi STA Connect and Disconnect Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

*/
#include <WiFi.h>

const char *ssid = "_LinksysSetupCBC";
const char *password = "vi1utmcc2ialienpass";

String glObjectsName[50];
int glObjectsRSSI[50];
int glObjectsInField[50];
int glRunCount = 0;
String glDeviceName = "2UVBEACON_001_Z";


void scan_Networks()
{
      int numberOfNetworks = WiFi.scanNetworks();
      for (int i = 0; i < numberOfNetworks; i++) 
      {
      
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.print(" dBm)");
        delay(10);
      
        bool addToList = true;
        for (int j = 0; j < 50; j++)
        {
          if (glObjectsName[j] == String(WiFi.SSID(i)))
          {
              glObjectsRSSI[j] = WiFi.RSSI(i);
              glObjectsInField[j] = glRunCount;
              addToList = false;
          }         
        }
        if (addToList)
        {
          for (int j = 0; j < 50; j++){
            if (glObjectsName[j] == String(""))
            {  
              glObjectsRSSI[j] = WiFi.RSSI(i);
              glObjectsName[j] = String(WiFi.SSID(i));
              glObjectsInField[j] = glRunCount;
              break;
            }
          }
        }
          
      }     
}

void setup() {
  Serial.begin(115200);
  delay(10);
  
  Serial.println();
  Serial.print("[WiFi] Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  // Auto reconnect is set true as default
  // To set auto connect off, use the following function
  WiFi.setAutoReconnect(true);
}

void loop() {

  // put your main code here, to run repeatedly:
  Serial.println(F("Scanning will start"));
  scan_Networks();

  
  // Reconnect if disconnected the WIFI
  if (WiFi.status() == WL_DISCONNECTED)
  {
    WiFi.begin(ssid, password);  
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("Local IP:" + WiFi.localIP());
    }
  }

  //Construct the string
  glRunCount = glRunCount + 1;
  if (glRunCount == 50000)
  {
    glRunCount = 1;
  }
  for (int i = 0; i < 50; i++)
  {
    if (glObjectsInField[i] < (glRunCount - 20))
    {
        glObjectsRSSI[i] = 0;
        glObjectsName[i] = String("");
        glObjectsInField[i] = 0;
    }         
  }
  String loMessage = "<root><master>" + glDeviceName + "</master><devices>";
  for (int i = 0; i < 50; i++)
  {
    if (glObjectsName[i] != String(""))
    {
        if (  String(glObjectsName[i][0]) == "2" &&
              String(glObjectsName[i][1]) == "U" &&
              String(glObjectsName[i][2]) == "V" &&
              String(glObjectsName[i][3]) == "T")      
                loMessage = loMessage + "<device><name>" + glObjectsName[i] + "</name><rssi>" + String(glObjectsRSSI[i]) + "</rssi><time>" + String(glObjectsInField[i]) + "</time></device>";        
    }  
  }
  loMessage = loMessage + "</devices></root>";

  //Send the string
  
  // Use NetworkClient class to create TCP connections
  NetworkClient client;
  const uint16_t port = 4000;
  const char *host = "192.168.1.231";

  if (!client.connect(host, port)) {
    Serial.println("Connection failed.");
    Serial.println("Waiting 5 seconds before retrying...");
    delay(1000);
    return;
  }
  client.print(loMessage);

  //delay(1000);
  
}
