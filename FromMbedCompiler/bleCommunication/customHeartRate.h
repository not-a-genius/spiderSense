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

#ifndef __BLE_HEART_RATE_SERVICE_H__
#define __BLE_HEART_RATE_SERVICE_H__

#include "ble/BLE.h"


/**
* @class HeartRateService
* @brief BLE Service for HeartRate. This BLE Service contains the location of the sensor and the heart rate in beats per minute.
* Service:  https://developer.bluetooth.org/gatt/services/Pages/ServiceViewer.aspx?u=org.bluetooth.service.heart_rate.xml
* HRM Char: https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.heart_rate_measurement.xml
* Location: https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.body_sensor_location.xml
*/
class HeartRateService {
public:
    /**
    * @enum SensorLocation
    * @brief Location of the heart rate sensor on body.
    */
    enum {
        LOCATION_OTHER = 0, /*!< Other location. */
        LOCATION_CHEST,     /*!< Chest. */
        LOCATION_WRIST,     /*!< Wrist. */
        LOCATION_FINGER,    /*!< Finger. */
        LOCATION_HAND,      /*!< Hand. */
        LOCATION_EAR_LOBE,  /*!< Earlobe. */
        LOCATION_FOOT,      /*!< Foot. */
    };

    enum{

        UUID_DISTANCE_CM = 0x33AA;
        UUID_ANGLE_DEGREES = 0x33BB;

    }

public:
    /**
     * @brief Constructor with 8-bit HRM Counter value.
     *
     * @param[ref] _ble
     *               Reference to the underlying BLE.
     * @param[in] hrmCounter (8-bit)
     *               Initial value for the HRM counter.
     * @param[in] location
     *               Sensor's location.
     */
    HeartRateService(BLE &_ble, uint8_t hrmCounter, uint8_t hrmCounter2, uint8_t location) :
        ble(_ble),
        hrmRate(GattCharacteristic::UUID_HEART_RATE_MEASUREMENT_CHAR, &hrmCounter,
                sizeof(hrmCounter), sizeof(hrmCounter),
                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY),
        hrmRate2(GattCharacteristic::UUID_HEART_RATE_MEASUREMENT_CHAR, &hrmCounter2,
        sizeof(hrmCounter2), sizeof(hrmCounter2),
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY),

        hrmLocation(GattCharacteristic::UUID_BODY_SENSOR_LOCATION_CHAR, &location),
        controlPoint(GattCharacteristic::UUID_HEART_RATE_CONTROL_POINT_CHAR, &controlPointValue) {
        setupService();
    }


    /**
     * @brief Set a new 8-bit value for the heart rate.
     *
     * @param[in] hrmCounter
     *                  Heart rate in BPM.
     */
    void updateHeartRate(uint8_t hrmCounter) {
        
        ble.gattServer().write(hrmRate.getValueHandle(), hrmCounter, sizeof(hrmCounter) );
    }

    void updateHeartRate2(uint8_t hrmCounter2) {
    
    ble.gattServer().write(hrmRate.getValueHandle(), hrmCounter2, sizeof(hrmCounter2) );
}

   


    /**
     * This callback allows the heart rate service to receive updates to the
     * controlPoint characteristic.
     *
     * @param[in] params
     *     Information about the characterisitc being updated.
     */
    virtual void onDataWritten(const GattWriteCallbackParams *params) {
        if (params->handle == controlPoint.getValueAttribute().getHandle()) {
            /* Do something here if the new value is 1; else you can override this method by
             * extending this class.
             * @NOTE: If you are extending this class, be sure to also call
             * ble.onDataWritten(this, &ExtendedHRService::onDataWritten); in
             * your constructor.
             */
        }
    }

protected:
    void setupService(void) {
        GattCharacteristic *charTable[] = {&hrmRate, &hrmRate2, &hrmLocation, &controlPoint};
        GattService         hrmService(GattService::UUID_HEART_RATE_SERVICE, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

        ble.addService(hrmService);
        ble.onDataWritten(this, &HeartRateService::onDataWritten);
    }


protected:
    BLE                 &ble;
    uint8_t              controlPointValue;

    GattCharacteristic                   hrmRate;
    GattCharacteristic                   hrmRate2;
    ReadOnlyGattCharacteristic<uint8_t>  hrmLocation;
    WriteOnlyGattCharacteristic<uint8_t> controlPoint;
};

#endif /* #ifndef __BLE_HEART_RATE_SERVICE_H__*/