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
 
#include <string.h>
#include "mbed.h"
#include "BLE.h"
 
#include "UARTService.h"
#include "BatteryService.h"
#include "DeviceInformationService.h" 

#include "Servo.h"
#include "HCSR04.h"
//#include "beep.h"


//useful defines
#define LOW 0
#define HIGH 1

//// flags
#define DEBUG 0
#define SERVO_DEBUG 0
#define LED_DEBUG 0
#define BUZZER_DEBUG 0
#define ULTRA_DEBUG 0


//Define Serial object
Serial pc(USBTX, USBRX);

// Defines Trig and Echo pins (TRIG,ECHO) of the Ultrasonic Sensor
HCSR04 sensor= HCSR04(D9, D8);


// Buzzer pin
//Beep buzzer = Beep(D6);

// Define pins for LEDs
DigitalOut blueLED(D13);
DigitalOut greenLED(D12);
DigitalOut redLED(D11);

// Variables for the duration and the distance
long duration;
int ultraDistance=-1;

// Creates a servo object for controlling the servo motor
Servo myServo(D2);

static const uint16_t uuid16_list[] = {GattService::UUID_BATTERY_SERVICE,
                                GattService::UUID_DEVICE_INFORMATION_SERVICE};

BLEDevice  ble;

//Used to access the defined in main UARTService object globally.
UARTService *uart;

//Keeping a receive buffer to alter the data received.
uint8_t DatatoSend[1000];

//-------------------------|||   RADAR FUNCTIONS     |||----------------------------------

// Function for calculating the distance measured by the Ultrasonic sensor
int calculateDistance()
{
    return sensor.distance(CM); //TODO make it long
}

//attempt to call a function in thread to buzz the buzzer
void buzzerThead()
{
    //buzzer.beep(1000, 0.5);
}

//function responsible to acquire data from radar,parseit,send it on uart
void printAngleAndDistance(int angle)
{
    myServo.write(angle/180.0);
    wait_ms(30);
    ultraDistance = calculateDistance();// Calls a function for calculating the distance measured by the Ultrasonic sensor for each degree
    wait_ms(0.1);
    pc.printf("%d",angle); // Sends the current degree into the Serial Port
    pc.printf(","); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
    pc.printf("%d",ultraDistance); // Sends the distance value into the Serial Port
    pc.printf("."); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
    // pc.printf("\n");

}



//----------------------------------------||| BLE |||--------------------------------------------------------------
 
void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    pc.printf("\n Disconnected.!!\n");
    pc.printf("\n Restarting the advertising process.\n");
    ble.startAdvertising();
}

//Called just to wake up from sleep
void periodicCallback(void)
{
}

void onDataWritten(const GattWriteCallbackParams *params)
{
    /*if received something, print it out on PC screen*/
    if ((uart != NULL) && (params->handle == uart->getTXCharacteristicHandle())) {
        uint16_t bytesRead = params->len;
 
        pc.printf("Data Received\n");
        /*for all received data, send a copy back to the BLE central(in this case the phone app)*/
        for(int j=0;j<bytesRead;j++)
        {
            pc.printf(" %x\n",*((params->data)+j));
            DatatoSend[j]=(*((params->data)+j));
        }
        
        wait(1);
        
        ble.updateCharacteristicValue(uart->getRXCharacteristicHandle(), DatatoSend, bytesRead);
        //Use the below statement for loopback.
        //ble.updateCharacteristicValue(uart->getRXCharacteristicHandle(), params->data, bytesRead);
        
        /*print out what have just been sent*/
        pc.printf("Data Sent\n");
        for(int j=0;j<bytesRead;j++)
        {
            pc.printf(" %x\n",DatatoSend[j]);
        }
        
        wait(1);
    }
}

int main(void)
{
    
    pc.baud(9600);
    pc.printf("Hello, starting BlueNRG Serial Console Demo!\n");
    Ticker ticker;
    ticker.attach(periodicCallback, 1);
 
    pc.printf("Initialising the module\n\r");
    ble.init();
    ble.onDisconnection(disconnectionCallback);
    ble.onDataWritten(onDataWritten);
    
    UARTService uartService(ble);
    uart = &uartService;
    
    BatteryService  battery(ble);
    DeviceInformationService deviceInfo(ble, "ARM", "Model1", "SN1", "hw-rev1", "fw-rev1", "soft-rev1");
 
    /* setup advertising */
    ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME,
                                     (const uint8_t *)"BLE UART", sizeof("BLE UART") - 1);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::INCOMPLETE_LIST_128BIT_SERVICE_IDS,
                                     (const uint8_t *)UARTServiceUUID_reversed, sizeof(UARTServiceUUID_reversed));
    ble.accumulateAdvertisingPayload(GapAdvertisingData::INCOMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));                               
    ble.setAdvertisingInterval(160); /* 100ms; in multiples of 0.625ms. */
    ble.startAdvertising();
    pc.printf("Start Advertising.\n");
 
    while (true) {
        ble.waitForEvent();
    }
}
