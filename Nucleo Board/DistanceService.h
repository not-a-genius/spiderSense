#ifndef __BLE_DISTANCE_SERVICE_H__
#define __BLE_DISTANCE_SERVICE_H__

#include "ble/BLE.h"

#define UUID_DISTANCE_VALUE_CHAR 0x9A99
#define UUID_DISTANCE_SERVICE 0x990D

class DistanceService {
public:
   
    DistanceService(BLE &_ble, uint8_t value = 100) :
        ble(_ble),
        distance(value),
        distanceCharacteristic(UUID_DISTANCE_VALUE_CHAR, &distance, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY) {

        GattCharacteristic *charTable[] = {&distanceCharacteristic};
        GattService distanceService(UUID_DISTANCE_SERVICE, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

        ble.addService(distanceService);
    }
    void updateDistanceValue(uint8_t newValue) {
        distance = newValue;
        ble.gattServer().write(distanceCharacteristic.getValueHandle(), &distance, 1);
    }

protected:
    /**
     * A reference to the underlying BLE instance that this object is attached to.
     * The services and characteristics will be registered in this BLE instance.
     */
    BLE &ble;

    /**
     * The current distance.
     */
    uint8_t distance;
    /**
     * A ReadOnlyGattCharacteristic that allows access to the peer device to the
     * distance value through BLE.
     */
    ReadOnlyGattCharacteristic<uint8_t> distanceCharacteristic;
};

#endif