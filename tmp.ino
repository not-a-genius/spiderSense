

#define INTRA_DELAY 20 //delay between sequential echoing
#define LOG_DELAY 2000
#define trigPin1 2
#define echoPin1 3

#define trigPin2 4
#define echoPin2 5

#define trigPin3 6
#define echoPin3 

int trigPins[3]={trigPin1,trigPin2,trigPin3};
int echoPins[3]={echoPin1,echoPin2,echoPin3};

//TODO define functions to get distance from HC-SR04
//TODO optimize code (DRY)

void setup() {
  Serial.begin (9600);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
   pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
   pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);

}

void loop() {
  long duration;
  double distance;
  int i=0;
  while(i<3){
    
    digitalWrite(trigPins[i], LOW);  // Added this line
    delayMicroseconds(2); // Added this line
    digitalWrite(trigPins[i], HIGH);
    delayMicroseconds(10); // Added this line
    digitalWrite(trigPins[i], LOW);
    duration = pulseIn(echoPins[i], HIGH);
    distance = (duration/2) / 29.1;

     if (distance >= 500 || distance <= 0){
      Serial.println("Out of range");
    }
    else {
      Serial.print ( "Sensor");
      Serial.print (i+1); 
      Serial.print (" LEFT ");
      Serial.print ( distance);
      Serial.println("cm");
    }
    delay(INTRA_DELAY);

    i++;
  }
  Serial.println(""); 
  delay(LOG_DELAY);
}