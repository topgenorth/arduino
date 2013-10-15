/**
 * Simple sketch to read the temperature, and then display a red/green/yellow light for 
 * to hot/just right/ to cold. Will also write the temperature to a CSV on the SDcard.
 */

#include "SD.h"

const unsigned int BAUD_RATE               = 9600;
const unsigned int READING_DELAY           = 30000;

// Pins in use by this sketch.
const byte HOT_PIN                         = 2;        // The LED for when it's to warm
const byte TEMP_SENSOR_PIN                 = 3;        // The temperature sensor pin is A0
const byte CS_PIN                          = 4;        // Required by the Ethernet shield
const byte NORMAL_PIN                      = 5;        // The LED for when the temperature is OK
const byte COLD_PIN                        = 6;        // The LED for it's to cool/cold
const byte SDCARD_PIN                      = 10;       // Required by the Ethernet shield


// Constants for converting the TMP36 readings into degress Celsius
const float SUPPLY_VOLTAGE                 = 5000;     // Volts, not milliVolts
const float HOT_TEMP                       = 24;
const float COOL_TEMP                      = 15;

File tempFile;
int readCount = 0;
int writeToTemperatureFile = 1;

void setup() {
  delay(250);
  Serial.begin(BAUD_RATE);
  delay(500);

  Serial.println("********** SETUP     **********");
  init_led();
  init_sdcard();
  Serial.println("********** SETUP COMPLETE *****");    
}

void init_led() {
  pinMode(HOT_PIN, OUTPUT);
  pinMode(NORMAL_PIN, OUTPUT);
  pinMode(COLD_PIN, OUTPUT);  
}

void init_sdcard() {
  Serial.print("Initializing SD card...");

  pinMode(SDCARD_PIN, OUTPUT);
  
  if (!SD.begin(CS_PIN)) {
    Serial.println("failed!");
    return;
  }

  Serial.println("succeded.");
  tempFile = SD.open("TEMP.CSV", FILE_WRITE);
  if (tempFile) {
    tempFile.close();
  }
  else {
    Serial.println("Failed to connnect to SD card.");
    writeToTemperatureFile=0;
  }
}

void loop() {
  Serial.println();
  float temperature = read_temperature_value();
  
  turn_on_led_for(temperature);

  delay(READING_DELAY);
}

float read_temperature_value() {
  int sensorValue = analogRead(TEMP_SENSOR_PIN);
  while (readCount < 3) {
    // we discard the first three reading or so
    delay(2000);
    sensorValue = analogRead(TEMP_SENSOR_PIN);
    readCount++;
  }
  float temperature = convert_sensor_reading_to_celsius(sensorValue);
  log_temperature(sensorValue, temperature);

  return temperature;
}

void turn_on_led_for(float temperature) {
  if (temperature <= COOL_TEMP) {
    digitalWrite(COLD_PIN, HIGH);
    digitalWrite(HOT_PIN, LOW);
    digitalWrite(NORMAL_PIN, LOW);
    // Serial.println("  Turn on cool LED.");
  }
  else if (temperature > COOL_TEMP && temperature < HOT_TEMP) {
    digitalWrite(COLD_PIN, LOW);
    digitalWrite(HOT_PIN, LOW);
    digitalWrite(NORMAL_PIN, HIGH);
    // Serial.println("  Turn on normal LED.");
  }
  else {
    digitalWrite(COLD_PIN, LOW);
    digitalWrite(HOT_PIN, HIGH);
    digitalWrite(NORMAL_PIN, LOW);
    // Serial.println("  Turn on hot LED.");
  }
}

float convert_sensor_reading_to_celsius(int sensor_value) {
  const int adjusted_sensor_value= sensor_value - 40; // My TMP36 is damaged and the calibration is off by ~ this amount.
  const float volts = (adjusted_sensor_value * SUPPLY_VOLTAGE) / 1024;
  const float adjustedVoltage = (volts - 500);  
  const float temperature = (adjustedVoltage / 10);
  return temperature;
}

void log_temperature(int sensorValue, float temperature) {
  Serial.print(sensorValue);
  Serial.print(",");
  Serial.println(temperature);
  write_values_to_csv(sensorValue, temperature);
}

void write_values_to_csv(int sensorValue, float temperature) {
  if (writeToTemperatureFile) {
    tempFile = SD.open("TEMP.CSV", FILE_WRITE);
    if (tempFile) {
      tempFile.print(sensorValue);
      tempFile.print(",");
      tempFile.println(temperature);
      tempFile.close();
    }
    else {
      Serial.println("Failed to open TEMP.CSV, cannot write data to SD card.");
      writeToTemperatureFile = 0;
    }
  }  
}