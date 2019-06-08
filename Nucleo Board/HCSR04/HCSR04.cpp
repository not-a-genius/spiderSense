#include "HCSR04.h"
#include "mbed.h"


HCSR04::HCSR04(PinName t, PinName e) : trig(t), echo(e) {}

//      Trigger          Echo
//      _______           _____________,,,,,,,,,
// ____|  10us |_________| 150us-25ms, or 38ms if no obstacle
// 

//return echo duration in us (refer to digram above)
long HCSR04::echo_duration() {
    timer.reset();
    trig = 0;
    wait_us(2);
    trig = 1;
    wait_us(10);
    trig = 0;
    while(echo == 0);
    timer.start();
    while(echo == 1);
    timer.stop();
    return timer.read_us();
}

//return distance to nearest obstacle or returns -1 
//if no obstacle within range
//set sys to cm or inch accordingly
long HCSR04::distance(int sys){
    duration = echo_duration();
    if(duration > 30000)
        return -1;
    distacne_cm = duration /29 / 2 ;
    distance_inc = duration / 74 / 2;
    if (sys)
        return distacne_cm;
    else
        return distance_inc;
}

