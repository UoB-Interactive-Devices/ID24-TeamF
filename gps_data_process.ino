#include <Arduino.h>
#include <math.h>
#include <SoftwareSerial.h>
#include <stdlib.h>

#define RADIUS_EARTH 6371 // Earth radius in kilometers

/*
Not necessary now I guess:
Exception handling
Processing of negative values in latitude and longitude conversion
*/
SoftwareSerial ss(3, 2);  // RX, TX

// Variable declaration
String sentance = "";
String gnrmc = "";
String gngga = "";  // Stored GNGGA information
String info[15];  // Use array to store GPS data

int commaPosition = -1;

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
    //gngga = ss.readStringUntil('\n'); // Read the line until newline character
    //gngga = "$GNGGA,123519.00,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
    //gnrmc = "$GNRMC,123519.00,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A";
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

        Serial.print("Lati1: ");
        Serial.println(lat, 6); 
        Serial.print("Long1: ");
        Serial.println(lng, 6);

        // Test coordinates, convert to decimal degrees
        float latitude2 = convertToDecimalDegrees(3403.1234);
        float longitude2 = convertToDecimalDegrees(-11814.5678);

        Serial.print("Lati2: ");
        Serial.println(latitude2, 6);
        Serial.print("Long2: ");
        Serial.println(longitude2, 6);

        // Calculate and print distance
        float distance = calculateDistance(lat, lng, latitude2, longitude2);
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

      Serial.println("Lati: " + info[3]);
      Serial.println("Long: " + info[5]);

      if (infoIndex >= 6) { // Make sure we have at least 7 fields (0-indexed 6)
        float lat = convertToDecimalDegrees(info[2].toFloat());
        float lng = convertToDecimalDegrees(info[4].toFloat());

        Serial.print("Lati1: ");
        Serial.println(lat, 6); 
        Serial.print("Long1: ");
        Serial.println(lng, 6);

        // Test coordinates, convert to decimal degrees
        float latitude2 = convertToDecimalDegrees(3403.1234);
        float longitude2 = convertToDecimalDegrees(-11814.5678);

        Serial.print("Lati2: ");
        Serial.println(latitude2, 6);
        Serial.print("Long2: ");
        Serial.println(longitude2, 6);

        // Calculate and print distance
        float distance = calculateDistance(lat, lng, latitude2, longitude2);
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

