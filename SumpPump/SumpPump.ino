/*
* This sketch will use a PING))) ultrasonic sensor to find the distance
* to an object. It requires a TMP36 sensor so that the speed of sound
* can be adjusted according to the temperature of the air.
*
* There is a header file not include that defines the Xively API key. 
* It should have the follow line in it:
*      char XIVELY_API_KEY[] = "PUT YOUR XIVELY API KEY HERE";
*
* Tom Opgenorth (tom@opgenorth.net)
*/
#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Xively.h>
#include "XivelyKey.h"
#include "SumpPump.h"
#include "Tmp36Sensor.h"
#include "PingSensor.h"

// Constants
const char PROGRAM_NAME[] = "DistanceFinder.ino v5";
const int LOOP_DELAY = 10 ; // We wait this many minutes between pings.

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
	pingSensor.update();	

	datastreams[0].setInt(pingSensor.getDistance(tmp36Sensor));
	datastreams[1].setInt(pingSensor.getLastValue());
	datastreams[2].setInt(tmp36Sensor.getLastValue());
	int ret = xivelyclient.put(feed, XIVELY_API_KEY);

	Serial.print("Measured distance: ");
	Serial.print(pingSensor.getDistance(tmp36Sensor));
	Serial.print(" @ ");
	Serial.print(tmp36Sensor.getTemperature());
	Serial.print("C. Xively return code ");
	Serial.print(ret);
	Serial.println(".");
	
	delay(LOOP_DELAY * 60 * 1000);
}

