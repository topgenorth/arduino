#include "PingSensor.h"

PingSensor::PingSensor(uint8_t pin)
{
	_pin = pin;
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

	if (newValue <= PingSensor::MeaningfulDistanceDelta)
	{
		if (abs(_lastValue - newValue) > PingSensor::MeaningfulDistanceDelta)
		{
			_lastValue = newValue;
		}
	}
}

long PingSensor::getLastValue()
{
	return _lastValue;
}

int PingSensor::getDistance(Tmp36Sensor tmp36Sensor)
{
	tmp36Sensor.update();
	float netDistance = (_lastValue * tmp36Sensor.getSpeedOfSound()) / 1000.0;
	int distance = netDistance / 2;

}
