/**
 * Much like BinaryDice.ino, but use a button instead of constantly looping.
 * 
 * Tom Opgenorth
 * March 3rd, 2013
 */


const unsigned int LED_BIT0=2;
const unsigned int LED_BIT1=3;
const unsigned int LED_BIT2=4;
const unsigned int PAUSE=2000;
const unsigned int BAUD_RATE = 9600;

void setup ()
{
	Serial.begin(BAUD_RATE);
	randomSeed(analogRead(A0));

	pinMode(LED_BIT0, OUTPUT);
	pinMode(LED_BIT1, OUTPUT);
	pinMode(LED_BIT2, OUTPUT);

	Serial.println("RandomNumber2 - setup complete.");
}

void loop()
{
  blink_light();
  delay(PAUSE);
}

/*
 * Picks a number between 1 and 3 and will light
 * up the appropriate LED.
 */
void blink_light() {  
	long result=random(1,4);

	output_result(result);

	Serial.print("showing number ");
	Serial.println(result);
}

/*
 * Light up the appropriate LED on the breadboard.
 */
void output_result(const long result) {

	switch (result) 
	{
		case 1:
			digitalWrite(LED_BIT0,1);
			digitalWrite(LED_BIT1,0);
			digitalWrite(LED_BIT2,0);
			break;
		case 2:
			digitalWrite(LED_BIT0,0);
			digitalWrite(LED_BIT1,1);
			digitalWrite(LED_BIT2,0);
			break;
		default:
			digitalWrite(LED_BIT0,0);
			digitalWrite(LED_BIT1,0);
			digitalWrite(LED_BIT2,1);
			break;
	}
}