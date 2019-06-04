#include "mbed.h"
//radar libs
#include "Servo.h"
#include "HCSR04.h"
//#include "beep.h"

//bluetooth libs
//#include "ble/BLE.h"
#include "ble/services/HeartRateService.h"

#if !defined(IDB0XA1_D13_PATCH)
DigitalOut led1(LED1, 1);   // LED conflicts SPI_CLK in case of D13 patch
#endif


//useful defines
#define LOW 0
#define HIGH 1

//// flags
#define DEBUG 0
#define SERVO_DEBUG 0
#define LED_DEBUG 0
#define BUZZER_DEBUG 0
#define ULTRA_DEBUG 0


//Define Serial object
Serial pc(USBTX, USBRX);

// Defines Trig and Echo pins (TRIG,ECHO) of the Ultrasonic Sensor
HCSR04 sensor= HCSR04(D9, D8);


// Buzzer pin
//Beep buzzer = Beep(D6);

// Define pins for LEDs
DigitalOut blueLED(D13);
DigitalOut greenLED(D12);
DigitalOut redLED(D11);

// Variables for the duration and the distance
long duration;
int ultraDistance=-1;

// Creates a servo object for controlling the servo motor
Servo myServo(D2);

//names and services for ble
const static char     DEVICE_NAME[]        = "SpiderSense";
static const uint16_t uuid16_list[]        = {GattService::UUID_HEART_RATE_SERVICE};

static volatile bool  triggerSensorPolling = false;

//-------------------------|||   RADAR FUNCTIONS     |||----------------------------------

// Function for calculating the distance measured by the Ultrasonic sensor
int calculateDistance()
{
    return sensor.distance(CM); //TODO make it long
}

//attempt to call a function in thread to buzz the buzzer
void buzzerThead()
{
    //buzzer.beep(1000, 0.5);
}

//function responsible to acquire data from radar,parseit,send it on uart
void printAngleAndDistance(int angle)
{
    myServo.write(angle/180.0);
    wait_ms(30);
    ultraDistance = calculateDistance();// Calls a function for calculating the distance measured by the Ultrasonic sensor for each degree
    wait_ms(0.1);
    pc.printf("%d",angle); // Sends the current degree into the Serial Port
    pc.printf(","); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
    pc.printf("%d",ultraDistance); // Sends the distance value into the Serial Port
    pc.printf("."); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
    // pc.printf("\n");

}

//function to check if there is obstacle or note based on ultraDistance value
void checkDistance()
{
    if(ultraDistance < 20) {
        greenLED = LOW ;
        redLED = HIGH;
        //buzzer.beep(1000, 0.5);
    } else {
        redLED = LOW ;
        greenLED = HIGH ;
    }

}



//-------------------------|||   BLE FUNCTIONS     |||----------------------------------

void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    pc.printf("[BLE] Disconnected. \n");
    (void)params;
    BLE::Instance().gap().startAdvertising(); // restart advertising
}

void periodicCallback(void)
{
#if !defined(IDB0XA1_D13_PATCH)
    led1 = !led1; /* Do blinky on LED1 while we're waiting for BLE events */
#endif

    /* Note that the periodicCallback() executes in interrupt context, so it is safer to do
     * heavy-weight sensor polling from the main thread. */
    triggerSensorPolling = true;
}

void onBleInitError(BLE &ble, ble_error_t error)
{
    pc.printf("[ERROR] onBleInitError \n");
    (void)ble;
    (void)error;
    /* Initialization error handling should go here */
}

void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
    pc.printf("[BLE] starting initialization \n");
    BLE&        ble   = params->ble;
    ble_error_t error = params->error;

    if (error != BLE_ERROR_NONE) {
        pc.printf("[ERROR] error != BLE_ERROR_NONE \n");
        onBleInitError(ble, error);
        return;
    }

    if (ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        pc.printf("[ERROR] ist id not default \n");
        return;
    }

    ble.gap().onDisconnection(disconnectionCallback);

    /* Setup primary service. */
    uint8_t hrmCounter = 60; // init HRM to 60bps
    HeartRateService hrService(ble, hrmCounter, HeartRateService::LOCATION_FINGER);

    /* Setup advertising. */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::GENERIC_HEART_RATE_SENSOR);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(1000); /* 1000ms */
    ble.gap().startAdvertising();
    pc.printf("[BLE] Setup complete \n");

    // infinite loop
    while (true) {
        // check for trigger from periodicCallback()
        bool isConnected=ble.getGapState().connected; 
        pc.printf("[INFO] poll: %d connected :%d \n",triggerSensorPolling, isConnected);
        if (triggerSensorPolling && isConnected) {
            triggerSensorPolling = false;

            // Do blocking calls or whatever is necessary for sensor polling.
            // In our case, we simply update the HRM measurement.
            hrmCounter++;

            //  60 <= HRM bps <= 100
            if (hrmCounter == 100) {
                hrmCounter = 60;
            }

            // update bps
            hrService.updateHeartRate(hrmCounter);
        } else {
            pc.printf("[BLE] waiting for events \n");
            ble.waitForEvent(); // low power wait for event
        }
        pc.printf("newRound\n");

        // rotates the servo motor from 15 to 165 degrees
        for(int i=15; i<=165; i++) {
            //pc.printf("moving: %d \n",i);
            printAngleAndDistance(i);
    //        checkDistance();

            wait_ms(100);
        }

        // Repeats the previous lines from 165 to 15 degrees
        for(int i=165; i>15; i--) {
            printAngleAndDistance(i);
            checkDistance();
            wait_ms(100);
        }
    
    }
}


//-------------------------|||   MAIN     |||----------------------------------

int main(void)
{
    pc.baud(9600);
    pc.printf("[INFO] Spidersense started \n");
    Ticker ticker;
    ticker.attach(periodicCallback, 1); // blink LED every second
    pc.printf("[INFO] Calling instance \n");
    //BLE &ble = BLE::Instance();
    BLEDevice ble;
    pc.printf("[INFO] Calling init complete \n");
    ble.init();

    ble.onDisconnection(disconnectionCallback);
    ble.onDataWritten(onDataWritten);
    
    UARTService uartService(ble);
    uart = &uartService;
    
    BatteryService  battery(ble);
    DeviceInformationService deviceInfo(ble, "ARM", "Model1", "SN1", "hw-rev1", "fw-rev1", "soft-rev1");
 
    /* setup advertising */
    ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME,
                                     (const uint8_t *)"BLE UART", sizeof("BLE UART") - 1);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::INCOMPLETE_LIST_128BIT_SERVICE_IDS,
                                     (const uint8_t *)UARTServiceUUID_reversed, sizeof(UARTServiceUUID_reversed));
    ble.accumulateAdvertisingPayload(GapAdvertisingData::INCOMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));                               
    ble.setAdvertisingInterval(160); /* 100ms; in multiples of 0.625ms. */
    ble.startAdvertising();
    pc.printf("Started Advertising.\n");
 
    while (true) {
        ble.waitForEvent();
    }

      
}
