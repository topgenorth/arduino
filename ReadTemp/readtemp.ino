const unsigned int TEMP_SENSOR_PIN=0;
const float SUPPLY_VOLTAGE = 5.0;
const unsigned int BAUD_RATE = 9600;

void setup() 
{
	Serial.begin(BAUD_RATE);
	Serial.println("Starting...");
	delay(500);
}

void loop() 
{
	float temp = get_temperature();
	delay(1000);
} 

float get_temperature() 
{
	// get a reading from the sensor. 
	const int reading = analogRead(TEMP_SENSOR_PIN);
	Serial.print(reading);
	Serial.print(", ");

	// convert that reading to voltage
	const float voltage = reading * SUPPLY_VOLTAGE / 1024;
	Serial.print(voltage);
	Serial.print(", ");

	// Concert the voltage to degrees Celsius.
	const float temp = (voltage - .5) * 100;
	Serial.println(temp);

	return temp;
}