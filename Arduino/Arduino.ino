// Includes the Servo library
#include <Servo.h>.

// Defines Tirg and Echo pins of the Ultrasonic Sensor
const int trigPin = 9;
const int echoPin = 8;

// Servo pin
const int servoPin = 10;

// Buzzer pin
const int buzzerPin = 6;

// Define pins for LEDs
static int bluLED = 13;
static int greenLED = 12;
static int redLED = 11;

// Variables for the duration and the distance
long duration;
int distance;

// Creates a servo object for controlling the servo motor
Servo myServo;

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(buzzerPin, OUTPUT); // Sets the buzzerPin as an Output
  Serial.begin(9600);

  // Setup LEDs as Outputs
  pinMode(bluLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  // Setup LEDs
  digitalWrite(bluLED, LOW);
  digitalWrite(greenLED, HIGH);
  digitalWrite(redLED, LOW);
  
  myServo.attach(servoPin); // Defines on which pin is the servo motor attached
}

void loop() {
  
  // rotates the servo motor from 15 to 165 degrees
  for(int i=0;i<=180;i++) {  
    myServo.write(i);
    delay(100);
  }
  
  // Repeats the previous lines from 165 to 15 degrees
  for(int i=180;i>0;i--){  
    myServo.write(i);
    delay(100);
  }
}

// Function for calculating the distance measured by the Ultrasonic sensor
int calculateDistance(){  
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); // Reads the echoPin, returns the sound wave travel time in microseconds
  distance = duration*0.0172;
  return distance;
}
