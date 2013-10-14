/**
 *
 */
const unsigned int BAUD_RATE               = 9600;
const unsigned int READING_DELAY           = 60000;

// Pins in use by this sketch.
const byte TEMP_SENSOR_PIN                 = 3;        // The temperature sensor pin is A0
const byte CS_PIN                          = 4;        // Required by the Ethernet shield
const byte SDCARD_PIN                      = 10;       // Required by the Ethernet shield

// Constants for converting the TMP36 readings into degress Celsius
const float DEFAULT_TEMPERATURE_ADJUSTMENT = -2.7;     // Use this value to correct the reading of the TMP_36 sensor.
const float SUPPLY_VOLTAGE                 = 5000;      // Volts, not milliVolts

int firstReading = 1;
void setup() {
  delay(250);
  Serial.begin(BAUD_RATE);

  Serial.println();
  Serial.println("********** SETUP     **********");

  Serial.println("Looks like we're all initialized.");

  Serial.println();
  Serial.println("********** SETUP COMPLETE *****");    
}

void loop() {
  Serial.println();
  read_temperature_value();
  delay(READING_DELAY);
}

void read_temperature_value() {
  int sensorValue = analogRead(TEMP_SENSOR_PIN);
  if (firstReading) {
    // we discard the first reading - give things a chance to settle down.
    delay(2000);
    sensorValue = analogRead(TEMP_SENSOR_PIN);
    firstReading=0;
  }
  float temperature = convert_sensor_reading_to_celsius(sensorValue);
  log_temperature(sensorValue, temperature);
}

float convert_sensor_reading_to_celsius(int sensor_value) {
  const float volts = (sensor_value * SUPPLY_VOLTAGE) / 1024;
  const float adjustedVoltage = (volts - 500);  
  const float temperature = (adjustedVoltage / 10) + DEFAULT_TEMPERATURE_ADJUSTMENT;
  return temperature;
}

void log_temperature(int sensorValue, float temperature) {
  Serial.print(sensorValue);
  Serial.print(",");
  Serial.println(temperature);
}