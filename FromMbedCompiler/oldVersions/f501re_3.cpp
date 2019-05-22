#include "mbed.h"
#include "SG90.h"
#include "HCSR04.h"
#include "beep.h"

#define LOW 0
#define HIGH 1

#define DEBUG 1
#define SERVO_DEBUG 1
#define LED_DEBUG 0
#define BUZZER_DEBUG 0
#define ULTRA_DEBUG 0

Timeout timer;

//Define Serial object
Serial pc(USBTX, USBRX);

// Defines Tirg and Echo pins of the Ultrasonic Sensor
HCSR04 sensor= HCSR04(D9, D8);


// Buzzer pin
Beep buzzer = Beep(D6);

// Define pins for LEDs
DigitalOut bluLED(D13);
DigitalOut greenLED(D12);
DigitalOut redLED(D11);

// Variables for the duration and the distance
long duration;
int ultraDistance;

// Creates a servo object for controlling the servo motor
SG90 myServo(D10);






int calculateDistance()
{

    // Function for calculating the distance measured by the Ultrasonic sensor
    return sensor.distance(CM); //TODO make it long
}


/*
void printAngleAndDistance(int i)
{
    myServo.position((float)i);
    wait_ms(30);
    ultraDistance = calculateDistance();// Calls a function for calculating the distance measured by the Ultrasonic sensor for each degree
    wait_ms(0.1);
    pc.printf("%d",i); // Sends the current degree into the Serial Port
    pc.printf(","); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
    pc.printf("%d",ultraDistance); // Sends the distance value into the Serial Port
    pc.printf("."); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
    pc.printf("\n");

}
*/

int main()
{

    /////////////loop/////////////
    while(1) {

#if DEBUG   //ONLY for testing
    #if SERVO_DEBUG    
        pc.printf("HelloEveryone \n");
       /*
        while(1){    
            myServo.position(180.0);
            wait(1);
            myServo.position(0.0);
            wait(1);
            }
        */
            
        for(int i=0; i<=180; i++) {
            pc.printf("moving: %f \n",myServo.read());
            //myServo.write( i/100.0 );
            //printAngleAndDistance(i);
            //myServo.position(i);
            myServo.SetAngle(i);
            wait_ms(100);
        }
        for(int i=180; i >= 0; i--) {
            pc.printf("moving: %f \n",myServo.read());            
            //myServo.write( i/100.0 );
            //printAngleAndDistance(i);
            //myServo.position(i);
            myServo.SetAngle(i);
            wait_ms(100);


        }
#endif        
#if LED_DEBUG
    redLED=!redLED;
    wait(0.5);
    
#endif
#if BUZZER_DEBUG
    buzzer.beep( 1000, 100);
    wait(0.5);

#endif

#if ULTRA_DEBUG
    pc.printf("CM: %d \n", calculateDistance() );
    wait(0.5);

#endif    
    
    
#else

        // rotates the servo motor from 15 to 165 degrees
        for(int i=15; i<=165; i++) {
            //pc.printf("moving: %d \n",i);
            printAngleAndDistance(i);
        
            if(ultraDistance < 20) {
                greenLED = LOW ;
                redLED = HIGH;
                buzzer.beep(1000, 100);
            } else {
                redLED = LOW ;
                greenLED = HIGH ;
            }
            wait_ms(1000);
        }

        // Repeats the previous lines from 165 to 15 degrees
        for(int i=165; i>15; i--) {
            printAngleAndDistance(i);
            if(ultraDistance < 20) {
                greenLED = LOW ;
                redLED = HIGH ;
                buzzer.beep( 1000, 100);
            } else {
                redLED = LOW ;
                greenLED = HIGH ;
            }
        }
#endif
    }

}

