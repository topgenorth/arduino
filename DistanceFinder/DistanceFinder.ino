/**
 * This sketch will read the distance to an object using the PING))) sensor.
 * 
 * Using the code from http://arduino.cc/en/Tutorial/UdpNtpClient to get the time.
 */
#include "SD.h" 
#include "floatToString.h"
#include "SensorValues.h"
#include "XivelyKey.h"
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <HttpClient.h>
#include <Xively.h>

#define MEGA_ADK 0                                    // Set this to 0 for the Uno R3, 1 for the Mega ADK board.

const char PROGRAM_NAME[]                 = "DistanceFinder.ino v5";
const char LOGFILE_NAME[]                 = "SUMPLOG.CSV";
const unsigned int BAUD_RATE              = 9600;
const unsigned int PING_DELAY             = 1000;
const unsigned int REASONABLE_PING_VALUE  = 3000;     // If the PING))) sensor gives us a value higher than this, reject it.

// Constants for pin declarations
const byte TEMP_SENSOR_PIN                = 0;        // TMP36 Analog Pin 0
const byte CS_PIN                         = 4;        // Required by the Ethernet shield
const byte PING_SENSOR_PIN                = 7;        // PING))) sensor
const byte SDCARD_PIN                     = 10;       // Required by the Ethernet shield
const float SUPPLY_VOLTAGE                = 5000;     // Volts, not milliVolts

// Constants for the TMP36 sensor
const int TMP36_ADJUSTMENT                = 0;        // My TMP36 seems to be damaged/mis-calibrated. This should compensate? 
const int MEANINGFUL_DISTANCE_DELTA       = 7;        // The PING))) sensor reading must change by this amount, otherwise it's not significant enough to record
const float SPEED_OF_SOUND                = 331.5;    // metres per second
const int TEMPERATURE_READING_DELAY       = 10 * 60 * 1000;    // Sample the temperature this many minutes

// Constants for NTP
const int NTP_PACKET_SIZE                 = 48;       // NTP time stamp is in the first 48 bytes of the message
const unsigned int NTP_REQUEST_PORT       = 123;      // All NTP requests are on port 123.
const unsigned int LOCAL_PORT             = 8888;     // The local port to listen for UDP packets.

// Define the strings for our datastream IDs
char timeChannel[] = "TIME_CHANNEL";
char pingSensorChannel[] = "PING_SENSOR_CHANNEL";
char tmp36Channel[] = "TMP36_SENSOR_CHANNEL";

// Xively stuff here.
XivelyDatastream datastreams[] = {
  XivelyDatastream(timeChannel, strlen(timeChannel), DATASTREAM_INT),
  XivelyDatastream(pingSensorChannel, strlen(pingSensorChannel), DATASTREAM_INT),
  XivelyDatastream(tmp36Channel, strlen(tmp36Channel), DATASTREAM_INT)
};
XivelyFeed feed(XIVELY_FEED_ID, datastreams, 3 /* number of datastreams */);
EthernetClient client;
XivelyClient xivelyclient(client);

// Variables
SensorValues sensor_values                = { -1000, -273, 0, -1} ;
unsigned long last_measurement_time       = 0;
byte mac[]                                = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ethernetInitialize                   = 0;
File logFile;
IPAddress xivelyServer(216,52,233,122);               // numeric IP for api.xively.com
IPAddress timeServer(132, 163, 4, 101);               // time-a.timefreq.bldrdoc.gov NTP server
// IPAddress timeServer(132, 163, 4, 102);               // time-b.timefreq.bldrdoc.gov NTP server
// IPAddress timeServer(132, 163, 4, 103);               // time-c.timefreq.bldrdoc.gov NTP server
byte packetBuffer[NTP_PACKET_SIZE];                   //buffer to hold incoming and outgoing packets 
EthernetUDP Udp;                                      // A UDP instance to let us send and receive packets over UDP

