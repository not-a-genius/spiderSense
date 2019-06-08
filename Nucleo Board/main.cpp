#include "mbed.h"
#include "HCSR04.h"
#include "Servo.h"
#include "ble/BLE.h"
#include "DistanceService.h"
#include "AngleService.h"

// Values of leds
#define LOW 0
#define HIGH 1

#if !defined(IDB0XA1_D13_PATCH)
DigitalOut led1(LED1, 1);   // LED conflicts SPI_CLK in case of D13 patch
#endif

#define UUID_ANGLE_SERVICE 0x990F
#define UUID_DISTANCE_SERVICE 0x990D

// Name of the device
const static char DEVICE_NAME[] = "SpiderSense";

// List of services
static const uint16_t uuid16_list[] = {UUID_DISTANCE_SERVICE, UUID_ANGLE_SERVICE};

static volatile bool  triggerSensorPolling = false;
static bool increment = true;
static double periodicCallbackTime = 0.2;
static int dangerDistance = 20;

// Defines Tirg and Echo pins of the Ultrasonic Sensor
HCSR04 sensor = HCSR04(D9, D8);

// Define pin for Servo
Servo myServo(D10);

// Define pins for LEDs
DigitalOut greenLED(D4);
DigitalOut redLED(D2);

// Function for calculating the distance measured by the Ultrasonic sensor
int calculateDistance() {
    return sensor.distance(CM);
}

// Callback function on disconnection
void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params) {
    (void)params;
    redLED = LOW;
    greenLED = LOW;
    BLE::Instance().gap().startAdvertising(); // restart advertising
}

// Periodic callback function
void periodicCallback(void) {
    #if !defined(IDB0XA1_D13_PATCH)
        led1 = !led1; // Do blinky on LED1 while waiting for BLE events
    #endif
    triggerSensorPolling = true;
}

// Function for handling initialization errors
void onBleInitError(BLE &ble, ble_error_t error) {
    (void)ble;
    (void)error;
}


// Function for initializing
void bleInitComplete(BLE::InitializationCompleteCallbackContext *params) {
    BLE& ble = params->ble;
    ble_error_t error = params->error;

    if (error != BLE_ERROR_NONE) {
        onBleInitError(ble, error);
        return;
    }

    if (ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        return;
    }

    ble.gap().onDisconnection(disconnectionCallback);

    // Setup distance and angle services
    uint8_t distance = 60;
    DistanceService distanceService(ble, distance);
    
    uint8_t angle = 15;
    AngleService angleService(ble, angle);

    // Setup advertising
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(50);
    ble.gap().startAdvertising();
    
    myServo.Enable(1500,20000);

    // infinite loop
    while (true) {
        // check for trigger from periodicCallback()
        if (triggerSensorPolling && ble.getGapState().connected) {
            triggerSensorPolling = false;
            
            // rotates the servo motor
            myServo.SetPosition(500 + (angle * 12.1212));  // It normalize the angle between 0 and 180 degrees
            distance = calculateDistance();
            if(distance < dangerDistance) {
                greenLED = LOW ;
                redLED = HIGH;
            } else {
                redLED = LOW ;
                greenLED = HIGH ;
            }
            distanceService.updateDistanceValue(distance);
            angleService.updateAngleValue(angle);
            if(increment) angle++;
            else angle--;
            if(angle == 15) increment = true;
            else if(angle == 165) increment = false;
        } else {
            ble.waitForEvent(); // Low power wait for event
        }
    }
}

int main(void) {
    Ticker ticker;
    ticker.attach(periodicCallback, periodicCallbackTime);
    BLE::Instance().init(bleInitComplete);
}

