#include "Tmp36Sensor.h"

Tmp36Sensor::Tmp36Sensor(uint8_t pin, float supplyVoltage)
{
	_pin = pin;
	_supplyVoltage = supplyVoltage;
	_lastValue = 0;
}

void Tmp36Sensor::update()
{
	_lastValue = analogRead(_pin);
}

int Tmp36Sensor::getLastValue()
{
	return _lastValue;
}

float Tmp36Sensor::getTemperature()
{
	const int adjusted_sensor_value= _lastValue + Tmp36Sensor::Tmp36SensorAdjustment; 
	float voltage = ((float)adjusted_sensor_value * _supplyVoltage) / 1024; // this is in milli-volts.
	float adjustedVolts = voltage - 500.0; // Take away 500 millivolts.
	float temperature = adjustedVolts / 10.0; // Divide by 10 to get the temperature in degrees Celsius.
/*
	Serial.print("   TMP36 > ");
	Serial.print(_lastValue);
	Serial.print(",");
	Serial.print(voltage);
	Serial.print(",");
	Serial.print(temperature);
	Serial.println(".");
*/	
	return temperature;
}

float Tmp36Sensor::getSpeedOfSound()
{
	// returns the speed of sound in metres/second for a given temperature in Celsius.
	// From http://en.wikipedia.org/wiki/Speed_of_sound#Practical_formula_for_dry_air
	float adjustment = 0.606 * getTemperature();
	return 331.3 + adjustment; 
}