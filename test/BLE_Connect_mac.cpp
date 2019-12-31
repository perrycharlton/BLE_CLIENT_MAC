
#include <BLEDevice.h>
#include <Arduino.h>   

// The remote service we wish to connect to.
static BLEUUID serviceUUID("226c0000-6476-4566-7562-66734470666d");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("226caa55-6476-4566-7562-66734470666d");

static BLEAddress MJAddress("58:2d:34:32:2c:c7");

static BLERemoteCharacteristic* pRemoteCharacteristic;
// static BLEAdvertisedDevice* myDevice;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.println((char*)pData);
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    Serial.println("on connect");
  }

  void onDisconnect(BLEClient* pclient) {
    // connected = false;
    Serial.println("onDisconnect");
  }
};

void connectToServer() {
  Serial.print("Forming a connection to ");

  // Create the client
  BLEClient* pClient = BLEDevice::createClient();
  // Connect the client to the server
  pClient->setClientCallbacks(new MyClientCallback());
  pClient->connect(MJAddress);
  Serial.println(pClient->isConnected());
  // Get a reference to a specific remote service on the server
  BLERemoteService* pMyRemoteService = pClient->getService(serviceUUID);
  // Get a reference to a specific remote characteristic owned by the service
  BLERemoteCharacteristic* pMyRemoteCharacteristic =  pMyRemoteService->getCharacteristic(charUUID);
  // Retrieve the current value of the remote characteristic.
  std::string myValue = pMyRemoteCharacteristic->readValue();
  pRemoteCharacteristic->registerForNotify(notifyCallback);
  
  Serial.println(myValue.c_str());
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  connectToServer();
} 

// This is the Arduino main loop function.
void loop() {
 
} // End of loop