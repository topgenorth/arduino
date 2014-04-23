#ifndef SUMPPUMP_H
#define SUMPPUMP_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define XIVELY_FEED_ID 1645597536

// Constants
//const unsigned int REASONABLE_PING_VALUE = 3000;     // If the PING))) sensor gives us a value higher than this, reject it.
const int BAUD_RATE = 9600;
const int TMP36_ADJUSTMENT = 0;        // My TMP36 seems to be damaged/mis-calibrated. This should compensate? 
//const int MEANINGFUL_DISTANCE_DELTA = 7;        // The PING))) sensor reading must change by this amount, otherwise it's not significant enough to record
//const float SPEED_OF_SOUND = 331.5;    // metres per second

// Constants for pin declarations
const float SUPPLY_VOLTAGE = 5000;     // milliVolts

const uint8_t TEMP_SENSOR_PIN = 0;        // TMP36 Analog Pin 0
const uint8_t CS_PIN = 4;        // Required by the Ethernet shield
const uint8_t PING_SENSOR_PIN = 7;        // PING))) sensor
const uint8_t SDCARD_PIN = 10;       // Required by the Ethernet shield

const unsigned int NUMBER_OF_CHANNELS = 3;
char pingSensorChannel[] = "PING_SENSOR_CHANNEL";
char tmp36Channel[] = "TMP36_SENSOR_CHANNEL";
char distanceChannel[] = "CALCULATED_DISTANCE_CHANNEL";

uint8_t mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0xAE, 0x8F };
uint8_t ip[] = { 192, 168, 100, 191 };

XivelyDatastream datastreams[] = {
	XivelyDatastream(distanceChannel, strlen(distanceChannel), DATASTREAM_FLOAT),
	XivelyDatastream(pingSensorChannel, strlen(pingSensorChannel), DATASTREAM_INT),
	XivelyDatastream(tmp36Channel, strlen(tmp36Channel), DATASTREAM_INT)
};

XivelyFeed feed(XIVELY_FEED_ID, datastreams, NUMBER_OF_CHANNELS /* number of datastreams */);
EthernetClient client;
XivelyClient xivelyclient(client);
IPAddress xivelyServer(216, 52, 233, 122);               // numeric IP for api.xively.com
#endif