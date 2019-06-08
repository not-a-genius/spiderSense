//Library for controlling ultrasonic module HCSR04
//Ported by hiawoood from arduino library orgininally created by ITead studio.
//Instantiate object by supplying the proper pin numbers of "trigger" and "echo"
//e.g. 
/*
        int main() {
            Ultrasonic sensor(p5, p6);
            while(1){
                long distance = sensor.distance(CM);
                printf("Distance:%d\n");
                wait(0.1);
            }
        }
*/



#ifndef HCSR04_H
#define HCSR04_H

#include "mbed.h"

#define CM 1
#define INC 0

class HCSR04 {
  public:
    HCSR04(PinName t, PinName e);
    long echo_duration();
    long distance(int sys);

    private:
        DigitalOut trig;
        DigitalIn echo;
        Timer timer;
        long duration,distacne_cm,distance_inc;
};

#endif