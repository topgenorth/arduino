/*
 * Just a simple program to read the values from a TMP36 analog sensor.
 * 
 * The middle pin of the sensor uses A0. 
 * The left pin should lead to 5V, while the right pin is to ground.
 *
 * April 24th, 2013
 */
const float SUPPLY_VOLTAGE = 5.0;
const unsigned int TEMP_SENSOR_PIN = 0;
const unsigned int BAUD_RATE = 9600;
const unsigned int PAUSE_BETWEEN_TEMP_READINGS = 1000;

void setup() 
{
	delay(500);
	init_serial();
	delay(500);
}

void loop() 
{
	delay(PAUSE_BETWEEN_TEMP_READINGS);
	float temperature = get_temperaure();
}

void init_serial() 
{
	Serial.begin(BAUD_RATE);
	Serial.println("Starting...");
}

float get_temperaure() 
{
	const int reading = analogRead(TEMP_SENSOR_PIN);
	const float voltage = reading * SUPPLY_VOLTAGE / 1024;
	const float temp = (voltage-0.5) * 100;

	write_temp(reading, voltage, temp);

	return temp;
}

void write_temp(int reading, float volts, float temp)
{
	Serial.print("Raw=");
	Serial.print(reading);
	Serial.print(", voltage=");
	Serial.print(volts);
	Serial.print(", temp=");
	Serial.print(temp);
	Serial.println(" celsius");
}