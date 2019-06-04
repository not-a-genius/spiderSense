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
#include "ble/BLE.h"
#include "ble/services/HeartRateService.h"
#include "x_nucleo_iks01a1.h"
 
Serial pc(SERIAL_TX, SERIAL_RX);
 
DigitalOut led1(LED1, 1);
 
/* Instantiate the expansion board */
static X_NUCLEO_IKS01A1 *mems_expansion_board = X_NUCLEO_IKS01A1::Instance(D14, D15);
 
/* Retrieve the composing elements of the expansion board */
static GyroSensor *gyroscope = mems_expansion_board->GetGyroscope();
static MotionSensor *accelerometer = mems_expansion_board->GetAccelerometer();
static MagneticSensor *magnetometer = mems_expansion_board->magnetometer;
static HumiditySensor *humidity_sensor = mems_expansion_board->ht_sensor;
static PressureSensor *pressure_sensor = mems_expansion_board->pt_sensor;
static TempSensor *temp_sensor1 = mems_expansion_board->ht_sensor;
static TempSensor *temp_sensor2 = mems_expansion_board->pt_sensor;
 
/* Helper function for printing floats & doubles */
static char *printDouble(char* str, double v, int decimalDigits=2)
{
  int i = 1;
  int intPart, fractPart;
  int len;
  char *ptr;
 
  /* prepare decimal digits multiplicator */
  for (;decimalDigits!=0; i*=10, decimalDigits--);
 
  /* calculate integer & fractinal parts */
  intPart = (int)v;
  fractPart = (int)((v-(double)(int)v)*i);
 
  /* fill in integer part */
  sprintf(str, "%i.", intPart);
 
  /* prepare fill in of fractional part */
  len = strlen(str);
  ptr = &str[len];
 
  /* fill in leading fractional zeros */
  for (i/=10;i>1; i/=10, ptr++) {
    if(fractPart >= i) break;
    *ptr = '0';
  }
 
  /* fill in (rest of) fractional part */
  sprintf(ptr, "%i", fractPart);
 
  return str;
}
 
const static char     DEVICE_NAME[]        = "HRM1";
static const uint16_t uuid16_list[]        = {GattService::UUID_HEART_RATE_SERVICE};
 
static volatile bool  triggerSensorPolling = false;
 
void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    (void)params;
    BLE::Instance().gap().startAdvertising(); // restart advertising
}
 
void periodicCallback(void)
{
    led1 = !led1; /* Do blinky on LED1 while we're waiting for BLE events */
    /* Note that the periodicCallback() executes in interrupt context, so it is safer to do
     * heavy-weight sensor polling from the main thread. */
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
 
  uint8_t id;
  float value1, value2;
  char buffer1[32], buffer2[32];
  int32_t axes[3];
 
    
    pc.printf("1\n");
    if (error != BLE_ERROR_NONE) {
        onBleInitError(ble, error);
        return;
    }
    pc.printf("2\n");
    if (ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        return;
    }
    pc.printf("3\n");
    ble.gap().onDisconnection(disconnectionCallback);
 
    /* Setup primary service. */
    uint8_t hrmCounter = 100; // init HRM to 100bps
    HeartRateService hrService(ble, hrmCounter, HeartRateService::LOCATION_FINGER);
    pc.printf("4\n");
    /* Setup advertising. */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::GENERIC_HEART_RATE_SENSOR);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(1000); /* 1000ms */
    ble.gap().startAdvertising();
    pc.printf("5\n");
    
  humidity_sensor->ReadID(&id);
  pc.printf("HTS221  humidity & temperature    = 0x%X\r\n", id);
  pressure_sensor->ReadID(&id);
  pc.printf("LPS25H  pressure & temperature    = 0x%X\r\n", id);
  magnetometer->ReadID(&id);
  pc.printf("LIS3MDL magnetometer              = 0x%X\r\n", id);
  gyroscope->ReadID(&id);
  pc.printf("LSM6DS0 accelerometer & gyroscope = 0x%X\r\n", id);
  
  wait(3);
      
    // infinite loop
    while (true) {
        // check for trigger from periodicCallback()
    //pc.printf("6\n");        
        if (triggerSensorPolling && ble.getGapState().connected) {
            triggerSensorPolling = false;
 
            // Do blocking calls or whatever is necessary for sensor polling.
            // In our case, we simply update the HRM measurement.
            hrmCounter++;
 
            //  100 <= HRM bps <=175
            if (hrmCounter == 175) {
                hrmCounter = 100;
            }
 
            // update bps
            hrService.updateHeartRate(hrmCounter);
 
    temp_sensor1->GetTemperature(&value1);
    humidity_sensor->GetHumidity(&value2);
    pc.printf("HTS221: [temp] %7s°C,   [hum] %s%%\r\n", printDouble(buffer1, value1), printDouble(buffer2, value2));
    
    temp_sensor2->GetFahrenheit(&value1);
    pressure_sensor->GetPressure(&value2);
    pc.printf("LPS25H: [temp] %7s°F, [press] %smbar\r\n", printDouble(buffer1, value1), printDouble(buffer2, value2));
 
    pc.printf("---\r\n");
 
    magnetometer->Get_M_Axes(axes);
    pc.printf("LIS3MDL [mag/mgauss]:  %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);
 
    accelerometer->Get_X_Axes(axes);
    pc.printf("LSM6DS0 [acc/mg]:      %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);
 
    gyroscope->Get_G_Axes(axes);
    pc.printf("LSM6DS0 [gyro/mdps]:   %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);
            
        } else {
            ble.waitForEvent(); // low power wait for event
        }
    }
}
 
int main(void)
{
    
    Ticker ticker;
    pc.printf("Hello World !\n");        
    ticker.attach(periodicCallback, 1); // blink LED every second
 
    BLE::Instance().init(bleInitComplete);
}