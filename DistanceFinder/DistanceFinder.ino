/**
 * This sketch will read the distance to an object using the PING))) sensor.
 * 
 */
#include "SD.h" 

#define MEGA_ADK 0 // Set this to 0 for the Uno R3, 1 for the Mega ADK board.
#define USE_MM 1   // Set this to 1 to measure in millimetres, set it to 0 to use centimetres

const unsigned int BAUD_RATE              = 9600;
const unsigned int PING_DELAY             = 2000;
const unsigned int REASONABLE_PING_VALUE  = 3000;     // If the PING))) sensor gives us a value higher than this, reject it.
const byte TEMP_SENSOR_PIN                = 0;        // TMP36 Analog Pin 0
const byte CS_PIN                         = 4;        // Required by the Ethernet shield
const byte PING_SENSOR_PIN                = 7;        // PING))) sensor.
const byte SDCARD_PIN                     = 10;       // Required by the Ethernet shield
const float SUPPLY_VOLTAGE                = 5000;     // Volts, not milliVolts
const int TMP36_ADJUSTMENT                = 0;        // My TMP36 seems to be damaged/mis-calibrated. This should compensate? 
const float SENSOR_GAP                    = 0.2;      // This is the time to wait between pings.
const int TEMPERATURE_READING_DELAY       = 1 * 60 * 1000;    // Sample the temperature this many minutes
const int MEANINGFUL_TMP36_DELTA          = 0;        // The TMP36 sensor value must change by at least this amount to record the change.

struct SensorValues {
  int tmp36_sensor;
  float temperature;
  unsigned long ping_sensor;
  int distance;
};

File logFile;
unsigned long last_measurement_time = 0;
SensorValues sensor_values = { -1000, -273, -1, -1} ;

void setup() {
  delay(250);
  Serial.begin(BAUD_RATE);
  delay(500);

  Serial.println("********** SETUP     **********");
  init_sdcard();
  Serial.println("********** SETUP COMPLETE *****");    
  delay(5000);
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

void loop() {
  update_temperature();
  update_distance();
  if (sensor_values.ping_sensor < REASONABLE_PING_VALUE) {
    write_values_to_csv(sensor_values);    
  }
  else {
    Serial.println("Unreasonable TMP36 value rejected.");
  }
  log_sensorvalues(sensor_values);
  delay(PING_DELAY);
}

/**
 * If enough time has elapsed since the last temperature reading, this function
 * will convert the tmp36 sensor values to Celsius.
 */
void update_temperature() {
  unsigned long current_millis = millis();

  int time_diff = abs(current_millis - last_measurement_time);
  if (time_diff >= TEMPERATURE_READING_DELAY) {
    sensor_values.tmp36_sensor = analogRead(TEMP_SENSOR_PIN);
    convert_tmp36_reading_to_celsius();
    last_measurement_time = millis();
  }
}

/**
 * Does the math to convert the tmp36 value to Celsius (does not check for a time lapse).
 */
float convert_tmp36_reading_to_celsius() {
  const int adjusted_sensor_value= sensor_values.tmp36_sensor + TMP36_ADJUSTMENT; 
  const float volts = (adjusted_sensor_value * SUPPLY_VOLTAGE) / 1024;
  const float adjustedVoltage = (volts - 500.0);  
  sensor_values.temperature = adjustedVoltage / 10.0;
}

/**
 * This function will update the distance, but only if it has changed by a meaningful amount.
 * Returns 1 if the distance has been updated, 0 if it hasn't.
 */ 
int update_distance() {
  unsigned long ping_value = read_ping_value();
  if (abs(ping_value - sensor_values.ping_sensor) >= MEANINGFUL_TMP36_DELTA) {
    sensor_values.ping_sensor = ping_value;  
    sensor_values.distance = scaled_value(microseconds_to_distance(sensor_values)) / 100;
    return 1;
  }
  else {
    return 0;
  }
}

int scaled_value(const float value) {
  float round_offset = value < 0 ? -0.5 : 0.5;
  return (long) (value * 100 + round_offset);
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

const float sensor_offset(float temperature) {
  return SENSOR_GAP * speed_of_sound(temperature) * 2;
}

/**
 * This function will figure out the speed of sound, adjusted by the temperature. The units
 * returned are either in millimetres or centimetres (depends on the USE_MM flag).
 */
const float speed_of_sound(float temperature) {
  float time_for_distance=1 / ((331.5 + (0.6 * temperature)) / 10000);
#if USE_MM
  return time_for_distance * 10;
#else
  return time_for_distance;
#endif
}

/**
 * This function will convert the duration (how long the ping was) to a distance. The units
 * returned are either millimeters or centimetres (depends on the USE_MM flag).
 */
const float microseconds_to_distance(SensorValues values) {
  const float net_distance = max(0, values.ping_sensor - sensor_offset(values.temperature));
  return net_distance / speed_of_sound(values.temperature) / 2;
}

/**
 * This function will fire off a ping and measure how long it takes to return.
 */
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
