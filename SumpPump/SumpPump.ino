/*
* This sketch will use a PING))) ultrasonic sensor to find the distance
* to an object. It requires a TMP36 sensor so that the speed of sound
* can be adjusted according to the temperature of the air.
*
* There is a header file not include that defines the Xively API key. 
* It should have the follow line in it:
*      char XIVELY_API_KEY[] = "PUT YOUR XIVELY API KEY HERE";
*
* Xively Feed : https://xively.com/feeds/1645597536
*
* Tom Opgenorth (tom@opgenorth.net)
*/
#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Xively.h>
#include "SumpPump.h"

// Constants
const char PROGRAM_NAME[] = "DistanceFinder.ino v5";
const float LOOP_DELAY = 1.5 ; // We wait this many minutes between pings.

// Variables
PingSensor pingSensor(PING_SENSOR_PIN);
Tmp36Sensor tmp36Sensor(TEMP_SENSOR_PIN, SUPPLY_VOLTAGE);

void setup()
{
	delay(250);
	Serial.begin(BAUD_RATE);
	delay(500);

	Serial.println(" ");
	Serial.println("********** SETUP     **********");
	Serial.println(PROGRAM_NAME);
	Serial.println("This version logs to Xively.");

	Serial.print("Initializing Ethernet...");
	Ethernet.begin(device_mac_address, device_ip_address);
	Serial.println("done.");

	Serial.println("********** SETUP COMPLETE *****");
	Serial.println(" ");
	delay(5000);
}

void loop()
{
	int distance = pingSensor.getDistance(&tmp36Sensor);
	unsigned long lastPingValue = pingSensor.getLastValue();
	int lastTmp36Value = tmp36Sensor.getLastValue();
	float lastTemp = tmp36Sensor.getTemperature();

	datastreams[0].setInt(distance);
	datastreams[1].setInt(lastPingValue);
	datastreams[2].setInt(lastTmp36Value);
	int ret = xivelyclient.put(feed, XIVELY_API_KEY);
	
	Serial.print("Values: ");
	Serial.print(distance);
	Serial.print(",");
	Serial.print(lastPingValue);
	Serial.print(",");
	Serial.print(lastTemp);
	Serial.print(",");
	Serial.print(lastTmp36Value);
	Serial.print(",");
	Serial.println(ret);

	delay(LOOP_DELAY * 60000);
}

