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
#include "ble/BLE.h"
#include "ble/services/UARTService.h"
#include "Serial.h"
 
#define UART_BUFFER (UARTService::BLE_UART_SERVICE_MAX_DATA_LEN*10)
 
const static char DEVICE_NAME[] = "UART";
UARTService *uartServicePtr;
static uint8_t uartBuff[UART_BUFFER];
static uint8_t uartBuffPos=0;
 
void onBleError(ble_error_t error);
 
DigitalOut led1(LED1);
Serial uart1(USBTX,USBRX);
Ticker ticker;
 
#define DEBUG uart1.printf
 
/* Ticker */
void periodicCallback(void)
{
    led1 = !led1;
}
 
/* UART */
void uartRx(void)
{
    if(uart1.readable()){
        uartBuff[uartBuffPos] = uart1.getc();
        if((uartBuff[uartBuffPos] == '\r') || (uartBuff[uartBuffPos] == '\n') || (uartBuffPos >= UART_BUFFER)) {
            uartBuff[uartBuffPos] = '\0';
            /* We are sending the whole string even if less than BLE_UART_SERVICE_MAX_DATA_LEN otherwise we need to wait */
            uartServicePtr->write(uartBuff, (uartBuffPos/UARTService::BLE_UART_SERVICE_MAX_DATA_LEN +1) * UARTService::BLE_UART_SERVICE_MAX_DATA_LEN);
            DEBUG("TX : %s\r\n", uartBuff);
            memset(uartBuff, 0, UART_BUFFER);
            uartBuffPos = 0;
        }
        else
            uartBuffPos++;
    }
}
 
 
/* BLE */
void BleConnectionCallback(const Gap::ConnectionCallbackParams_t *params) {
    DEBUG("BLE Client Connected!\n\r");
    DEBUG("Please type a string and press return\r\n");
 
    ticker.attach(periodicCallback, 1);
}
 
void BleDisconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    (void)params;
    DEBUG("BLE Client Disconnected!\r\n");
    ticker.detach();
    BLE::Instance().gap().startAdvertising(); // restart advertising
    led1=0;
}
 
void BleOnDataWrittenCallback(const GattWriteCallbackParams *params) {
    if (params->handle == uartServicePtr->getTXCharacteristicHandle()){
        DEBUG("RX: %s\r\n", params->data);
    }
}
 
void onBleError(ble_error_t error) { 
    DEBUG("BLE Error: %d\r\n");
    /* Handle error now */
} 
 
void bleInitComplete(BLE::InitializationCompleteCallbackContext *params) 
{
    BLE&        ble   = params->ble;
    ble_error_t error = params->error;
 
    if (error != BLE_ERROR_NONE) {
        /* In case of error, forward the error handling to onBleInitError */
        onBleError(error);
        return;
    }
 
    /* Ensure that it is the default instance of BLE */
    if(ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        return;
    }
 
    ble.gap().onConnection(BleConnectionCallback);
    ble.gap().onDisconnection(BleDisconnectionCallback);
    ble.gattServer().onDataWritten(BleOnDataWrittenCallback);
 
    DEBUG("BLE UARTService: ");
    /* Setup primary service. */
    UARTService uartService(ble);
    uartServicePtr = &uartService;
    DEBUG("Started\r\n");
 
    /* setup advertising */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)UARTServiceUUID_reversed, sizeof(UARTServiceUUID_reversed));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(500); /* 500ms. */
    ble.gap().startAdvertising();
 
    while (true) {
        ble.waitForEvent();
    }
}
 
int main(void)
{
    led1=0;
        
    uart1.baud(9600);
    uart1.attach(uartRx,Serial::RxIrq);
    BLE &ble = BLE::Instance();
    ble.init(bleInitComplete);
}
 
 
          