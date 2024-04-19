#include <Arduino.h>
#include <math.h>
#include <SoftwareSerial.h>
#include <stdlib.h>

#define RADIUS_EARTH 6371 // Earth radius in kilometers

// this is the define for peltier
#define MAX_SENTENCE_LENGTH 80 // Maximum length of an NMEA sentence
#define enA 9
#define in1 6
#define in2 7
#define button 4

int rotDirection = 0;
int pressed = false;

SoftwareSerial ss(3, 2); // RX, TX

String sentance = "";
String gnrmc = "";
String gngga = "";     // Stored GNGGA information
String info[15];       // Use array to store GPS data
int distance;          // Add a semicolon to properly declare the variable

int commaPosition = -1; // Move commaPosition declaration to global scope

// Function declaration
String getFieldValue(int index);
float convertToDecimalDegrees(float coordinate);
float calculateDistance(float lat1, float lon1, float lat2, float lon2);

void setup() {
  Serial.begin(115200);
  ss.begin(9600);
}

void loop() {
  if (ss.available()) {
    sentance = ss.readStringUntil('\n');
    Serial.println("Received sentance: " + sentance);

    if (sentance.startsWith("$GNGGA")) {
      gngga = sentance;
      Serial.println("gngga: " + gngga);
      int infoIndex = 0;
      int startPos = 0;

      while ((commaPosition = gngga.indexOf(',', startPos)) != -1 && infoIndex < 15) {
        info[infoIndex++] = gngga.substring(startPos, commaPosition);
        startPos = commaPosition + 1;
      }
      if (startPos < gngga.length()) { // Catch any remaining data after the last comma
        info[infoIndex] = gngga.substring(startPos);
      }

      Serial.println("Lati: " + info[2]);
      Serial.println("Long: " + info[4]);

      if (infoIndex >= 6) { // Make sure we have at least 7 fields (0-indexed 6)
        float lat = convertToDecimalDegrees(info[2].toFloat());
        float lng = convertToDecimalDegrees(info[4].toFloat());

        // Test coordinates, convert to decimal degrees
        float latitude2 = convertToDecimalDegrees(3403.1234);
        float longitude2 = convertToDecimalDegrees(-11814.5678);

        // Calculate and print distance
        distance = calculateDistance(lat, lng, latitude2, longitude2);
        Serial.print("Latitude: ");
        Serial.println(lat, 6);
        Serial.print("Longitude: ");
        Serial.println(lng, 6);
        Serial.print("Distance between the points: ");
        Serial.print(distance);
        Serial.println(" m");
      } else {
        Serial.println("Not enough data or data is invalid.");
      }
    } else if (sentance.startsWith("$GNRMC")) {
      gnrmc = sentance;
      Serial.println("gnrmc: " + gnrmc);
      int infoIndex = 0;
      int startPos = 0;

      while ((commaPosition = gnrmc.indexOf(',', startPos)) != -1 && infoIndex < 15) {
        info[infoIndex++] = gnrmc.substring(startPos, commaPosition);
        startPos = commaPosition + 1;
      }
      if (startPos < gnrmc.length()) { // Catch any remaining data after the last comma
        info[infoIndex] = gnrmc.substring(startPos);
      }

      Serial.println("Lati: " + info[2]);
      Serial.println("Long: " + info[4]);

      if (infoIndex >= 6) { // Make sure we have at least 7 fields (0-indexed 6)
        float lat = convertToDecimalDegrees(info[2].toFloat());
        float lng = convertToDecimalDegrees(info[4].toFloat());

        // Test coordinates, convert to decimal degrees
        float latitude2 = convertToDecimalDegrees(3403.1234);
        float longitude2 = convertToDecimalDegrees(-11814.5678);

        // Calculate and print distance
        distance = calculateDistance(lat, lng, latitude2, longitude2);
        Serial.print("Latitude: ");
        Serial.println(lat, 6);
        Serial.print("Longitude: ");
        Serial.println(lng, 6);
        Serial.print("Distance between the points: ");
        Serial.print(distance);
        Serial.println(" m");
      } else {
        Serial.println("Not enough data or data is invalid.");
      }
    } else {
      // Ignore sentences that are not GNGGA or GNRMC
      Serial.println("It's not gngga or gnrmc");
      return;
    }

    // The peltier code and the distance

    // Map distance to potentiometer value
    int potValue = map(distance, 0, 100, 255, 0); // Inverse mapping: closer -> higher potValue

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
  }

  delay(5000); // Delay for 5 seconds before next read
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
