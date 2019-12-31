#include <BLEDevice.h>
#include <Arduino.h>

BLEClient* pClient;

#define MJ_ADDR "58:2d:34:32:2c:c7"
static BLEAddress MJAddress(MJ_ADDR);
static boolean connected = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
uint8_t* pData;
float temp;
float humi;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
// The remote service we wish to connect to.
static BLEUUID serviceUUID("226c0000-6476-4566-7562-66734470666d");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("226caa55-6476-4566-7562-66734470666d");

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
    for (int i = 0; i < length; i++){
      Serial.println(pData[i]);
      Serial.print(" ");

    }
    Serial.println();
    Serial.println(pData[3]);
    temp = ((pData[2]-48)*10 + pData[3]-48 + (pData[5]-48)*0.1);
    Serial.println(temp);
    humi = ((pData[9]-48)*10 + pData[10]-48 + (pData[12]-48)*0.1);
    Serial.println(humi);
  // this is where
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    Serial.println("on connect");
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

void connectToServer(BLEAddress pAddress){

  Serial.println(" - Created client");
  pClient->setClientCallbacks(new MyClientCallback());
  Serial.println("=================== Forming a connection to Flora device ====================");
  
  Serial.println(pAddress.toString().c_str());
  Serial.println(" - Connection to MJ");
  pClient->connect(pAddress);
  
  Serial.println(" - Connected to MJ");

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
    }
    Serial.println(" - Found our service");

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
    }

    /* Read the value of the characteristic */
    if(pRemoteCharacteristic->canRead()) {
        std::string value = pRemoteCharacteristic->readValue();
        Serial.print("The characteristic value was: ");
        Serial.println(value.c_str());
    }

    Serial.println(" - Found our characteristic");
    
    pRemoteCharacteristic->registerForNotify(notifyCallback);
}



void setup() {
  Serial.begin(115200);
  Serial.println("Starting MJ client...");
  delay(500);
  BLEDevice::init("");
  // pClient  = BLEDevice::createClient();
  // delay(5000);
  // timer.setInterval(600000L, getSensorData1);
  // getSensorData1();
  // delay(500)
  // connectToServer(MJAddress);
}

void loop() {
  if (!connected){
    pClient  = BLEDevice::createClient();
    delay(500);
    connectToServer(MJAddress);
  }
  // 
  // delay(500);  
}