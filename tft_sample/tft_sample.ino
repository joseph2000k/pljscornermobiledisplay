#include "rm67162.h"
#include <TFT_eSPI.h>
/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleWrite.cpp
    Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "NotoSansBold36.h"
#include "Final_Frontier_28.h"

#include "font.h"

#include "SuperSedan.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define led 38

BLECharacteristic *pCharacteristic;
bool isConnected = false;

std::string callbackString = "...";
std::string totalString= "";

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        
        std::string strValue = value;

        if (strValue == "0"){
          totalString = "";
          callbackString = "";
        } else {
          totalString ="Total    Amount";
          callbackString = strValue+".00";
        }
      }
    }
};


class BLEConnection : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      isConnected = true;
      digitalWrite(38, HIGH);  // Enable pin 38
    }

    void onDisconnect(BLEServer* pServer) {
      isConnected = false;
      digitalWrite(38, LOW);  // Disable pin 38
    }
};


void setup() {
  rm67162_init();  // amoled lcd initialization
  lcd_setRotation(1);
  sprite.createSprite(536, 240);
  sprite.setSwapBytes(1);
  pinMode(led, OUTPUT);

  Serial.begin(115200);

  BLEDevice::init("MyESP32");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new BLEConnection());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

void draw()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.setTextColor(TFT_PINK, TFT_BLACK);
  sprite.loadFont(SuperSedan);
  sprite.setTextSize(1);
  sprite.drawString(totalString.c_str(), 130,50,4);
  sprite.unloadFont();

  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
  sprite.setTextSize(2);
  sprite.drawString(callbackString.c_str(), 100, 100, 7);
 
 lcd_PushColors(0, 0, 536, 240, (uint16_t*)sprite.getPointer());
}

void loop() {
  draw();
}