void setup() {
  delay(250);
  Serial.begin(BAUD_RATE);
  delay(500);

  Serial.println("********** SETUP     **********");
  init_sdcard();
  init_ethernet();  // call this AFTER we initialize the SD card as we will try to write stuff the SD card.
  Serial.println("********** SETUP COMPLETE *****");    
  delay(5000);
}

void init_sdcard() {
  Serial.println(PROGRAM_NAME);
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

  Serial.println("succeeded.");
  // println_to_logfile("epoch_time,ping value,distance,tmp36_value,temperature");

  // Setup up w5100
  digitalWrite(SDCARD_PIN, HIGH);

  // It takes a bit of time for the w5100 to get going
  delay(5000);
}

void init_ethernet() {
  if (Ethernet.begin(mac) == 0) {
    logln("Could not initialize Ethernet.");
    ethernetInitialize = 0;
  }
  else {
    Udp.begin(LOCAL_PORT);
    ethernetInitialize = 1;
    logln("Initialized Ethernet.");
  }
}

void loop() {
  get_time_from_NTP();

  update_temperature();
  update_distance();

  if (sensor_values.ping_sensor < REASONABLE_PING_VALUE) {
    log_sensorvalues();

    Serial.print("Uploading to Xively...");
    datastreams[0].setInt(sensor_values.epoch_time);
    datastreams[1].setInt(sensor_values.ping_sensor);
    datastreams[2].setInt(sensor_values.tmp36_sensor);
    int ret = xivelyclient.put(feed, XIVELY_API_KEY);
    Serial.print("Xively return ");
    Serial.println(ret);
  }
  else {
    Serial.println("Unreasonable PING))) value rejected.");
  }
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

void log_sensorvalues() {
  String line = String(sensor_values.epoch_time);
  line += ",";
  line += sensor_values.ping_sensor;
  line += ",";
  line += sensor_values.distance;
  line += "mm,";
  line += sensor_values.tmp36_sensor;
  line += ",";

  // There is no += overload for floating point values.
  char temperature_buffer[25];
  floatToString(temperature_buffer, sensor_values.temperature, 1);
  line += temperature_buffer;
  line += "C";
  logln(line);
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

// send an NTP request to the time server at the given address 
unsigned long sendNTPpacket(IPAddress& address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49; 
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now you can send a packet requesting a timestamp:         
  Udp.beginPacket(address, NTP_REQUEST_PORT);
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();  
}

void get_time_from_NTP() {
  unsigned long epoch = get_epoch_time();

  String utcTime = String("The UTC time is ");
  utcTime += (epoch  % 86400L) / 3600;  // print the hour
  utcTime += ":";

  if ( ((epoch % 3600) / 60) < 10 ) {
    // In the first 10 minutes of each hour, we'll want a leading '0'
    utcTime += '0';
  }
  utcTime += (epoch  % 3600) / 60;

  utcTime += ':';
  if ( (epoch % 60) < 10 ) {
    // In the first 10 seconds of each minute, we'll want a leading '0'
    utcTime += '0';
  }
  utcTime += epoch %60;

  sensor_values.epoch_time = epoch;

  //logln(utcTime);
}

// Returns the number of seconds since January 1, 1970
unsigned long get_epoch_time() {
  unsigned long epoch = 0;
  if (ethernetInitialize == 0) {
    return epoch;
  }

  sendNTPpacket(timeServer);  // send an NTP packeet to a time server;
  delay(1000);                // wait to see if a reply is available;
  if ( Udp.parsePacket() ) {  
    // We've received a packet, read the data from it
    Udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);  

    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;  

    // now convert NTP time into everyday time:
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;     
    // subtract seventy years:
    epoch = secsSince1900 - seventyYears;  
  }
  
  return epoch;
}

void logln(String line) {
  // int length = line.length() + 1;
  // char *ptr = (char*)malloc(length);

  // line.toCharArray(ptr, length);

  Serial.println(line);
  // println_to_logfile(ptr);

  logFile = SD.open(LOGFILE_NAME, FILE_WRITE);
  if (logFile) {
    logFile.println(line);
    logFile.close();
  }
  else {
    Serial.println("Couldn't open logfile.");
  }
  delay(750);

}