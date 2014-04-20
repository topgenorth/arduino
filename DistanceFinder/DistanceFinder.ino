/**
 * This sketch will read the distance to an object using the PING))) sensor.
 * 
 * Using the code from http://arduino.cc/en/Tutorial/UdpNtpClient to get the time.
 */

#include "DistanceFinder.h"

#define MEGA_ADK 0                                    // Set this to 0 for the Uno R3, 1 for the Mega ADK board.

#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Xively.h>
#include "LogToXively.h"
#include "XivelyKey.h"
const unsigned int LOOP_DELAY             = 5 * 60  * 1000;        // Number of minutes to delay the loop.
const unsigned int PING_DELAY             = 5 * 1000;     // Wait this many milliseconds before sending another ping.
const unsigned int REASONABLE_PING_VALUE  = 3000;     // If the PING))) sensor gives us a value higher than this, reject it.

// Constants for pin declarations
const byte TEMP_SENSOR_PIN                = 0;        // TMP36 Analog Pin 0
const byte CS_PIN                         = 4;        // Required by the Ethernet shield
const byte PING_SENSOR_PIN                = 7;        // PING))) sensor
const byte SDCARD_PIN                     = 10;       // Required by the Ethernet shield
const float SUPPLY_VOLTAGE                = 5000;     // milliVolts

// Constants for the TMP36 sensor
const int TMP36_ADJUSTMENT                = 0;        // My TMP36 seems to be damaged/mis-calibrated. This should compensate? 
const int MEANINGFUL_DISTANCE_DELTA       = 7;        // The PING))) sensor reading must change by this amount, otherwise it's not significant enough to record
const float SPEED_OF_SOUND                = 331.5;    // metres per second
const int TEMPERATURE_READING_DELAY       = 10 * 60 * 1000;    // Sample the temperature this many minutes

// Variables
SensorValues sensor_values                = { -1000, -273, 0, -1} ;
unsigned long last_measurement_time       = 0;
byte ethernetInitialize                   = 0;
byte mac[]                                = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[]                                 = { 192, 168, 100, 191 };

XivelyDatastream datastreams[] = {
  XivelyDatastream(distanceChannel, strlen(distanceChannel), DATASTREAM_FLOAT),
  XivelyDatastream(pingSensorChannel, strlen(pingSensorChannel), DATASTREAM_INT),
  XivelyDatastream(tmp36Channel, strlen(tmp36Channel), DATASTREAM_INT)
};

XivelyFeed feed(XIVELY_FEED_ID, datastreams, NUMBER_OF_CHANNELS /* number of datastreams */);
EthernetClient client;
XivelyClient xivelyclient(client);
IPAddress xivelyServer(216,52,233,122);               // numeric IP for api.xively.com

void setup() {

  delay(250);
  Serial.begin(BAUD_RATE);
  delay(500);

  Serial.println(" ");
  Serial.println("********** SETUP     **********");
  Serial.println(PROGRAM_NAME);
  Serial.println("This version logs to Xively.");

  init_ethernet(); 

  Serial.println("********** SETUP COMPLETE *****");    
  Serial.println(" ");
  delay(5000);
}

void init_ethernet() {
  Serial.print("Initializing Ethernet...");

  ethernetInitialize = 0;
  Ethernet.begin(mac, ip);
  ethernetInitialize = 1;
  Serial.println("done.");
}

void loop() {
  update_temperature();
  update_distance();

  Serial.print("Distance = ");
  Serial.print(sensor_values.distance);
  Serial.println("mm");

  if (sensor_values.ping_sensor < REASONABLE_PING_VALUE) {
    log_to_xively();
  }
  else {
    Serial.println("Unreasonable PING))) value rejected.");
  }

  delay(LOOP_DELAY);
}

/**
 * If enough time has elapsed since the last temperature reading, this function
 * will convert the tmp36 sensor values to Celsius.
 */
void update_temperature() {
  // unsigned long current_millis = millis();

  // int time_diff = abs(current_millis - last_measurement_time);
  // if (time_diff >= TEMPERATURE_READING_DELAY) {
    sensor_values.tmp36_sensor = analogRead(TEMP_SENSOR_PIN);
    convert_tmp36_reading_to_celsius();
    // last_measurement_time = millis();
  // }
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
  const unsigned long ping_value = read_ping_value();
  const unsigned long ping_delta = abs(ping_value - sensor_values.ping_sensor);

  if (ping_delta >= MEANINGFUL_DISTANCE_DELTA) {
    sensor_values.ping_sensor = ping_value;  
    convert_ping_duration_to_distance();
    return 1;
  }
  else {
    return 0;
  }
}

/**
 * This method will convert the ping sensor value (microseconds) to a distance (millimetres).
 */
void convert_ping_duration_to_distance() {
  // correct the speed of sound based on the air temperature, in metres per second
  const float adjustedSpeedOfSound = speed_of_sound(sensor_values.temperature);

  // Now we find the net distance the ping travelled (i.e from the sensor, to the object, and then back).
  // This value is in millimetres.
  float netDistance =  (sensor_values.ping_sensor * adjustedSpeedOfSound) / 1000.0;

  // The distance to the object, in millimetres, is 1/2 of the total distance.
  unsigned int distance = netDistance / 2;
  sensor_values.distance = distance;
}

/**
 * Adjusts the speed of sound for the air temperature. Returns metres per second. 
 */
float speed_of_sound(float temperature) {
  float adjustment = 0.606 * temperature;
  return SPEED_OF_SOUND + adjustment;
}

int scaled_value(const float value) {
  float round_offset = value < 0 ? -0.5 : 0.5;
  return (long) (value * 100 + round_offset);
}

/**
 * This function will fire off a ping and measure how long (in microseconds) it takes to return.
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


void log_to_xively() {
  datastreams[0].setFloat(sensor_values.distance);
  datastreams[1].setInt(sensor_values.ping_sensor);
  datastreams[2].setInt(sensor_values.tmp36_sensor);
  int ret = xivelyclient.put(feed, XIVELY_API_KEY);
}

void logln(String line) {

  Serial.println(line);

}