#define MEGA_ADK 0                                    // Set this to 0 for the Uno R3, 1 for the Mega ADK board.

#include <b64.h>
#include <util.h>
#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Xively.h>

#include "XivelyKey.h"
#include "SumpPump.h"


// Variables
SensorValues sensor_values = { -1000, -273, 0, -1 };


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

	/* add main program code here */

}
