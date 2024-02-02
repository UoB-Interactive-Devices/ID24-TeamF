#include <Servo.h>

#define FullHealth 1000
#define Fullawaketime 1000
#define starvethreshold 0.3
#define sleepythreshold 0.3

Servo myservo;  // create servo object to control a servo

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 2;     // the number of the pushbutton pin

const int ledPin2 = 11;      // live green
const int ledPin3 = 12;      // sleep red light
const int ledPin1 = 13;      // food red button

const int potpin1 = A0;       // analog pin used to connect the servo
const int potpin2 = A1;       // analog pin used to connect the sleep

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
int val1;    // variable to read the value from the analog pin
int val2;
int health = FullHealth;            // if health is 0, the tamagotchi dies
int timetosleep = Fullawaketime;

byte lastButtonState = LOW;

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  // initialize the LED pin as an output:
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);


}

void loop() {
  byte buttonState = digitalRead(buttonPin);
  // if fed, the health goes to FullHealth
  if (buttonState != lastButtonState) {
    lastButtonState = buttonState;
    if (buttonState == LOW && health > 0) {
      health = FullHealth;
    }
  }
  
  if (health > starvethreshold * FullHealth && health <= FullHealth) {
    digitalWrite(ledPin1, HIGH);
  }
  else if (health > 0 && health <= starvethreshold * FullHealth ) {
    digitalWrite(ledPin1, HIGH);
    delay(10);
    digitalWrite(ledPin1, LOW);
    delay(10);
  }
  else digitalWrite(ledPin1, LOW);
  
  val1 = analogRead(potpin1);            // reads the value of the potentiometer (value between 0 and 1023)
  Serial.print("sensor1 = ");
  Serial.print(val1);
  
  val1 = map(val1, 400, 1000, 0, 180);     // scale it for use with the servo (value between 0 and 180)
  Serial.print("\t output1 = ");
  Serial.println(val1);
  
  
  val2 = analogRead(potpin2);            // reads the value of the potentiometer (value between 0 and 1023)
  Serial.print("sensor2 = ");
  Serial.print(val2);
  
  val2 = map(val2, 300, 800, -40, 250);
  Serial.print("\t output2 = ");
  Serial.println(val2);

  if (health > 0 && timetosleep > 0) {
    analogWrite(ledPin3, val2);
    myservo.write(val1);                  // sets the servo position according to the scaled value
    if (val2 < 50){
      timetosleep = timetosleep - 5;
    }
    else {
      timetosleep = timetosleep + 3;
    }
  }
  else digitalWrite(ledPin2, LOW);

  if (timetosleep < 500 && timetosleep > 0) {
    digitalWrite(ledPin3, HIGH);
    delay(10);
    digitalWrite(ledPin3, LOW);
    delay(10);
  }
  else {
    digitalWrite(ledPin3, HIGH);
  }
  
  if (health > 0  && timetosleep > 0 )
  {
    digitalWrite(ledPin2, HIGH);
  }
  else 
  {
    digitalWrite(ledPin2, LOW);
  }

  health = health - 1;
  timetosleep = timetosleep - 1;
  delay(15);                           // waits for the servo to get there
}

