#include <Arduino.h>
#include <math.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

#define MAX_SENTENCE_LENGTH 80 // Maximum length of an NMEA sentence
#define RADIUS_EARTH 6371      // Earth radius in kilometers
#define enA 9
#define in1 6
#define in2 7
#define button 4

int rotDirection = 0;
int pressed = false;

SoftwareSerial mySerial(3, 2);
Adafruit_GPS GPS(&mySerial);

char currentSentence[MAX_SENTENCE_LENGTH];
bool parsingSentence = false;

float latPlayer;      // Variable to store player's latitude
float longPlayer;     // Variable to store player's longitude
float latPlace = 51.4575094;   // Latitude of the place in DDMM.MMMM format
float longPlace = -2.6025733; // Longitude of the place in DDMM.MMMM format
float angle;

// Function to convert coordinate to decimal degrees
float convertToDecimalDegrees(float coordinate) {
  // Extract degrees and minutes
  int degrees = int(coordinate / 100);    // Get the integer part as degrees
  float minutes = coordinate - degrees * 100; // Get the fractional part as minutes

  // Convert minutes to degrees
  float decimalDegrees = degrees + (minutes / 60.0); // Combine degrees and minutes

  return decimalDegrees;
}

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
  mySerial.begin(9600);
  delay(2000);

  Serial.println("Software Serial GPS Test Echo Test");

  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(button, INPUT);
  // Set initial rotation direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}

void loop() {
  if (mySerial.available()) {
    char c = mySerial.read();
    if (c == '$') { // Start of an NMEA sentence
      memset(currentSentence, 0, sizeof(currentSentence)); // Clear the current sentence buffer
      parsingSentence = true; // Start parsing the sentence
    } else if (c == '\r' || c == '\n') { // End of an NMEA sentence
      parsingSentence = false; // Stop parsing the sentence
      if (strstr(currentSentence, "$GNRMC") != NULL) { // Check if it's an RMC sentence
        // Tokenize the sentence based on commas
        char* token = strtok(currentSentence, ",");
        int tokenIndex = 0;
        while (token != NULL) {
          if (tokenIndex == 2) { // Latitude
            latPlayer = convertToDecimalDegrees(atof(token));
          } else if (tokenIndex == 3) { // Latitude direction (N/S)
            if (*token == 'S') {
              latPlayer *= -1.0;
            }
          } else if (tokenIndex == 4) { // Longitude
            longPlayer = convertToDecimalDegrees(atof(token));
          } else if (tokenIndex == 5) { // Longitude direction (E/W)
            if (*token == 'W') {
              longPlayer *= -1.0;
            }
          } else if (tokenIndex == 8) { // Angle
            angle = atof(token);
          }
          token = strtok(NULL, ",");
          tokenIndex++;
        }

        // Print the extracted values to the serial monitor
        Serial.print("Latitude: ");
        Serial.println(latPlayer, 6);
        Serial.print("Longitude: ");
        Serial.println(longPlayer, 6);
        Serial.print("Angle: ");
        Serial.println(angle, 2);
      }
    }
    if (parsingSentence) {
      strncat(currentSentence, &c, 1); // Add character to the current sentence buffer
      // Ensure the buffer does not exceed its maximum length
      if (strlen(currentSentence) >= MAX_SENTENCE_LENGTH - 1) {
        parsingSentence = false; // Stop parsing if the buffer is full
      }
    }
  }

  // Calculate distance between player and place
  float distance = calculateDistance(latPlayer, longPlayer, latPlace, longPlace);

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

  delay(5000); // Delay for 5 seconds before repeating
}
