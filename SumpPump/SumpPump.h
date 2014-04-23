#ifndef SUMPPUMP_H
#define SUMPPUMP_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "XivelyKey.h"
#include "Tmp36Sensor.h"
#include "PingSensor.h"


#define XIVELY_FEED_ID 1645597536

// Constants
const int BAUD_RATE = 9600;
const float SUPPLY_VOLTAGE = 5000; // milliVolts

// Constants for pin declarations
const uint8_t TEMP_SENSOR_PIN = 0;        // TMP36 Analog Pin 0
const uint8_t CS_PIN = 4;        // Required by the Ethernet shield
const uint8_t PING_SENSOR_PIN = 7;        // PING))) sensor
const uint8_t SDCARD_PIN = 10;       // Required by the Ethernet shield

// Stuff we need for Xively
const unsigned int NUMBER_OF_CHANNELS = 3;
EthernetClient client;
IPAddress xivelyServer(216, 52, 233, 122); // numeric IP for api.xively.com

char pingSensorChannel[] = "PING_SENSOR_CHANNEL";
char tmp36Channel[] = "TMP36_SENSOR_CHANNEL";
char distanceChannel[] = "CALCULATED_DISTANCE_CHANNEL";
uint8_t device_mac_address[] = { 0x90, 0xA2, 0xDA, 0x0D, 0xAE, 0x8F };
uint8_t device_ip_address[] = { 192, 168, 100, 191 };

XivelyDatastream datastreams[] = {
	XivelyDatastream(distanceChannel, strlen(distanceChannel), DATASTREAM_FLOAT),
	XivelyDatastream(pingSensorChannel, strlen(pingSensorChannel), DATASTREAM_INT),
	XivelyDatastream(tmp36Channel, strlen(tmp36Channel), DATASTREAM_INT)
};
XivelyFeed feed(XIVELY_FEED_ID, datastreams, NUMBER_OF_CHANNELS);
XivelyClient xivelyclient(client);
#endif