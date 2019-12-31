/**
 * This sketch is the BLE GATT client for a halloween costume.
 * For more information on the project, check this post:
 *    https://jcjc-dev.com/2019/11/11/esp32-arduino-bluetooth-halloween-costume/
 *
 * Author: Juan Carlos Jimenez Caballero
 *
 * License: Fully Open Source. Use at your own peril
 * This firmware uses code found in Aruino's ESP32 BLE GATT example, and some
 * snippets from other unlicensed open source projects
 */

#include <Arduino.h>
#include "BLEDevice.h"

/* These must be the same as the server's firmware */

#define BLE_STORM_SERVICE_UUID "226c0000-6476-4566-7562-66734470666d"
#define BLE_PROP_CHARACTERISTIC_UUID "226caa55-6476-4566-7562-66734470666d"

/* The remote service we wish to connect to */
static BLEUUID serviceUUID(BLE_STORM_SERVICE_UUID);
/* The characteristic of the remote service we are interested in */
static BLEUUID charUUID(BLE_PROP_CHARACTERISTIC_UUID);

static boolean                  doConnect = false;
static boolean                  connected = false;
static boolean                  doScan    = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice*     myDevice;

static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic,
                           uint8_t* pData, size_t length, bool isNotify)
{
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    uint32_t value = 0;
    memcpy(&value, pData, sizeof(uint32_t));
    Serial.println(value);
}


class MyClientCallback : public BLEClientCallbacks
{
    void onConnect(BLEClient* pclient)
    {
    }

    void onDisconnect(BLEClient* pclient)
    {
        connected = false;
        Serial.println("onDisconnect");
    }
};

bool connectToServer()
{
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());

    BLEClient* pClient = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    /* Connect to the remove BLE Server */
    pClient->connect(myDevice); /* if you pass BLEAdvertisedDevice instead of
                                 * address, it will be recognized type of peer
                                 * device address (public or private) */
    Serial.println(" - Connected to server");

    /* Obtain a reference to the service we are after in the remote BLE server */
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if(pRemoteService == nullptr) {
        Serial.print("Failed to find our service UUID: ");
        Serial.println(serviceUUID.toString().c_str());
        pClient->disconnect();
        return false;
    }
    Serial.println(" - Found our service");


    /* Get a reference to the charact in the service of the remote BLE server */
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if(pRemoteCharacteristic == nullptr) {
        Serial.print("Failed to find our characteristic UUID: ");
        Serial.println(charUUID.toString().c_str());
        pClient->disconnect();
        return false;
    }
    Serial.println(" - Found our characteristic");

    /* Read the value of the characteristic */
    if(pRemoteCharacteristic->canRead()) {
        std::string value = pRemoteCharacteristic->readValue();
        Serial.print("The characteristic value was: ");
        Serial.println(value.c_str());
    }

    if(pRemoteCharacteristic->canNotify())
        pRemoteCharacteristic->registerForNotify(notifyCallback);

    connected = true;
    return true;
}

/**
 * Scan for BLE servers and find the first one that advertises the service we
 * are looking for.
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    /* Called for each advertising BLE server */
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        Serial.print("BLE Advertised Device found: ");
        Serial.println(advertisedDevice.toString().c_str());

        /* Device found. Check if it contains the service we are looking for */
        if(advertisedDevice.haveServiceUUID() &&
           advertisedDevice.isAdvertisingService(serviceUUID)) {
            BLEDevice::getScan()->stop();
            myDevice  = new BLEAdvertisedDevice(advertisedDevice);
            doConnect = true;
            doScan    = true;

        }
    }
};


void setup()
{
    /* Init serial comms with the development computer */
    Serial.begin(115200);


    /* Init Bluetooth stack */
    Serial.println("Starting Arduino BLE Client application...");
    BLEDevice::init("");

    /* Retrieve a Scanner and set the callback we want to use to be informed
     * when we have detected a new device.  Specify that we want active scanning
     * and start the scan to run for 5 seconds.
     */
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(5, false);
}

void loop()
{
    /* If the flag "doConnect" is true then we have scanned for and found the
     * desired BLE Server with which we wish to connect.  Now we connect to it.
     * Once we are connected we set the connected flag to be true.
     */
    if(doConnect == true) {
        if(connectToServer()) {
            Serial.println("We are now connected to the BLE Server.");
        } else {
            Serial.println(
                "We have failed to connect to the server; there is nothin "
                "more we will do.");
        }
        doConnect = false;
    }

    if(connected) {
    } else if(doScan) {
        /* this is just eample to start scan after disconnect, most
         * likely there is better way to do it in arduino
         */
        BLEDevice::getScan()->start(0);

    }
    delay(10);
}