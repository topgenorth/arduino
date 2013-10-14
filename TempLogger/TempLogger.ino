/**
 *
 */
const unsigned int BAUD_RATE               = 9600;
const unsigned int READING_DELAY           = 5000;

// Pins in use by this sketch.
const byte HOT_PIN                         = 2;        // The LED for when it's to warm
const byte TEMP_SENSOR_PIN                 = 3;        // The temperature sensor pin is A0
const byte CS_PIN                          = 4;        // Required by the Ethernet shield
const byte NORMAL_PIN                      = 5;        // The LED for when the temperature is OK
const byte COLD_PIN                        = 6;        // The LED for it's to cool/cold
const byte SDCARD_PIN                      = 10;       // Required by the Ethernet shield


// Constants for converting the TMP36 readings into degress Celsius
const float DEFAULT_TEMPERATURE_ADJUSTMENT = -29.5;     // Use this value to correct the reading of the TMP_36 sensor.
const float SUPPLY_VOLTAGE                 = 5000;     // Volts, not milliVolts
const float HOT_TEMP                       = 24;
const float COOL_TEMP                      = 17;

int firstReading = 1;
void setup() {
  delay(250);
  Serial.begin(BAUD_RATE);

  Serial.println();
  Serial.println("********** SETUP     **********");

  pinMode(HOT_PIN, OUTPUT);
  pinMode(NORMAL_PIN, OUTPUT);
  pinMode(COLD_PIN, OUTPUT);

  Serial.println("Looks like we're all initialized.");

  Serial.println();
  Serial.println("********** SETUP COMPLETE *****");    
}


void loop() {
  Serial.println();
  float temperature = read_temperature_value();
  
  turn_on_led_for(temperature);

  delay(READING_DELAY);
}

float read_temperature_value() {
  int sensorValue = analogRead(TEMP_SENSOR_PIN);
  if (firstReading) {
    // we discard the first reading - give things a chance to settle down.
    delay(2000);
    sensorValue = analogRead(TEMP_SENSOR_PIN);
    firstReading=0;
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