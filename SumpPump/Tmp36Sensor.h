#ifndef _TMP36_SENSOR_h
#define _TMP36_SENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class Tmp36Sensor
{
	float _supplyVoltage;
	uint8_t _pin;
	int _lastValue;

protected:

public:
	static const int Tmp36SensorAdjustment = -10;

	Tmp36Sensor(uint8_t pin, float supplyVoltage);

	void update();
	int getLastValue();
	float getTemperature();
	float getSpeedOfSound();
};
#endif
