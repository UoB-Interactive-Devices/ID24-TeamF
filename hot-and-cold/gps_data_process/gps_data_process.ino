#include <Arduino.h>
#include <math.h>
#include <SoftwareSerial.h>
#include <stdlib.h>

#define RADIUS_EARTH 6371 // Earth radius in kilometers
#define enA 9
#define in1 6
#define in2 7
#define button 4

int rotDirection = 0;
int pressed = false;

SoftwareSerial ss(3, 2);  // RX, TX

// Variable declaration
String sentence = "";
String original_sentence = "";
String gnrmc = "";
String gngga = "";  // Stored GNGGA information
String info[15];  // Use array to store GPS data

int commaPosition = -1;

// Test coordinates:

// royal fort gardens
// float latitude2 = 51.4575094;
// float longitude2 = -2.6025733;

// Favell House
float latitude2 = 51.4524688;
float longitude2 = -2.5928609;

float lat;
float lng;
float distance;

// Function declaration
float convertToDecimalDegrees(float coordinate);
float calculateDistance(float lat1, float lon1, float lat2, float lon2);

void setup() {
  Serial.begin(115200);
  delay(5000);
  Serial.println("HeatHunt started!");
  ss.begin(9600);
}

void loop() {
  if (ss.available()) {
    original_sentence = ss.readStringUntil('\n');
    sentence = original_sentence;
    //gngga = ss.readStringUntil('\n'); // Read the line until newline character
    //gngga = "$GNGGA,123519.00,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
    //gnrmc = "$GNRMC,123519.00,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A";
    Serial.println("Received sentence: " + sentence);

    int infoIndex = 0;
    int startPos = 0;

    while ((commaPosition = gngga.indexOf(',', startPos)) != -1 && infoIndex < 15) {
      info[infoIndex++] = gngga.substring(startPos, commaPosition);
      startPos = commaPosition + 1;
    }
    if (startPos < gngga.length()) { // Catch any remaining data after the last comma
      info[infoIndex] = gngga.substring(startPos);
    }

    if (original_sentence.startsWith("$GNGGA")) {
      lat = convertToDecimalDegrees(info[2].toFloat());
      lng = convertToDecimalDegrees(info[4].toFloat());
    }
    else if (original_sentence.startsWith("$GNRMC")) {
      lat = convertToDecimalDegrees(info[3].toFloat());
      lng = convertToDecimalDegrees(info[5].toFloat());
    }
    else {
      // Ignore sentences that are not GNGGA or GNRMC
      Serial.println("It's not gngga or gnrmc");
    }
    // Calculate and print distance
    distance = calculateDistance(lat, lng, latitude2, longitude2);
    Serial.print("Latitude: ");
    Serial.println(lat, 6);
    Serial.print("Longitude: ");
    Serial.println(lng, 6);
    Serial.print("Distance between the points: ");
    Serial.print(distance);
    Serial.println(" km");
  }
  // Map distance to potentiometer value
  int potValue = map(distance, 0, 1000, 255, 0); // Inverse mapping: closer -> higher potValue

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
  Serial.print(" m | Mapped potValue: ");
  Serial.print(potValue);
  Serial.print(" | Button: ");
  Serial.print(digitalRead(button));
  Serial.print(" | in1: ");
  Serial.print(digitalRead(in1));
  Serial.print(" | in2: ");
  Serial.print(digitalRead(in2));
  Serial.print(" | Rotation Direction: ");
  Serial.println(rotDirection);

  delay(2000); // Delay for 5 seconds before repeating
}

float convertToDecimalDegrees(float coordinate) {
  int degrees = int(coordinate / 100);
  float minutes = coordinate - degrees * 100;
  float decimalDegrees = degrees + (minutes / 60.0);
  return (coordinate >= 0) ? decimalDegrees : -decimalDegrees;
}

float calculateDistance(float lat1, float lon1, float lat2, float lon2) {
  float dLat = radians(lat2 - lat1);
  float dLon = radians(lon2 - lon1);
  float a = sin(dLat / 2) * sin(dLat / 2) + cos(radians(lat1)) * cos(radians(lat2)) * sin(dLon / 2) * sin(dLon / 2);
  float c = 2 * atan2(sqrt(a), sqrt(1 - a));
  float distance = RADIUS_EARTH * c;
  return distance;
}

