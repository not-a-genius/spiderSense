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

#ifndef __BLE_ENVIRONMENTAL_SERVICE_H__
#define __BLE_ENVIRONMENTAL_SERVICE_H__

#include "ble/BLE.h"

/**
* @class EnvironmentalService
* @brief BLE Environmental Service. This service provides temperature, humidity and pressure measurement.
* Service:  https://developer.bluetooth.org/gatt/services/Pages/ServiceViewer.aspx?u=org.bluetooth.service.environmental_sensing.xml
* Temperature: https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.temperature.xml
* Humidity: https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.humidity.xml
* Pressure: https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.pressure.xml
*/
class MyService {
public:
    typedef int16_t  distanceType_t;
    typedef uint16_t angleType_t;
    typedef uint32_t PressureType_t;

    /**
     * @brief   EnvironmentalService constructor.
     * @param   ble Reference to BLE device.
     * @param   temperature_en Enable this characteristic.
     * @param   humidity_en Enable this characteristic.
     * @param   pressure_en Enable this characteristic.
     */
    MyService(BLE& _ble) :
        ble(_ble),
        distanceCharacteristic(GattCharacteristic::UUID_TEMPERATURE_CHAR, &distance),
        angleCharacteristic(GattCharacteristic::UUID_HUMIDITY_CHAR, &angle),
        pressureCharacteristic(GattCharacteristic::UUID_PRESSURE_CHAR, &pressure)
    {
        static bool serviceAdded = false; /* We should only ever need to add the information service once. */
        if (serviceAdded) {
            return;
        }

        GattCharacteristic *charTable[] = { &angleCharacteristic,
                                            &pressureCharacteristic,
                                            &distanceCharacteristic };

        GattService myService(GattService::UUID_ENVIRONMENTAL_SERVICE, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

        ble.gattServer().addService(myService);
        serviceAdded = true;
    }

    /**
     * @brief   Update humidity characteristic.
     * @param   newHumidityVal New humidity measurement.
     */
    void updateDistance(distanceType_t newDistanceVal)
    {
        distance = (distanceType_t) ( newDistanceVal );
        ble.gattServer().write(distanceCharacteristic.getValueHandle(), (uint8_t *) &distance, sizeof(distanceType_t));
    }

    /**
     * @brief   Update pressure characteristic.
     * @param   newPressureVal New pressure measurement.
     */
    void updatePressure(PressureType_t newPressureVal)
    {
        pressure = (PressureType_t) (newPressureVal * 10);
        ble.gattServer().write(pressureCharacteristic.getValueHandle(), (uint8_t *) &pressure, sizeof(PressureType_t));
    }

    /**
     * @brief   Update temperature characteristic.
     * @param   newTemperatureVal New temperature measurement.
     */
    void updateAngle(int newAngleVal)
    {
        angle = (int) ( newAngleVal );
        ble.gattServer().write(angleCharacteristic.getValueHandle(), (uint8_t *) &angle, sizeof(angle));
    }

private:
    BLE& ble;

    distanceType_t distance;
    angleType_t    angle;
    PressureType_t    pressure;

    ReadOnlyGattCharacteristic<distance> distanceCharacteristic;
    ReadOnlyGattCharacteristic<angleType_t>    angleCharacteristic;
    ReadOnlyGattCharacteristic<PressureType_t>    pressureCharacteristic;
};

#endif /* #ifndef __BLE_ENVIRONMENTAL_SERVICE_H__*/