#include "mbed.h"
#include "Servo.h"
#include "HCSR04.h"
#include "beep.h"

#define LOW 0
#define HIGH 1

//// flags
#define DEBUG 0
#define SERVO_DEBUG 0
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
DigitalOut blueLED(D13);
DigitalOut greenLED(D12);
DigitalOut redLED(D11);

// Variables for the duration and the distance
long duration;
int ultraDistance=-1;

// Creates a servo object for controlling the servo motor
Servo myServo(D3);

int calculateDistance()
{

    // Function for calculating the distance measured by the Ultrasonic sensor
    return sensor.distance(CM); //TODO make it long
}


void buzzerThead(){
    //buzzer.beep(1000, 0.5);
    
    }


void printAngleAndDistance(int angle)
{
    myServo.SetPosition(500 + (angle * 12.1212));
    wait_ms(30);
    ultraDistance = calculateDistance();// Calls a function for calculating the distance measured by the Ultrasonic sensor for each degree
    wait_ms(0.1);
    pc.printf("%d",angle); // Sends the current degree into the Serial Port
    pc.printf(","); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
    pc.printf("%d",ultraDistance); // Sends the distance value into the Serial Port
    pc.printf("."); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
   // pc.printf("\n");

}

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

int main()
{
    myServo.Enable(1500,20000);

    /////////////loop/////////////
    while(1) {
       
        // rotates the servo motor from 15 to 165 degrees
        for(int i=15; i<=165; i++) {
            printAngleAndDistance(i);
            checkDistance();
        
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

