/**
 * Simple sketch to read the temperature, and then display a red/green/yellow light for 
 * to hot/just right/ to cold. Will also write the temperature to a CSV on the SDcard.
 * 
 */

 #include "SD.h"

// Set this to 0 for the Uno R3, 1 for the Mega ADK board.
 #define MEGA_ADK 1

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
#if MEGA_ADK 
const int TMP36_ADJUSTMENT                 = -60;      // My TMP36 seems to be damaged/mis-calibrated. This should compensate? 
#else
const int TMP36_ADJUSTMENT                 = -167;
#endif

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
  delay(2000);
}

void init_led() {
  pinMode(HOT_PIN, OUTPUT);
  pinMode(NORMAL_PIN, OUTPUT);
  pinMode(COLD_PIN, OUTPUT);  
}

void init_sdcard() {
  Serial.print("Initializing SD card...");

#if MEGA_ADK
  // Disable w5100 while setting up SD
  pinMode(SDCARD_PIN, OUTPUT);
  digitalWrite(SDCARD_PIN, HIGH);
#endif

  // Setup SD card...
  if (!SD.begin(CS_PIN)) {
    Serial.println("failed!");
    return;
  }

  Serial.println("succeded.");
#if MEGA_ADK
  tempFile = SD.open("TEMPMEGA.CSV", FILE_WRITE);
#else
  tempFile = SD.open("TEMP.CSV", FILE_WRITE);
#endif
  if (tempFile) {
    tempFile.close();
  }
  else {
    Serial.println("Failed to connnect to SD card.");
    writeToTemperatureFile=0;
  }

  // Setup up w5100
  // Ethernet.begin(mac,ip);
  // Disable w5100 SPI
  digitalWrite(SDCARD_PIN, HIGH);

  // It takes a bit of time for the w5100 to get going
  delay(2000);
}

void loop() {
  float temperature = read_temperature_value();
  
  turn_on_led_for(temperature);

  delay(READING_DELAY);
}

const float read_temperature_value() {
  int sensorValue = analogRead(TEMP_SENSOR_PIN);
  while (readCount < 3) {
    // we discard the first three reading or so
    delay(2000);
    sensorValue = analogRead(TEMP_SENSOR_PIN);
    readCount++;
  }
  const float temperature = convert_sensor_reading_to_celsius(sensorValue);
  log_temperature(sensorValue, temperature);

  return temperature;
}

void turn_on_led_for(float temperature) {
  if (temperature <= COOL_TEMP) {
    digitalWrite(COLD_PIN, HIGH);
    digitalWrite(HOT_PIN, LOW);
    digitalWrite(NORMAL_PIN, LOW);
  }
  else if (temperature > COOL_TEMP && temperature < HOT_TEMP) {
    digitalWrite(COLD_PIN, LOW);
    digitalWrite(HOT_PIN, LOW);
    digitalWrite(NORMAL_PIN, HIGH);
  }
  else {
    digitalWrite(COLD_PIN, LOW);
    digitalWrite(HOT_PIN, HIGH);
    digitalWrite(NORMAL_PIN, LOW);
  }
}

float convert_sensor_reading_to_celsius(const int sensor_value) {
  const int adjusted_sensor_value= sensor_value + TMP36_ADJUSTMENT; 
  const float volts = (adjusted_sensor_value * SUPPLY_VOLTAGE) / 1024;
  const float adjustedVoltage = (volts - 500);  
  const float temperature = (adjustedVoltage / 10);
  return temperature;
}

void log_temperature(const int sensorValue, const float temperature) {
  Serial.print(sensorValue);
  Serial.print(",");
  Serial.println(temperature);
  write_values_to_csv(sensorValue, temperature);
}

void write_values_to_csv(const int sensorValue, const float temperature) {
  if (writeToTemperatureFile) {
    #if MEGA_ADK
    tempFile = SD.open("TEMPMEGA.CSV", FILE_WRITE);
    #else
    tempFile = SD.open("MEGA.CSV", FILE_WRITE);
    #endif 
    if (tempFile) {
      tempFile.print(sensorValue);
      tempFile.print(",");
      tempFile.println(temperature);
      tempFile.close();
    }
    else {
      Serial.println("Failed to open CSV file, cannot write data to SD card.");
      writeToTemperatureFile = 0;
    }
  }  
}