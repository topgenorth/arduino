/**
 * This sketch will read the distance to an object using the PING))) sensor.
 * 
 */
#include "SD.h" 

// Set this to 0 for the Uno R3, 1 for the Mega ADK board.
#define MEGA_ADK 0

const unsigned int BAUD_RATE              = 9600;
const unsigned int PING_DELAY             = 2000;
const unsigned int REASONABLE_PING_VALUE  = 3000;     // If the PING))) sensor gives us a value higher than this, reject it.
const byte TEMP_SENSOR_PIN                = 0;        // TMP36 Analog Pin 0
const byte CS_PIN                         = 4;        // Required by the Ethernet shield
const byte PING_SENSOR_PIN                = 7;        // PING))) sensor.
const byte SDCARD_PIN                     = 10;       // Required by the Ethernet shield
const float SUPPLY_VOLTAGE                = 5000;     // Volts, not milliVolts
const int TMP36_ADJUSTMENT                = 0;      // My TMP36 seems to be damaged/mis-calibrated. This should compensate? 
const float SENSOR_GAP                    = 0.2;
const int TEMPERATURE_READING_DELAY       = 10 * 60 * 1000;    // Sample the temperature this many minutes

struct SensorValues {
  int tmp36_sensor;
  float temperature;
  unsigned long ping_sensor;
  int distance;
};

File logFile;
unsigned long last_measurement = millis();
SensorValues sensor_values = { 0, 0.0, 0, 0} ;

void setup() {
  delay(250);
  Serial.begin(BAUD_RATE);
  delay(500);

  Serial.println("********** SETUP     **********");
  init_sdcard();
  Serial.println("********** SETUP COMPLETE *****");    
  delay(5000);
}

void loop() {
  unsigned long current_millis = millis();

  if (abs(current_millis - last_measurement) >= TEMPERATURE_READING_DELAY) {
    sensor_values.tmp36_sensor = analogRead(TEMP_SENSOR_PIN);
    sensor_values.temperature  = convert_sensor_reading_to_celsius(sensor_values.tmp36_sensor);
    last_measurement = millis();
  }

  sensor_values.ping_sensor = read_ping_value();
  sensor_values.distance = scaled_value(microseconds_to_cm(sensor_values)) / 100;

  if (sensor_values.ping_sensor < REASONABLE_PING_VALUE) {
    log_sensorvalues(sensor_values);
    write_values_to_csv(sensor_values);    
  }
  else {
    Serial.println("Unreasonable TMP36 value rejected.");
  }

  delay(PING_DELAY);
}

long scaled_value(const float value) {
  float round_offset = value < 0 ? -0.5 : 0.5;
  return (long) (value * 100 + round_offset);
}

void init_sdcard() {
    Serial.println("DistanceFinder.ino v2 started.");
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
    logFile = SD.open("DISTLOG.CSV", FILE_WRITE);

    if (logFile) {
        logFile.println("DistanceFinder.ino v2 started.");
        logFile.close();
    }
    else {
        Serial.println("Failed to connnect to SD card.");
    }

    // Setup up w5100
    digitalWrite(SDCARD_PIN, HIGH);

    // It takes a bit of time for the w5100 to get going
    delay(5000);
}

float convert_sensor_reading_to_celsius(const int sensor_value) {
  const int adjusted_sensor_value= sensor_value + TMP36_ADJUSTMENT; 
  const float volts = (adjusted_sensor_value * SUPPLY_VOLTAGE) / 1024;
  const float adjustedVoltage = (volts - 500);  
  return adjustedVoltage / 10;
}

void log_sensorvalues(SensorValues values) {
  Serial.print(values.ping_sensor);
  Serial.print(",");
  Serial.print(values.distance);
  Serial.print(",");
  Serial.print(values.tmp36_sensor);
  Serial.print(",");
  Serial.println(values.temperature);
}

void write_values_to_csv(SensorValues values) {
  logFile = SD.open("DISTLOG.CSV", FILE_WRITE);
  if (logFile) {
    logFile.print(values.ping_sensor);
    logFile.print(",");
    logFile.print(values.distance);
    logFile.print(",");
    logFile.print(values.tmp36_sensor);
    logFile.print(",");
    logFile.println(values.temperature);
    logFile.close();
  }
  else {
    Serial.println("Failed to open CSV file, cannot write data to SD card.");
  }
}

const float microseconds_per_cm(float temperature) {
  return 1 / ((331.5 + (0.6 * temperature)) / 10000);
}

const float sensor_offset(float temperature) {
  return SENSOR_GAP * microseconds_per_cm(temperature) * 2;
}

const float microseconds_to_cm(SensorValues values) {
  const float net_distance = max(0, values.ping_sensor - sensor_offset(values.temperature));
  return net_distance / microseconds_per_cm(values.temperature) / 2;
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
