#include "Adafruit_MAX31855.h"

// Define the Data Out, Chip Select, and Clock pins from the MAX31855 to the Arduino
#define MAXDO   3
#define MAXCS   4
#define MAXCLK  5

// Define the fan and LED pins
#define FAN 7
#define LED 9

int i;
const int n_cycle = 25; // Total number of cycles


// Set variables for calculating the heating and cooling rates
double h_y1;
double h_y2; 
double h_x1; 
double h_x2;
double c_y2 = 0;
double c_x2 = 0;
double c_y1;
double c_x1;
double heating_list[n_cycle];
double cooling_list[n_cycle];

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

void heat(double heatTemperature) {
  readTemp = thermocouple.readCelsius();
  while (readTemp < heatTemperature) {
    readTemp = thermocouple.readCelsius();
    analogWrite(LED, 255);
    digitalWrite(FAN, LOW);
    printData(readTemp, i);
  }  
}

void cool(double coolTemperature) {
  readTemp = thermocouple.readCelsius();
  while (readTemp > coolTemperature) {
    readTemp = thermocouple.readCelsius();
    analogWrite(LED, 0);
    digitalWrite(FAN, HIGH);  
    printData(readTemp, i);
  }  
}

void hold(unsigned long timeOfInterval, double holdTemperature) {
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


void loop() {
  heat(98); // Initial Denaturation
  hold(60, 98); // Initial Denaturation
  hold(5, 98); // First "cycle"
  cool(72); // First "cycle"
  hold(20, 72);

  // Start cycling
  for (i = 0; i < n_cycle; i++) {  
    h_y1 = thermocouple.readCelsius();
    h_x1 = millis() / 1000;
    heat(98);
    h_y2 = thermocouple.readCelsius();
    h_x2 = millis() / 1000;
    heating_list[i] = (h_y2 - h_y1) / (h_x2 - h_x1);
    hold(5, 98); 
    c_y1 = thermocouple.readCelsius();
    c_x1 = millis() / 1000;
    cool(72);
    c_y2 = thermocouple.readCelsius();
    c_x2 = millis() / 1000;
    hold(20, 72);
    cooling_list[i] = (c_y2 - c_y1) / (c_x2 - c_x1);
  }
  hold(600, 72); // Final Extension
  
  Serial.print("DONE");
  Serial.println("");
  Serial.println("Setting LED power to 0...");

  //Print the heating and cooling rates
  for (int i = 0; i < n_cycle; i++) {
    Serial.print("Cycle ");
    Serial.print(i+1);
    Serial.print(": ");
    Serial.print(heating_list[i]);
    Serial.print(",");
    Serial.println(cooling_list[i]);
  }

  double avg_heating = 0;
  double avg_cooling = 0;
  for (int i = 0; i < n_cycle; i++) {
    avg_heating += heating_list[i];
    avg_cooling += cooling_list[i];
  }
  avg_heating /= n_cycle;
  avg_cooling /= n_cycle;

  Serial.print("Average heating rate: ");
  Serial.println(avg_heating);
  Serial.print("Average cooling rate: ");
  Serial.println(avg_cooling);
  
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
 // h_y2 = thermocouple.readCelsius();
  //h_x2 = millis() / 1000;
 // heating_list[i] = (h_y2 - h_y1) / (h_x2 - h_x1);

  // Otherwise, if readTemp is greater than 
  // the holdTemperature, cool down the sample
  // by turning the LED off and the fan on.
  while (readTemp > holdTemperature) {
    analogWrite(LED, 0);
    digitalWrite(FAN, HIGH);
    readTemp = thermocouple.readCelsius();
    printData(readTemp, i);
  }
  c_y2 = thermocouple.readCelsius();
  c_x2 = millis() / 1000;
  cooling_list[i] = (c_y2 - h_y2) / (c_x2 - h_x2);

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
