#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic* pCharacteristic = NULL;
BLEServer *pServer = NULL;

String glObjectsName[50];
int glObjectsRSSI[50];
int glObjectsInField[50];
int glRunCount = 0;
String glDeviceName = "2UVBEACON_001_X";

byte mac[6];



class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("A device has connected");
  };

  void onDisconnect(BLEServer* pServer) {
    Serial.println("A device has disconnected");
    pServer->startAdvertising();  // Start advertising again to allow new connections
  }
};



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
  //------------- Serial ---------------------------------
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.println(F("Setup is finisched"));

  BLEDevice::init(glDeviceName);
  BLEAddress myAddress = BLEDevice::getAddress();
  Serial.print("BLE MAC Address: ");
  Serial.println(myAddress.toString().c_str());

  pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pServer->setCallbacks(new MyServerCallbacks());  
  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                         );


  pCharacteristic->setCallbacks(new MyCallbackHandler());

                                         
  pCharacteristic->setValue("Hello from Server");
  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);  
  pAdvertising->start();
  Serial.println(F("Waiting a client connection to notify..."));
  
  for (int i = 0; i < 50; i++)
  {
    glObjectsName[i] = String("");  
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(F("Scanning will start"));
  scan_Networks();
  delay(1000);   

  //Take into account how much it runs every 10 sec
  
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
      loMessage = loMessage + "<device><name>" + glObjectsName[i] + "</name><rssi>" + String(glObjectsRSSI[i]) + "<rssi><time>" + String(glObjectsInField[i]) + "</time>";        
    }  
  }
  loMessage = loMessage + "</devices></root>";
  Serial.println(loMessage);
  //pCharacteristic->writeValue(loMessage);
  pCharacteristic->setValue(loMessage.c_str());
}
