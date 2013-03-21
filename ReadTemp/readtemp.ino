const unsigned int TEMP_SENSOR_PIN=0;
const float SUPPLY_VOLTAGE = 5.0;
const unsigned int BAUD_RATE = 9600;

void setup() 
{
	Serial.begin(BAUD_RATE);
}

void loop() 
{
	Serial.print(get_temperature() );
	Serial.println(" C");
	delay(1000);
} 

float get_temperature() 
{
	const int sensor_voltage = analogRead(TEMP_SENSOR_PIN);
	const float voltage = sensor_voltage * SUPPLY_VOLTAGE / 1024;
	return (voltage * 1000 -500)/10;
}