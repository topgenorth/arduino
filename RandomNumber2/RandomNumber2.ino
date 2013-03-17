/**
 * Much like BinaryDice.ino, but use a button instead of constantly looping.
 * 
 * Tom Opgenorth
 * March 3rd, 2013
 */

#include <SD.h>

const unsigned int LED_BIT0=2;
const unsigned int LED_BIT1=3;
const unsigned int SD_CARD_PIN=4;
const unsigned int LED_BIT2=5;
const unsigned int BUTTON_PIN=7;
const unsigned int SD_CARD_OUTPUT_PIN=10;

const unsigned int PAUSE=2000;
const unsigned int BAUD_RATE = 9600;
const unsigned int DEBOUNCE_DELAY=50;

File _logFile;
int current_value = 0;
int old_value = 0;

void setup ()
{
	initializeLogging();
	randomSeed(analogRead(A0));

	pinMode(LED_BIT0, OUTPUT);
	pinMode(LED_BIT1, OUTPUT);
	pinMode(LED_BIT2, OUTPUT);
	pinMode(BUTTON_PIN, INPUT);

	logMessage("RandomNumber2 - setup complete.");
	turn_off_all_LED();
}

void initializeLogging() 
{
	Serial.begin(BAUD_RATE);    

	// Initialize the SD card.
	pinMode(SD_CARD_OUTPUT_PIN, OUTPUT);
	if (!SD.begin(SD_CARD_PIN)) 
	{
		Serial.println("Could not initialize SD card.");
	}
	_logFile = SD.open("rannum2.txt", FILE_WRITE);
	if (_logFile)
	{
		_logFile.println("Initialized.");
		_logFile.close();
	} 
	else
	{
		Serial.println("Could not open the file rannum2.txt");
	}	
}

void logMessage(String message) 
{
	Serial.println(message);

	_logFile = SD.open("rannum2.txt", FILE_WRITE);
	if (_logFile)
	{
		_logFile.println(message);
		_logFile.close();
	}
}

void loop()
{
	current_value = digitalRead(BUTTON_PIN);

	if ((current_value != old_value) && (current_value == HIGH))
	{
		turn_off_all_LED();
		output_result(random(1,4));
		delay(DEBOUNCE_DELAY);		
	}
	old_value = current_value;
}

/*
 * Light up the appropriate LED on the breadboard.
 */
void output_result(const long result) 
{
	String message = "Display number: ";
	message += result;
	logMessage(message);
	
	Serial.println(result);
	switch (result) 
	{
		case 1:
			digitalWrite(LED_BIT0,1);
			digitalWrite(LED_BIT1,0);
			digitalWrite(LED_BIT2,0);
			break;
		case 2:
			digitalWrite(LED_BIT0,0);
			digitalWrite(LED_BIT1,1);
			digitalWrite(LED_BIT2,0);
			break;
		default:
			digitalWrite(LED_BIT0,0);
			digitalWrite(LED_BIT1,0);
			digitalWrite(LED_BIT2,1);
			break;
	}
}

void turn_off_all_LED() 
{
	digitalWrite(LED_BIT0, LOW);
	digitalWrite(LED_BIT1, LOW);
	digitalWrite(LED_BIT2, LOW);
	logMessage("Turn off all LEDs.");
}