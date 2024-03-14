#include "Adafruit_MAX31855.h"

// Define the Data Out, Chip Select, and Clock pins from the MAX31855 to the Arduino
#define MAXDO   3
#define MAXCS   4
#define MAXCLK  5

// Define the fan and LED pins
#define FAN 7
#define LED 9

int i;

// Global variables
double readTemp; // The reading of the thermocouple
double t_delay = 1; // delay time for recording data as well as flicking the LED on/off for holding temperature (units: ms)

// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

void setup() {
  Serial.begin(9600);
  // Set the fan and LED as output pins
  pinMode(LED, OUTPUT);
  pinMode(FAN, OUTPUT);
}

void loop() {
  interval(60, 98);
  for (i = 0; i < 25; i++) { 
    interval(5, 98);  
    interval(20, 72); 
  }
  interval(600, 72); 
  Serial.print("DONE");
  Serial.println("");
  Serial.println("Setting LED power to 0...");
  while (true) {
    analogWrite(LED, 0);
    digitalWrite(FAN, LOW);
  }
  
}

void interval(unsigned long timeOfInterval, double holdTemperature) {
  // The interval function sets the length and time
  // of the specified heating block. Use as many
  // interval functions as you need.
  // Inputs:
  // timeOfInterval: time in seconds
  // holdTemperature: temperature in degrees C

  // Get the initial temperature of the thermocouple
  // at the beginning of the interval.
  readTemp = thermocouple.readCelsius();

  // If readTemp is less than holdTemperature,
  // heat up the sample by turning the LED on 
  // and the fan off.
  while (readTemp < holdTemperature) {
    analogWrite(LED, 255);
    digitalWrite(FAN, LOW);
    readTemp = thermocouple.readCelsius();
    printData(readTemp, i);  
  }

  // Otherwise, if readTemp is greater than 
  // the holdTemperature, cool down the sample
  // by turning the LED off and the fan on.
  while (readTemp > holdTemperature) {
    analogWrite(LED, 0);
    digitalWrite(FAN, HIGH);
    readTemp = thermocouple.readCelsius();
    printData(readTemp, i);
  }

  // Once we reach the holdTemperature, 
  // turn the LED on and off rapidly to 
  // maintain the holdTemperature
  double startingTime = millis() / 1000.00;
  double elapsedTime = 0;
  digitalWrite(FAN, LOW);
  while (elapsedTime <= timeOfInterval) {
    readTemp = thermocouple.readCelsius();
    elapsedTime = ( millis() / 1000.00 ) - startingTime;

    if (readTemp < holdTemperature) {analogWrite(LED, 255); }
    else if (readTemp > holdTemperature) {analogWrite(LED, 0);}
    printData(readTemp, i);
  }

}

// Function to format what is printed to the serial output
void printData(double temp, int n) {
  Serial.print( millis() / 1000.00 );
  Serial.print(" ");
  Serial.print(temp);
  Serial.print(" ");
  Serial.println(n); // cycle number
  delay(t_delay); 
}
