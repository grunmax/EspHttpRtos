#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <SoftwareSerial.h>

#define SERVICE_UUID        "5365f6fa-d6b8-11ec-9d64-0242ac120002"
#define CHARACTERISTIC_UUID "61c7e6ae-d6b8-11ec-9d64-0242ac120002"

std::string bleValue = "Hey Joe";

class MyCallbacks: public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0)
    {
      Serial.print("New BLE value: ");
      Serial.println(value.c_str());
      bleValue = value;
    }
  }
};

void bleServerSetup()
{
  BLEDevice::init("MyBLEServer");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue(bleValue);
  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  Serial.println("BLE started");
}