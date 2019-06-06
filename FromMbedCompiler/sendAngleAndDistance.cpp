/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
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
#include "HCSR04.h"
#include "Servo.h"
#include "ble/BLE.h"
#include "ble/services/HeartRateService.h"
#include "BatteryService.h"

#define LOW 0
#define HIGH 1

#if !defined(IDB0XA1_D13_PATCH)
DigitalOut led1(LED1, 1);   // LED conflicts SPI_CLK in case of D13 patch
#endif  

const static char     DEVICE_NAME[]        = "SpiderSense";
static const uint16_t uuid16_list[]        = {GattService::UUID_HEART_RATE_SERVICE, GattService::UUID_BATTERY_SERVICE};

static volatile bool  triggerSensorPolling = false;

//Define Serial object
Serial pc(USBTX, USBRX);

// Defines Tirg and Echo pins of the Ultrasonic Sensor
HCSR04 sensor = HCSR04(D9, D8);

Servo myServo(D10);

// Define pins for LEDs
DigitalOut greenLED(D4);
DigitalOut redLED(D2);

int calculateDistance() {
    // Function for calculating the distance measured by the Ultrasonic sensor
    return sensor.distance(CM); //TODO make it long
}

void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params) {
    (void)params;
    BLE::Instance().gap().startAdvertising(); // restart advertising
}

void periodicCallback(void) {
#if !defined(IDB0XA1_D13_PATCH)
    led1 = !led1; /* Do blinky on LED1 while we're waiting for BLE events */
#endif

    /* Note that the periodicCallback() executes in interrupt context, so it is safer to do
     * heavy-weight sensor polling from the main thread. */
    triggerSensorPolling = true;
}

void onBleInitError(BLE &ble, ble_error_t error) {
    (void)ble;
    (void)error;
   /* Initialization error handling should go here */
}

void bleInitComplete(BLE::InitializationCompleteCallbackContext *params) {
    BLE&        ble   = params->ble;
    ble_error_t error = params->error;

    if (error != BLE_ERROR_NONE) {
        onBleInitError(ble, error);
        return;
    }

    if (ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        return;
    }

    ble.gap().onDisconnection(disconnectionCallback);

    /* Setup primary service. */
    uint8_t distance = 60; // init HRM to 60bps
    HeartRateService distanceService(ble, distance, HeartRateService::LOCATION_FINGER);
    
    uint8_t angle = 50;
    BatteryService angleService(ble, angle);

    /* Setup advertising. */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::GENERIC_HEART_RATE_SENSOR);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(50); /* 1000ms */
    ble.gap().startAdvertising();

    // infinite loop
    while (true) {
        // check for trigger from periodicCallback()
        if (ble.getGapState().connected) {

            // Do blocking calls or whatever is necessary for sensor polling.
            // In our case, we simply update the HRM measurement.
            // rotates the servo motor from 15 to 165 degrees
            for(int i=15; i<=165; i++) {
                myServo.SetPosition(500 + (i * 12.1212));
                distance = calculateDistance();
                angle = i;
                if(distance < 20) {
                    greenLED = LOW ;
                    redLED = HIGH;
                } else {
                    redLED = LOW ;
                    greenLED = HIGH ;
                }
                // update bps
                distanceService.updateHeartRate(distance);
                angleService.updateBatteryLevel(angle);
                wait_ms(100);
            }

        // Repeats the previous lines from 165 to 15 degrees
            for(int i=165; i>15; i--) {
                myServo.SetPosition(500 + (i * 12.1212));
                distance = calculateDistance();
                angle = i;
                if(distance < 20) {
                    greenLED = LOW ;
                    redLED = HIGH;
                } else {
                    redLED = LOW ;
                    greenLED = HIGH ;
                }
                distanceService.updateHeartRate(distance);
                angleService.updateBatteryLevel(angle);
                wait_ms(100);
            }
            

            

        } else {
            ble.waitForEvent(); // low power wait for event
        }
    }
}

int main(void) {
    myServo.Enable(1500,20000);
    Ticker ticker;
    ticker.attach(periodicCallback, 1); // blink LED every second

    BLE::Instance().init(bleInitComplete);
}

