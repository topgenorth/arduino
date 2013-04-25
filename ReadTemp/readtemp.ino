/*
 * Just a simple program to read the values from a TMP36 analog sensor.
 * 
 * The middle pin of the sensor uses A0. 
 * The left pin should lead to 5V, while the right pin is to ground.
 *
 * April 24th, 2013
 */

#include <SD.h>

// Analog A0
const unsigned int TEMP_SENSOR_PIN=0;

// Digital pins we need for the Ethernet shield
const unsigned int CS_PIN=4;
const unsigned int SDCARD_PIN=10;

const float SUPPLY_VOLTAGE = 5.0;
const unsigned int BAUD_RATE = 9600;

const unsigned long PAUSE_BETWEEN_TEMP_READINGS = 60000;

File _logFile;

void setup() 
{
	delay(500);
	init_serial();
	delay(500);
	init_sdcard();
	delay(500);
}

void loop() 
{
	float temp = get_temperature();
	delay(PAUSE_BETWEEN_TEMP_READINGS);
} 

void init_serial() 
{
	Serial.begin(BAUD_RATE);
	Serial.println("Starting...");
}

void init_sdcard() 
{
	Serial.print("Initializing SD card...");
	// On the Ethernet Shield, CS is pin 4. It's set as an output by default.
	// Note that even if it's not used as the CS pin, the hardware SS pin 
	// (10 on most Arduino boards, 53 on the Mega) must be left as an output 
	// or the SD library functions will not work. 
	pinMode(SDCARD_PIN, OUTPUT);
	if(!SD.begin(CS_PIN)) 
	{
		Serial.println("initialization failed!");
		return;
	}

	Serial.println("initialization succeded.");
	_logFile= SD.open("readtemp.txt", FILE_WRITE);
	if (_logFile)
	{
		_logFile.println("Starting.");
		_logFile.close();
	}
}

float get_temperature() 
{
	const int reading = analogRead(TEMP_SENSOR_PIN);
	const float voltage = reading * SUPPLY_VOLTAGE / 1024.0;
	const float temp = (voltage - 0.5) * 100;

	write_temp(reading, voltage, temp);

	return temp;
}

void write_temp(int reading, float volts, float temp)
{
	Serial.print(reading);
	Serial.print(", ");
	Serial.print(volts);
	Serial.print("V, ");
	Serial.print(temp);
	Serial.println(" C");

	_logFile = SD.open("readtemp.txt", FILE_WRITE);
	if (_logFile)
	{
		_logFile.print(reading);
		_logFile.print(",");
		_logFile.print(volts);
		_logFile.print(",");
		_logFile.print(temp);
		_logFile.println(" C");
		_logFile.close();
	}

}