
//characteristic and service UUID's are initialized here

//UUID's
uint16_t customServiceUUID  = 0xA000;
uint16_t readCharUUID       = 0xA001;
uint16_t writeCharUUID      = 0xA002;


//Set_ServiceID's
static const uint16_t uuid16_list[]        = {0xFFFF};

ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list)); // UUID's broadcast in advertising packet


// Set Up custom Characteristics
static uint8_t readValue[10] = {0};
ReadOnlyArrayGattCharacteristic<uint8_t, sizeof(readValue)> readChar(readCharUUID, readValue);
 
static uint8_t writeValue[10] = {0};
WriteOnlyArrayGattCharacteristic<uint8_t, sizeof(writeValue)> writeChar(writeCharUUID, writeValue);
 
// Set up custom service
GattCharacteristic *characteristics[] = {&readChar, &writeChar};
GattService        customService(customServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic *));

//Next we setup the writeCharCallback() function that will be called when a BLE onDataWritten event is triggered 
//(when someone writes to a characteristic on the device). This function will check to see what characteristic is being 
//written to and then handle it appropriately. In this case there is only one writable characteristic so its redundant, 
//but good form for more complex service/characteristic combinations.

//On_Characteristic_written_to_callback
void writeCharCallback(const GattCharacteristicWriteCBParams *params)
{
    // check to see what characteristic was written, by handle
    if(params->charHandle == writeChar.getValueHandle()) {
        // toggle LED if only 1 byte is written
        if(params->len == 1) {
            led = params->data[0];
            //...
        }
        // print the data if more than 1 byte is written
        else {
            printf("\n\r Data received: length = %d, data = 0x",params->len); 
            
            }
        // update the readChar with the value of writeChar
        ble.updateCharacteristicValue(readChar.getValueHandle(),params->data,params->len);
    }
}

// register callback function
ble.onDataWritten(writeCharCallback);

// register the service with the BLE object so it is available.
//Add_Service_to_BLE_Object

    // add our custom service
    ble.addService(customService);

