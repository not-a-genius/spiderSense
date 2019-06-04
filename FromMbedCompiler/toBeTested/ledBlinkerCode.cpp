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
#include "ble/DiscoveredCharacteristic.h"
#include "ble/DiscoveredService.h"
 
#define SCAN_INT  0x30 // 30 ms = 48 * 0.625 ms
#define SCAN_WIND 0x30 // 30 ms = 48 * 0.625 ms
 
 
const Gap::Address_t  BLE_address_BE       = {0xCC, 0x00, 0x00, 0xE1, 0x80, 0x02};
const Gap::Address_t  BLE_peer_address_BE  = {0xFD, 0x66, 0x05, 0x13, 0xBE, 0xBA};
 
DiscoveredCharacteristic ledCharacteristic;
 
uint8_t toggledValue = 0;
enum {
  READ = 0,
  WRITE,
  IDLE
};
static volatile unsigned int triggerOp = IDLE;
 
void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    (void)params;
    printf("disconnected\n\r");
}
 
void advertisementCallback(const Gap::AdvertisementCallbackParams_t *params) {
    if (params->peerAddr[0] != BLE_peer_address_BE[0]) {
        return;
    }
    
    printf("adv peerAddr[%02x %02x %02x %02x %02x %02x] rssi %d, isScanResponse %u, AdvertisementType %u\r\n",
           params->peerAddr[5], params->peerAddr[4], params->peerAddr[3], params->peerAddr[2], params->peerAddr[1], params->peerAddr[0],
           params->rssi, params->isScanResponse, params->type);
    
    if(!params->isScanResponse) {
      BLE::Instance().gap().connect(params->peerAddr, Gap::ADDR_TYPE_PUBLIC, NULL, NULL);
    }
}
 
void discoveryTerminationCallback(Gap::Handle_t connectionHandle) {
    printf("terminated SD for handle %u\r\n", connectionHandle);
}
 
void serviceDiscoveryCallback(const DiscoveredService *service) {
    if (service->getUUID().shortOrLong() == UUID::UUID_TYPE_SHORT) {
        printf("S UUID-%x attrs[%u %u]\r\n", service->getUUID().getShortUUID(), service->getStartHandle(), service->getEndHandle());
    } else {
        printf("S UUID-");
        const uint8_t *longUUIDBytes = service->getUUID().getBaseUUID();
        for (unsigned i = 0; i < UUID::LENGTH_OF_LONG_UUID; i++) {
            printf("%02X", longUUIDBytes[i]);
        }
        printf(" attrs[%u %u]\r\n", service->getStartHandle(), service->getEndHandle());
    }
}
 
void characteristicDiscoveryCallback(const DiscoveredCharacteristic *characteristicP) {
    //printf("  C UUID-%x valueAttr[%u] props[%x]\r\n", characteristicP->getShortUUID(), characteristicP->getValueHandle(), (uint8_t)characteristicP->getProperties().broadcast());
    if (characteristicP->getUUID().getShortUUID() == 0xa001) { /* !ALERT! Alter this filter to suit your device. */
      //printf("  C UUID-%x valueAttr[%u] props[%x]\r\n", characteristicP->getShortUUID(), characteristicP->getValueHandle(), (uint8_t)characteristicP->getProperties().broadcast());
      ledCharacteristic = *characteristicP;
      triggerOp = READ;
    }
}
 
void connectionCallback(const Gap::ConnectionCallbackParams_t *params) {
  uint16_t LED_SERVICE_UUID = 0xA000;
  uint16_t LED_STATE_CHARACTERISTIC_UUID = 0xA001;
  
  if (params->role == Gap::CENTRAL) {
    BLE &ble = BLE::Instance();
    ble.gattClient().onServiceDiscoveryTermination(discoveryTerminationCallback);
    ble.gattClient().launchServiceDiscovery(params->handle, serviceDiscoveryCallback, characteristicDiscoveryCallback, LED_SERVICE_UUID, LED_STATE_CHARACTERISTIC_UUID);
  }
}
 
void triggerToggledWrite(const GattReadCallbackParams *response) {
  if (response->handle == ledCharacteristic.getValueHandle()) {
#if 0
    printf("triggerToggledWrite: handle %u, offset %u, len %u\r\n", response->handle, response->offset, response->len);
    for (unsigned index = 0; index < response->len; index++) {
      printf("%c[%02x]", response->data[index], response->data[index]);
    }
    printf("\r\n");
#endif
    
    toggledValue = response->data[0] ^ 0x1;
    triggerOp = WRITE;
  }
}
 
void triggerRead(const GattWriteCallbackParams *response) {
  if (response->handle == ledCharacteristic.getValueHandle()) {
    triggerOp = READ;
  }
}
 
/** 
 * This function is called when the ble initialization process has failled 
 */ 
void onBleInitError(BLE &ble, ble_error_t error) 
{ 
    /* Initialization error handling should go here */ 
}
 
/** 
 * Callback triggered when the ble initialization process has finished 
 */ 
void bleInitComplete(BLE::InitializationCompleteCallbackContext *params) 
{
    BLE&        ble   = params->ble;
    ble_error_t error = params->error;
 
    if (error != BLE_ERROR_NONE) {
        /* In case of error, forward the error handling to onBleInitError */
        onBleInitError(ble, error);
        return;
    }
 
    /* Ensure that it is the default instance of BLE */
    if(ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        return;
    }
 
    // Set BT Address
    ble.gap().setAddress(BLEProtocol::AddressType::PUBLIC, BLE_address_BE);
 
    ble.gap().onConnection(connectionCallback);
    ble.onDisconnection(disconnectionCallback);
       
    ble.gattClient().onDataRead(triggerToggledWrite);
    ble.gattClient().onDataWrite(triggerRead);
 
    ble.gap().setScanParams(SCAN_INT, SCAN_WIND);
    ble.gap().startScan(advertisementCallback);
 
    // infinite loop
    while (1) {
      if (!ble.gattClient().isServiceDiscoveryActive()) {
        switch(triggerOp) {
        case READ:
          triggerOp = IDLE;
          ledCharacteristic.read();
          break;
        case WRITE:
          triggerOp = IDLE;
          ledCharacteristic.write(1, &toggledValue);
          break;
        }
      }
      ble.waitForEvent();
    }
}
 
int main(void)
{
    BLE::Instance().init(bleInitComplete);
}
         