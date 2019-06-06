#ifndef __BLE_ANGLE_SERVICE_H__
#define __BLE_ANGLE_SERVICE_H__

#include "ble/BLE.h"

#define UUID_ANGLE_VALUE_CHAR 0x9A90
#define UUID_ANGLE_SERVICE 0x990F

class AngleService {
public:
   
    AngleService(BLE &_ble, uint8_t value = 100) :
        ble(_ble),
        angle(value),
        angleCharacteristic(UUID_ANGLE_VALUE_CHAR, &angle, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY) {

        GattCharacteristic *charTable[] = {&angleCharacteristic};
        GattService angleService(UUID_ANGLE_SERVICE, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

        ble.addService(angleService);
    }
    void updateAngleValue(uint8_t newValue) {
        angle = newValue;
        ble.gattServer().write(angleCharacteristic.getValueHandle(), &angle, 1);
    }

protected:
    /**
     * A reference to the underlying BLE instance that this object is attached to.
     * The services and characteristics will be registered in this BLE instance.
     */
    BLE &ble;

    /**
     * The current angle.
     */
    uint8_t angle;
    /**
     * A ReadOnlyGattCharacteristic that allows access to the peer device to the
     * angle value through BLE.
     */
    ReadOnlyGattCharacteristic<uint8_t> angleCharacteristic;
};

#endif