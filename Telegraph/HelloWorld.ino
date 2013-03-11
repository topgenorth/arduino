/*
 * Simple sketch to output "Hello, world!" in morse code to 
 * by blinking an LED. A dot is 200 milliseconds, a dash
 * is 600 milliseconds.
 */
#include "telegraph.h"

const unsigned int BAUD_RATE = 9600;
const unsigned int OUTPUT_PIN = 13;
const unsigned int DIT_LENGTH = 200;

Telegraph telegraph(OUTPUT_PIN, DIT_LENGTH);

void setup() 
{
	Serial.begin(BAUD_RATE);
}

void loop() 
{
	telegraph.send_message("Hello, world!");
}