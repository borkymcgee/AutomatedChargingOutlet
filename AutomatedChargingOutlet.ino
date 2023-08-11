/**************************************************************************
* AutomatedChargingOutlet, Copyright (C) 2023 Juno Presken
* E-mail: juno.presken+AutomatedChargingOutlet@protonmail.com
*
* This program is free software: you can redistribute it and/or modify it
* under the terms of the GNU Affero General Public License as published by the
* Free Software Foundation, either version 3 of the License, or (at your
* option) any later version.

* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General
* Public License for more details.

* You should have received a copy of the GNU Affero General Public License
* along with this program. If not, see <https://www.gnu.org/licenses/>.
*
**************************************************************************/

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <NimBLEDevice.h>

#define RELAY_PIN 27
#define LED_PIN 23
#define BUTTON_PIN 0

//ms to snooze the relay when instructed
#define SNOOZE_DELAY 1800000
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"

static NimBLEUUID dataUuid(SERVICE_UUID);
static NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();

int my_gap_event_handler(ble_gap_event* event, void* param){
  Serial.println((int)event);
  snoozeCharging();
  return 0;
}

void setup() {

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  //turn on relay and LED by default
  digitalWrite(LED_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);

  Serial.begin(115200);
  Serial.println("Booting");

  //SerialBT.register_callback(BTCallback);
  WiFi.onEvent(WifiCallback, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STACONNECTED);
  
  NimBLEDevice::init("SnzChg");
    
  NimBLEServer *pServer = NimBLEDevice::createServer();

  BLEDevice::setCustomGapHandler(my_gap_event_handler);
  NimBLEService *pService = pServer->createService("ABCD");
  NimBLECharacteristic *pCharacteristic = pService->createCharacteristic("1234");
  
  pService->start();
  pCharacteristic->setValue("Hello BLE");
  
  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID("ABCD"); 
  pAdvertising->start(); 
  Serial.println("started bluetooth");

  WiFi.mode(WIFI_AP);
  WiFi.softAP("SnoozeCharging");
  ArduinoOTA.setHostname("AutoChargeOutlet");
  // ArduinoOTA.setPassword("admin");

ArduinoOTA
  .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle();
}

void WifiCallback(WiFiEvent_t event, WiFiEventInfo_t info){
    Serial.println("WiFi Connected!");
    snoozeCharging();
}

void snoozeCharging(){

  digitalWrite(LED_PIN, HIGH);
  digitalWrite(RELAY_PIN, HIGH);
  Serial.println("Charging Snoozed");

  delay(SNOOZE_DELAY);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);
  Serial.println("Charging Unsnoozed");
}
