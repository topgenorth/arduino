struct SensorValues {
  unsigned long epoch_time;     // The number of seconds since Jan 1 1970
  int tmp36_sensor;             // The raw value from the TMP36 sensor
  float temperature;            // The calculated temperature, in Celsius
  unsigned long ping_sensor;    // The raw sensor value from the PING))), in microseconds.
  int distance;                 // The distance to the object, in millimetres.
};
