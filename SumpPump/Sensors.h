// Sensors.h

#ifndef _SENSORS_h
#define _SENSORS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class SensorsClass
{
	float _supplyVoltage;
	uint8_t _pinTemperatureSensor;
	uint8_t _pinPingSensor;
	void updatePingValue();

protected:
	float convert_tmp36_value_to_celsius();

public:
	int Tmp36Value;
	int PingValue;
	SensorsClass(uint8_t pingSensorPin, uint8_t tmp36SensorPin);

	void update();
};

extern SensorsClass Sensors;

#endif

