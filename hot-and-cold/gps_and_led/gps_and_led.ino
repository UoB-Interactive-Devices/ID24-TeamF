#include <Arduino.h>
#include <math.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>


#define RADIUS_EARTH 6371      // Earth radius in kilometers
#define enA 9
#define ledPin 11
#define in1 6
#define in2 7
#define RX 3
#define TX 2
// use define to choose target place
// #define RoyalForkGarden
#define CantocksCloseBusStop

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO  false

SoftwareSerial mySerial(RX, TX);
Adafruit_GPS GPS(&mySerial);

float latPlayer;      // Variable to store player's latitude
float longPlayer;     // Variable to store player's longitude

#ifdef RoyalForkGarden
  float latPlace = 51.45783686779149;
  float longPlace = -2.602806472990425;
#endif

#ifdef CantocksCloseBusStop
float latPlace = 51.4558301767948;
float longPlace = -2.6023120229001377;
#endif

char c;

// Function to calculate distance between two points using Haversine formula
float calculateDistance(float lat1, float lon1, float lat2, float lon2) {
  float dLat = radians(lat2 - lat1);
  float dLon = radians(lon2 - lon1);
  float a = sin(dLat / 2) * sin(dLat / 2) + cos(radians(lat1)) * cos(radians(lat2)) * sin(dLon / 2) * sin(dLon / 2);
  float c = 2 * atan2(sqrt(a), sqrt(1 - a));
  float distance = RADIUS_EARTH * c * 1000;
  return distance;
}

void setup() {
  while (!Serial); // wait for Serial to be ready

  Serial.begin(115200);
  delay(3000);
  Serial.println("\n **********HeatHunt starting!**********");
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
  pinMode(ledPin, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  // Set initial rotation direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(ledPin, LOW);
}

void loop() {
  clearGPS();

  while (!GPS.newNMEAreceived()) {
    c = GPS.read();
  }

  GPS.parse(GPS.lastNMEA());

  Serial.print("Fix: ");
  Serial.println(GPS.fix);
  // Serial.print(" quality: ");
  // Serial.println(GPS.fixquality);
  // Serial.print("Satellites: ");
  // Serial.println(GPS.satellites);

  Serial.print("Location: ");
  Serial.print(GPS.latitudeDegrees, 4);
  Serial.print(", ");
  Serial.println(GPS.longitudeDegrees, 4);

  latPlayer = GPS.latitudeDegrees;
  longPlayer = GPS.longitudeDegrees;
  // Calculate distance between player and place
  float distance = calculateDistance(latPlayer, longPlayer, latPlace, longPlace);
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" m");

  // Map distance to potentiometer value
  int potValue = map(distance, 20, 200, 0, 2000); // Inverse mapping: closer -> higher potValue
  
  Serial.print("Mapped potValue: ");
  Serial.println(potValue);
  Serial.println("-------------------------------------");
  // analogWrite(ledPin, potValue);

  if (distance < 20) {
    digitalWrite(ledPin, HIGH);
    // No delay
  } else {
    // Otherwise, adjust the delay based on distance
    // Turn LED on
    digitalWrite(ledPin, HIGH);
    // Wait for the calculated delay
    delay(potValue);
    // Turn LED off
    digitalWrite(ledPin, LOW);
    // Wait for the same delay
    delay(potValue);
  }
  // delay(2000); // Delay for 2 seconds before repeating
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