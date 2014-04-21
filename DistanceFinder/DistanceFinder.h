#ifndef DISTANCEFINDER_H 
#define DISTANCEFINDER_H

#define XIVELY_FEED_ID 1645597536

struct SensorValues {
  unsigned long epoch_time;     // The number of seconds since Jan 1 1970
  int tmp36_sensor;             // The raw value from the TMP36 sensor
  float temperature;            // The calculated temperature, in Celsius
  unsigned long ping_sensor;    // The raw sensor value from the PING))), in microseconds.
  int distance;                 // The distance to the object, in millimetres.
};

const char PROGRAM_NAME[]                 = "DistanceFinder.ino v5";
const int BAUD_RATE              		  = 9600;
const int TMP36_ADJUSTMENT                = 0;        // My TMP36 seems to be damaged/mis-calibrated. This should compensate? 
const int MEANINGFUL_DISTANCE_DELTA       = 7;        // The PING))) sensor reading must change by this amount, otherwise it's not significant enough to record
const float SPEED_OF_SOUND                = 331.5;    // metres per second

// Constants for pin declarations
const byte TEMP_SENSOR_PIN                = 0;        // TMP36 Analog Pin 0
const byte CS_PIN                         = 4;        // Required by the Ethernet shield
const byte PING_SENSOR_PIN                = 7;        // PING))) sensor
const byte SDCARD_PIN                     = 10;       // Required by the Ethernet shield
const float SUPPLY_VOLTAGE                = 5000;     // milliVolts
byte mac[]                                = { 0x90, 0xA2, 0xDA, 0x0D, 0xAE, 0x8F };
byte ip[]                                 = { 192, 168, 100, 191 };



#endif   /* !DISTANCEFINDER_H */