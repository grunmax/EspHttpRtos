#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <SoftwareSerial.h>

#include <settings.h>

#define SERVICE_UUID        "5365f6fa-d6b8-11ec-9d64-0242ac120002"
#define CHARACTERISTIC_UUID "8e161fe2-dc7e-11ed-afa1-0242ac120002"
#define CHARACTERISTIC2_UUID "87e7b572-dc8d-11ed-afa1-0242ac120002"

Preferences pref;

BLEServer *bleserver;
BLEService *service;
BLECharacteristic *characteristic1;
BLECharacteristic *characteristic2;
BLEDescriptor *descriptor1;
BLEDescriptor *descriptor2;

bool deviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Device connected");
    };
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Device disconnected");
    }
};

class CharacteristicCallBack : public BLECharacteristicCallbacks
{
public:
  void onWrite(BLECharacteristic *pCharacteristic) override
  {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0)
    {
      Serial.print("New ssid value: ");
      Serial.println(value.c_str());
      putSsidSettings(String(value.c_str()));
    }
  }
};

class CharacteristicCallBack2 : public BLECharacteristicCallbacks
{
public:
  void onWrite(BLECharacteristic *pCharacteristic) override
  {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0)
    {
      Serial.print("New pass value: ");
      Serial.println(value.c_str());
      putPasswordSettings(String(value.c_str()));
    }
  }
};

void bleServerSetup()
{
  BLEDevice::init("PileusBLE");
  BLEDevice::setPower(ESP_PWR_LVL_P3);
  bleserver = BLEDevice::createServer();
  bleserver->setCallbacks(new MyServerCallbacks());
  service = bleserver->createService(SERVICE_UUID);

  characteristic1 = new BLECharacteristic(CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE |
                                         BLECharacteristic::PROPERTY_NOTIFY);                                      
  characteristic1->setCallbacks(new CharacteristicCallBack());                                          

  descriptor1 = new BLEDescriptor((uint16_t)0x2901);
  descriptor1->setAccessPermissions(BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ);
  descriptor1->setValue("ssid");
  characteristic1->addDescriptor(descriptor1);

  characteristic2 = service->createCharacteristic(CHARACTERISTIC2_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE |
                                         BLECharacteristic::PROPERTY_NOTIFY);
  characteristic2->setCallbacks(new CharacteristicCallBack2());  

  descriptor2 = new BLEDescriptor((uint16_t)0x2901);
  descriptor2->setAccessPermissions(BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ);
  descriptor2->setValue("password");
  characteristic2->addDescriptor(descriptor2);

  service->addCharacteristic(characteristic1);
  service->addCharacteristic(characteristic2);
  service->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  const char *macAddress_ = BLEDevice::getAddress().toString().c_str();
  Serial.print("BLE started: ");
  Serial.println(macAddress_);
}