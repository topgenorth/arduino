/**
 * This sketch will read the distance to an object using the PING))) sensor.
 * 
 */
#include "SD.h" 

// Set this to 0 for the Uno R3, 1 for the Mega ADK board.
#define MEGA_ADK 1

const unsigned int BAUD_RATE               = 9600;
const unsigned int READING_DELAY           = 3000;
const byte HOT_PIN                         = 2;        // The LED for when it's to warm
#if MEGA_ADK
const byte TEMP_SENSOR_PIN                 = 0;
#else
const byte TEMP_SENSOR_PIN                 = 8;        // The temperature sensor pin is A0
#endif
const byte CS_PIN                          = 4;        // Required by the Ethernet shield
const byte NORMAL_PIN                      = 5;        // The LED for when the temperature is OK
const byte COLD_PIN                        = 6;        // The LED for it's to cool/cold
#if MEGA_ADK
const byte PING_SENSOR_PIN                 = 49;       // PING))) sensor.
#else
const byte PING_SENSOR_PIN                 = 7;        // PING))) sensor.
#endif
const byte SDCARD_PIN                      = 10;       // Required by the Ethernet shield
const float SUPPLY_VOLTAGE                 = 5000;     // Volts, not milliVolts
const int HOT_TEMP                         = 24;
const int COOL_TEMP                        = 15;
const int TMP36_ADJUSTMENT                 = -60;      // My TMP36 seems to be damaged/mis-calibrated. This should compensate? 

File logFile;
int writeToLogFile = 1;

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

void loop() {
    const float temperature = read_temperature_value();
    const unsigned long duration = read_ping_value();

    Serial.print("Distance: ");
    Serial.print(microseconds_to_cm(duration));
    Serial.println(" cm");
    delay(100);

    turn_on_led_for(temperature);

    delay(READING_DELAY);
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
    logFile = SD.open("TEMPMEGA.CSV", FILE_WRITE);
    #else
    logFile = SD.open("TEMP.CSV", FILE_WRITE);
    #endif

    if (logFile) {
        logFile.close();
    }
    else {
        Serial.println("Failed to connnect to SD card.");
        writeToLogFile=0;
    }

    // Setup up w5100
    digitalWrite(SDCARD_PIN, HIGH);

    // It takes a bit of time for the w5100 to get going
    delay(2000);
}

const float read_temperature_value() {
  int sensorValue = analogRead(TEMP_SENSOR_PIN);
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
  write_temperature_values_to_csv(sensorValue, temperature);
}

void write_temperature_values_to_csv(const int sensorValue, const float temperature) {
  if (writeToLogFile) {
    #if MEGA_ADK
    logFile = SD.open("TEMPMEGA.CSV", FILE_WRITE);
    #else
    logFile = SD.open("TEMP.CSV", FILE_WRITE);
    #endif 
    if (logFile) {
      logFile.print(sensorValue);
      logFile.print(",");
      logFile.println(temperature);
      logFile.close();
    }
    else {
      Serial.println("Failed to open CSV file, cannot write data to SD card.");
      writeToLogFile = 0;
    }
  }  
}

unsigned long microseconds_to_cm(const unsigned long microseconds) {
    return microseconds / 29 / 2;
}

const unsigned long read_ping_value() {
    pinMode(PING_SENSOR_PIN, OUTPUT);
    digitalWrite(PING_SENSOR_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(PING_SENSOR_PIN, HIGH);
    delayMicroseconds(5);
    digitalWrite(PING_SENSOR_PIN, LOW); 
    pinMode(PING_SENSOR_PIN, INPUT);
    return pulseIn(PING_SENSOR_PIN, HIGH);
}