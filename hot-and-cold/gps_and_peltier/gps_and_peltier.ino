#include <Arduino.h>
#include <math.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>


#define RADIUS_EARTH 6371      // Earth radius in kilometers
#define enA 9
#define in1 6
#define in2 7
#define button 4

int rotDirection = 0;
int pressed = false;

SoftwareSerial mySerial(3, 2);
Adafruit_GPS GPS(&mySerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO  false

float latPlayer;      // Variable to store player's latitude
float longPlayer;     // Variable to store player's longitude
float latPlace = 51.4575094;   // Latitude of the place in DDMM.MMMM format
float longPlace = -2.6025733; // Longitude of the place in DDMM.MMMM format

char c;

// Function to calculate distance between two points using Haversine formula
float calculateDistance(float lat1, float lon1, float lat2, float lon2) {
  float dLat = radians(lat2 - lat1);
  float dLon = radians(lon2 - lon1);
  float a = sin(dLat / 2) * sin(dLat / 2) + cos(radians(lat1)) * cos(radians(lat2)) * sin(dLon / 2) * sin(dLon / 2);
  float c = 2 * atan2(sqrt(a), sqrt(1 - a));
  float distance = RADIUS_EARTH * c;
  return distance;
}

void setup() {
  while (!Serial); // wait for Serial to be ready

  Serial.begin(115200);
  delay(5000);
  Serial.println("HeatHunt starting!");
  mySerial.begin(9600);
  // uncomment this line to receive all data
  // GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_ALLDATA);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  // GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  // GPS.sendCommand(PGCMD_ANTENNA);

  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(button, INPUT);
  // Set initial rotation direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}

void loop() {
  clearGPS();

  while (!GPS.newNMEAreceived()) {
    c = GPS.read();
  }

  GPS.parse(GPS.lastNMEA());

  Serial.print("Fix: ");
  Serial.print(GPS.fix);
  Serial.print(" quality: ");
  Serial.println(GPS.fixquality);
  Serial.print("Satellites: ");
  Serial.println(GPS.satellites);

  if (GPS.fix) {
    Serial.print("Location: ");
    Serial.print(GPS.latitude, 4);
    Serial.print(GPS.lat);
    Serial.print(", ");
    Serial.print(GPS.longitude, 4);
    Serial.println(GPS.lon);
    Serial.print("Google Maps location: ");
    Serial.print(GPS.latitudeDegrees, 4);
    Serial.print(", ");
    Serial.println(GPS.longitudeDegrees, 4);
  }
  Serial.println("-------------------------------------");

  latPlayer = GPS.latitudeDegrees;
  longPlayer = GPS.longitudeDegrees;

  // Calculate distance between player and place
  float distance = calculateDistance(latPlayer, longPlayer, latPlace, longPlace);

  // Map distance to potentiometer value
  int potValue = map(distance, 0, 1, 255, 0); // Inverse mapping: closer -> higher potValue

  // Apply potentiometer value to motor control
  analogWrite(enA, potValue);

  // Read button - Debounce
  if (digitalRead(button) == true) {
    pressed = !pressed;
  }
  while (digitalRead(button) == true);
  delay(20);

  // If button is pressed - change rotation direction
  if (pressed == true  & rotDirection == 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    rotDirection = 1;
    delay(20);
  }
  // If button is pressed - change rotation direction
  if (pressed == false & rotDirection == 1) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    rotDirection = 0;
    delay(20);
  }

  // print the results to the Serial Monitor:
  Serial.print("Distance between player and place: ");
  Serial.print(distance);
  Serial.print(" km | Mapped potValue: ");
  Serial.print(potValue);
  Serial.print(" | Button: ");
  Serial.print(digitalRead(button));
  Serial.print(" | in1: ");
  Serial.print(digitalRead(in1));
  Serial.print(" | in2: ");
  Serial.print(digitalRead(in2));
  Serial.print(" | Rotation Direction: ");
  Serial.println(rotDirection);

  delay(2000); // Delay for 2 seconds before repeating
}

void clearGPS() {
  while (!GPS.newNMEAreceived()) {
    c = GPS.read();
  }
  GPS.parse(GPS.lastNMEA());

  while (!GPS.newNMEAreceived()) {
    c = GPS.read();
  }
  GPS.parse(GPS.lastNMEA());
}