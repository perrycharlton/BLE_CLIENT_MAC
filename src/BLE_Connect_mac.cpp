
#include "BLEDevice.h"
#include <Arduino.h>   

// The remote service we wish to connect to.
static BLEUUID serviceUUID("226c0000-6476-4566-7562-66734470666d");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("226caa55-6476-4566-7562-66734470666d");

// static BLEAddress MJAddress("58:2d:34:32:2c:c7");
// static BLEAddress MJAddress("58:2d:34:32:33:6e");
// static BLEAddress MJAddress;

static const int addressSize = 5;
char const* address[addressSize] = { "58:2d:34:32:33:6e", "58:2d:34:32:2c:c7", 
    "58:2d:34:32:3c:f3", "58:2d:34:32:3c:f3", "58:2d:34:32:2e:d4"};
int i = 0;
char const* addrs;
// static boolean doConnect = false;
static boolean connected = false;
static BLERemoteService* pRemoteService;
static BLERemoteCharacteristic* pMyRemoteCharacteristic;
// static boolean doScan = false;
// static BLERemoteCharacteristic* pRemoteCharacteristic;
// static BLEAdvertisedDevice* myDevice;

static BLEClient* pClient;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    // Serial.print("Notify callback for characteristic ");
    // Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    // Serial.print(" of data length ");
    // Serial.println(length);
    // Serial.print("data: ");
    // Serial.println((char*)pData);  
    Serial.printf("Connected to mac: %s\t Data: %s\n", addrs, pData);
     
    // pMyRemoteCharacteristic->registerForNotify(NULL, false);
    // delay(300); 
    // pClient->disconnect();

}

// class MyClientCallback : public BLEClientCallbacks {
//   void onConnect(BLEClient* pclient) {
//   }

//   void onDisconnect(BLEClient* pclient) {
//     connected = false;
//     // delete pClient;
//     // delete pclient;
//     Serial.println("onDisconnect");
//   }
// };

bool connectToServer(char const* Address) {
  BLEAddress MJAddress(Address);
  // pClient = BLEDevice::createClient();
  // Serial.println("Forming a connection to ");
  // Serial.printf("Is client connected %i\n", pClient->isConnected());
  // pClient->setClientCallbacks(new MyClientCallback());

  connected = pClient->connect(MJAddress);
   if(!connected){
      // delete pClient;
      // delete myDevice;
      return false;
    }

  // Serial.printf("Connected = %i - Connected to mac: %s\n", pClient->isConnected(), MJAddress.toString().c_str());
  // Serial.println(pClient->isConnected());

  // Get a reference to a specific remote service on the server
   // Obtain a reference to the service we are after in the remote BLE server.
  pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
  }
  
  // Serial.printf(" - Found our service - %s\n" , pRemoteService->toString().c_str());
  // Get a reference to a specific remote characteristic owned by the service
  
  
  pMyRemoteCharacteristic =  pRemoteService->getCharacteristic(charUUID);
  if (pMyRemoteCharacteristic == nullptr) {
    
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(charUUID.toString().c_str());
    pClient->disconnect();
  }
  pMyRemoteCharacteristic->registerForNotify(notifyCallback, true);
  // Retrieve the current value of the remote characteristic.
  delay(200);
  std::string myValue = pMyRemoteCharacteristic->readValue();
  // Serial.printf("Thes ae the values returned: %f\n", pMyRemoteCharacteristic->readFloat());
  // pMyRemoteCharacteristic->registerForNotify(notifyCallback, false);
  // Serial.printf("The notifcations value: %s\n\n", myValue.c_str());
    
  connected = true;
  return true;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");
    // Create the client
  pClient = BLEDevice::createClient();
} 

// This is the Arduino main loop function.
void loop() {
  // Serial.printf("Connection: %d\n", pClient->isConnected());
  // pClient->isConnected();
  if (!pClient->isConnected() ){
    delay(1000); 
    Serial.println("Reconnecting");
    addrs = address[i];
    connectToServer(address[i]);     
    
    i < (addressSize-1)?i++:i=0;
    Serial.printf("Reconnecting no = %i. Mac = %s\n", i, address[i]);

  }
  delay(1000); 
} // End of loop