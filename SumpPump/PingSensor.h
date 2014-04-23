#ifndef _PING_SENSOR_h
#define _PING_SENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Tmp36Sensor.h"

class PingSensor
{
	uint8_t _pin;
	unsigned long _lastValue;

protected:

public:
	
	// The ping value must change by this much to be accepted.
	// Otherwise it's just the real world preventing 
	// meaningful values. This was arbitrarily determined
	// by manually inspecting the log files.
	static const int MeaningfulDistanceDelta = 7;

	// If the PING))) returns a value greater than this, then 
	// something went wrong with the value. Arbitrarily 
	// determined by inspecting the log files.
	static const int ReasonablePingValue = 3000;

	PingSensor(uint8_t pin);
	
	void update();
	unsigned long getLastValue();

	// Returns the distance, in millimetres, that was measured.
	int getDistance(Tmp36Sensor* tmp36Sensor);
};

#endif