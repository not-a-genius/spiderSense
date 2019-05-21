/*
	Code imported from arduino forum
*/


// defines pins numbers
const int trigPin = 9;
const int echoPin = 10;

// defines variables
long duration;
int distance1=0;
int distance2=0;
double speed=0;
int distance=0;

void setup() 
{
pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input
pinMode( 7 , OUTPUT);
Serial.begin(9600); // Starts the serial communication
}


void loop() 
{

//calculating speed
   distance1 = ultrasonicRead(); //calls ultrasoninicRead() function below
   
   delay(1000);//giving a time gap of 1 sec
   
   distance2 = ultrasonicRead(); //calls ultrasoninicRead() function below
   
   //formula change in distance divided by change in time
   speed = (distance2 - distance1)/1.0; //as the time gap is 1 sec we divide it by 1.
   
//Displaying speed
  Serial.print("Speed in cm/s  :  "); 
  Serial.println(speed);

   
// LED indicator
if (distance >0 && distance <5) 
{
    digitalWrite( 7 , HIGH);
    delay(50);                  // waits for a second
}

if (distance > 5 && distance <10 ) 
{
    digitalWrite( 7 , HIGH);
    delay(50);                  // waits for a second
    digitalWrite( 7 , LOW);    // sets the LED off
    delay(50);                  // waits for a second
}

if (distance >10  && distance < 20) 
{
    digitalWrite( 7 , HIGH);
    delay(210);                  // waits for a second
    digitalWrite( 7 , LOW);    // sets the LED off
    delay(210);                  // waits for a second
}

if (distance >20  && distance < 35) 
{
    digitalWrite( 7 , HIGH);
    delay(610);                  // waits for a second
    digitalWrite( 7 , LOW);    // sets the LED off
    delay(610);                  // waits for a second
}


}

float ultrasonicRead ()
{
// Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

//calculating distance
distance= duration*0.034/2;

// Prints the distance on the Serial Monitor
Serial.print("Distance in cm : ");
Serial.println(distance);

return distance;

}