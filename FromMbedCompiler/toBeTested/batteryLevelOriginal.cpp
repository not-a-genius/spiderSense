/* mbed Microcontroller Library
 * Copyright (c) 2006-2014 ARM Limited
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
#include "BLE.h"
#include "BatteryService.h"
 
DigitalOut led1(LED1, 1);
 
const static char     DEVICE_NAME[] = "BATTERY";
static const uint16_t uuid16_list[] = {GattService::UUID_BATTERY_SERVICE};
 
static volatile bool  triggerSensorPolling = false;
 
void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    (void)params;
    BLE::Instance().gap().startAdvertising(); // restart advertising
}
 
void blink(void)
{
    led1 = !led1;
    triggerSensorPolling = true;
}
 
void onBleInitError(BLE &ble, ble_error_t error)
{
    (void)ble;
    (void)error;
   /* Initialization error handling should go here */
}
 
void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
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
 
    uint8_t batteryLevel = 50;
    BatteryService batteryService(ble, batteryLevel);
 
    /* setup advertising */
    ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *) uuid16_list, sizeof(uuid16_list));
    ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *) DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.setAdvertisingInterval(1000); /* 1000ms; in multiples of 0.625ms. */
    ble.startAdvertising();
 
    while (true) {
        // check for trigger from periodicCallback()
        if (triggerSensorPolling && ble.getGapState().connected) {
            triggerSensorPolling = false;
 
            // the magic battery processing
            batteryLevel++;
            if (batteryLevel > 100) {
                batteryLevel = 20;
            }
 
            batteryService.updateBatteryLevel(batteryLevel);
        } else {
            ble.waitForEvent(); // low power wait for event
        }
    }
}
 
int main(void)
{
    Ticker t;
    t.attach(blink, 1.0f);
 
    BLE::Instance().init(bleInitComplete);
}
 
 
          