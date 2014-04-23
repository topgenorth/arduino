#define MEGA_ADK 0                                    // Set this to 0 for the Uno R3, 1 for the Mega ADK board.

#include <b64.h>
#include <util.h>
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
const unsigned int LOOP_DELAY = 10 ; // Number of minutes to delay the loop.

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
	Ethernet.begin(mac, ip);
	Serial.println("done.");

	Serial.println("********** SETUP COMPLETE *****");
	Serial.println(" ");
	delay(5000);
}

void loop()
{
	pingSensor.update();
	
	Serial.print("Measured distance: ");
	Serial.print(pingSensor.getDistance(tmp36Sensor));
	Serial.print(" @ ");
	Serial.print(tmp36Sensor.getTemperature());
	Serial.println("C.");

	delay(LOOP_DELAY);
	delay(LOOP_DELAY * 60 * 1000);
}
