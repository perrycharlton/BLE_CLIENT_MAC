    /////////////////////////////////////////////////////////////////
   //          ESP32 & Xiaomi Bluetooth  sensor      v1.00        //
  //       Get the latest version of the code here:              //
 //           http://educ8s.tv/esp32-xiaomi-hack                //
/////////////////////////////////////////////////////////////////
#include <string> 
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <Arduino.h>
// #include "HardwareSerial.h"
// #include "soc/soc.h"
// #include "soc/rtc_cntl_reg.h"
// #include "esp_system.h"
// #include <sstream>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define SCAN_TIME  10 // seconds

BLEScan *pBLEScan;

// static BLEAddress *pServerAddress;


float  current_humidity = -100;
float  previous_humidity = -100;
float current_temperature = -100;
float previous_temperature = -100;


class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        // Serial.print("BLE Advertised Device found: ");
        // Serial.println(advertisedDevice.toString().c_str());
        // Serial.println(advertisedDevice.getName().c_str());
        // pServerAddress = new BLEAddress(advertisedDevice.getAddress());

        if (advertisedDevice.haveName() && advertisedDevice.haveServiceData()) {
            std::string strServiceData = advertisedDevice.getServiceData();
            Serial.printf("ServiceData: %s\n", advertisedDevice.getServiceData().c_str());
            uint8_t cServiceData[100];
            char charServiceData[100];

            strServiceData.copy((char *)cServiceData, strServiceData.length(), 0);

            Serial.printf("\n\nAdvertised Device: %s\n", advertisedDevice.toString().c_str());

            for (int i=0;i<strServiceData.length();i++) {
                sprintf(&charServiceData[i*2], "%02x", cServiceData[i]);
                Serial.printf("Test: %02X %d\n", cServiceData[i], i);
            }

            std::stringstream ss;
            ss << "fe95" << charServiceData;
            
            Serial.print("Payload:");
            Serial.println(ss.str().c_str());


            // char eventLog[256];
            unsigned long value, value2;
            char charValue[5] = {0,};
            Serial.printf(" test: %02X\n", cServiceData[11]);
            switch (cServiceData[11]) {
                case 0x04:
                    sprintf(charValue, "%02X%02X", cServiceData[15], cServiceData[14]);
                    value = strtol(charValue, 0, 16);
                    current_temperature = (float)value/10; 
                    Serial.printf("Temp: %f", current_temperature);
                    break;
                case 0x06:
                    sprintf(charValue, "%02X%02X", cServiceData[15], cServiceData[14]);
                    value = strtol(charValue, 0, 16);  
                    current_humidity = (float)value/10;                     
                    Serial.printf("HUMIDITY_EVENT: %s, %lu\n", charValue, value);
                    // Serial.print(current_humidity);
                    break;
                case 0x0A:
                    sprintf(charValue, "%02X", cServiceData[14]);
                    value = strtol(charValue, 0, 16);                    
                    Serial.printf("BATTERY_EVENT: %s, %lu\n", charValue, value);
                    break;
                case 0x0D:
                    sprintf(charValue, "%02X%02X", cServiceData[15], cServiceData[14]);
                    value = strtol(charValue, 0, 16);                     
                    current_temperature = (float)value/10;                         
                    Serial.printf("TEMPERATURE_EVENT: %s, %lu\n", charValue, value);                    
                    Serial.printf(charValue, "%02X%02X", cServiceData[17], cServiceData[16]);
                    value2 = strtol(charValue, 0, 16);
                    current_humidity = (float)value2/10;
                    Serial.printf("HUMIDITY_EVENT: %s, %lu\n", charValue, value2);
                    break;
            }
        }
    }
};

void initBluetooth()
{
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
    pBLEScan->setInterval(0x50);
    pBLEScan->setWindow(0x30);
}

void setup() {

  Serial.begin(115200);
  Serial.println("ESP32 XIAOMI DISPLAY");
 
  initBluetooth();

}

void loop() {
  
    // char printLog[256];
    Serial.printf("Start BLE scan for %d seconds...\n", SCAN_TIME);
    BLEScanResults foundDevices = pBLEScan->start(SCAN_TIME);
    int count = foundDevices.getCount();
    printf("Found device count : %d\n", count);

    delay(100);
}



String convertFloatToString(float f)
{
  String s = String(f,1);
  return s;
}

float CelciusToFahrenheit(float Celsius)
{
 float Fahrenheit=0;
 Fahrenheit = Celsius * 9/5 + 32;
 return Fahrenheit;
}