

/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "mbed.h"
#include "BLEDevice.h"
 
//To use LCD
#include "uLCD_4DGL.h"
 
//UART Primary Service
#include "UARTService.h"
 
//Battery and DeviceInformation Auxilary Services
#include "BatteryService.h"
#include "DeviceInformationService.h"
 
BLEDevice  ble;
DigitalOut led1(LED1);
uLCD_4DGL uLCD(P0_9,P0_11,P0_1);
 
//Using the standard GATT Service IDs.
static const uint16_t uuid16_list[]        = {GattService::UUID_BATTERY_SERVICE,
                                              GattService::UUID_DEVICE_INFORMATION_SERVICE};
                                              
//Used to access the defined in main UARTService object globally.
UARTService *uartServicePtr;
 
//Keeping a receive buffer to alter the data received.
uint8_t DatatoSend[1000];
 
void disconnectionCallback(Gap::Handle_t handle, Gap::DisconnectionReason_t reason)
{
    //Print the LCD messages.
    uLCD.cls();
    uLCD.printf("\n Disconnected.!!\n");
    uLCD.printf("\n Restarting the advertising process.");
    ble.startAdvertising();
}
 
void onDataWritten(const GattCharacteristicWriteCBParams *params)
{
    if ((uartServicePtr != NULL) && (params->charHandle == uartServicePtr->getTXCharacteristicHandle())) {
        uint16_t bytesRead = params->len;
 
        uLCD.cls();
        uLCD.printf("Data Received.!!\n");
        
        for(int j=0;j<bytesRead;j++)
        {
            uLCD.printf(" %x\n",*((params->data)+j));
            DatatoSend[j]=(*((params->data)+j))+1;
        }
        wait(1);
        
        ble.updateCharacteristicValue(uartServicePtr->getRXCharacteristicHandle(), DatatoSend, bytesRead);
 
        //Use the below statement for loopback.
        //ble.updateCharacteristicValue(uartServicePtr->getRXCharacteristicHandle(), params->data, bytesRead);
        
        uLCD.cls();
        uLCD.printf("Data Sent.!!\n");
        for(int j=0;j<bytesRead;j++)
        {
            uLCD.printf(" %x\n",DatatoSend[j]);
        }
        wait(1);
    }
}
 
void periodicCallback(void)
{
    led1 = !led1;
}
 
int main(void)
{
    uLCD.reset();    
    uLCD.printf("\n Hello...\n"); //Default Green on black text
    uLCD.printf("\n Starting BLE-UART Demo...");
    
    led1 = 1;
    Ticker ticker;
    ticker.attach(periodicCallback, 1);
 
    ble.init();
    ble.onDisconnection(disconnectionCallback);
    ble.onDataWritten(onDataWritten);
 
    /* Set up Primary service*/
    UARTService uartService(ble);
    uartServicePtr = &uartService;
    
    /* Setup auxiliary services. */
    BatteryService           battery(ble);
    DeviceInformationService deviceInfo(ble, "ARM", "Model1", "SN1", "hw-rev1", "fw-rev1", "soft-rev1");
 
 
    /* Setup advertising */
    ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME,
                                     (const uint8_t *)"BLE UART", sizeof("BLE UART") - 1);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::INCOMPLETE_LIST_128BIT_SERVICE_IDS,
                                     (const uint8_t *)UARTServiceUUID_reversed, sizeof(UARTServiceUUID_reversed));
    ble.accumulateAdvertisingPayload(GapAdvertisingData::INCOMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));                               
    ble.setAdvertisingInterval(160); /* 100ms; in multiples of 0.625ms. */
    ble.startAdvertising();
    uLCD.text_width(1);
    uLCD.text_height(1);
    uLCD.color(GREEN);
    
    uLCD.cls();
    uLCD.locate(0,2);
    uLCD.printf("Started Advertising.!! \n");
    wait(1);
        
    while (true) {
        
        uLCD.cls();
        uLCD.locate(0,2);
        uLCD.printf("\nWaiting to receive Data.\n");  
        ble.waitForEvent();
    }
}