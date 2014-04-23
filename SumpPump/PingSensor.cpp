#include "PingSensor.h"

PingSensor::PingSensor(uint8_t pin)
{
	_pin = pin;
	_lastValue = 0;
}

void PingSensor::update() 
{
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
	delayMicroseconds(2);
	digitalWrite(_pin, HIGH);
	delayMicroseconds(5);
	digitalWrite(_pin, LOW);
	pinMode(_pin, INPUT);

	// Do some checks before accepting the new PING))) value.
	unsigned long newValue = pulseIn(_pin, HIGH);

	if (newValue <= PingSensor::ReasonablePingValue)
	{
		if (abs(_lastValue - newValue) > PingSensor::MeaningfulDistanceDelta)
		{			
			_lastValue = newValue;
		}
	}
}

unsigned long PingSensor::getLastValue()
{
	return _lastValue;
}

int PingSensor::getDistance(Tmp36Sensor* tmp36Sensor)
{

	tmp36Sensor->update();
	this->update();

	float speedOfSound = tmp36Sensor->getSpeedOfSound();
	float netDistance = ((float) _lastValue * speedOfSound) / 1000.0;
	float  distance = netDistance / 2.0;
/*
	Serial.print("  > ");
	Serial.print(speedOfSound);
	Serial.print(",");
	Serial.print(_lastValue);
	Serial.print(",");
	Serial.print(distance);
	Serial.println(".");
*/
	return distance;
}
