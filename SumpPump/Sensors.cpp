#include "Sensors.h"

SensorsClass::SensorsClass(uint8_t pingSensorPin, uint8_t tmp36SensorPin)
{
	_pinPingSensor = pingSensorPin;
	_pinTemperatureSensor = tmp36SensorPin;
	Tmp36Value = -1;
	PingValue = -1;
}

void SensorsClass::update()
{
	Tmp36Value = analogRead(_pinTemperatureSensor);
	updatePingValue();
}

void SensorsClass::updatePingValue()
{
	pinMode(_pinPingSensor, OUTPUT);
	digitalWrite(_pinPingSensor, LOW);
	delayMicroseconds(2);
	digitalWrite(_pinPingSensor, HIGH);
	delayMicroseconds(5);
	digitalWrite(_pinPingSensor, LOW);
	pinMode(_pinPingSensor, INPUT);
}

float SensorsClass::convert_tmp36_value_to_celsius()
{
	return 0.0;
}