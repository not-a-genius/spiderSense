#include "mbed.h"
#include "Servo.h"
#include "HCSR04.h"
#include "beep.h"

#define LOW 0
#define HIGH 1

#define DEBUG 0

Timeout timer;
DigitalOut my_led(LED1);



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
Servo myServo(D10); 




                
                
int calculateDistance(){  
  
    // Function for calculating the distance measured by the Ultrasonic sensor
  return sensor.distance(CM); //TODO make it long  
}



    void printAngleAndDistance(int i) {
        myServo.position(i);
        wait(30);
        ultraDistance = calculateDistance();// Calls a function for calculating the distance measured by the Ultrasonic sensor for each degree
        wait(0.1);
        printf("%d",i); // Sends the current degree into the Serial Port
        printf(","); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
        printf("%d",ultraDistance); // Sends the distance value into the Serial Port
        printf("."); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing

    }


int main() {
    
    printf("HelloEveryone");
    /////////////loop/////////////
    while(1) {
        
#if DEBUG
        //testing loop
        while(1){
            for(int i=0;i<0.46;i+=0.2){
                myServo.write(i);
                wait(500);
                }
            for(int i=0.46;i>0;i-=0.2){
                myServo.write(i);
                wait(500);
            }
        
            }
#endif
         
        // rotates the servo motor from 15 to 165 degrees
        for(int i=15; i<=90; i++) {
            printAngleAndDistance(i);
                        
            if(ultraDistance < 20) {
                greenLED = LOW ;
                redLED = HIGH;
                buzzer.beep(1000, 100);
            } else {
                redLED = LOW ;
                greenLED = HIGH ;
            }
            wait(1000);
        }

        // Repeats the previous lines from 165 to 15 degrees
        for(int i=90; i>15; i--) {
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
    }
    
    
}
 
