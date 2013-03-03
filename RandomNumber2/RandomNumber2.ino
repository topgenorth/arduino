/**
 * Much like BinaryDice.ino, but use a button instead of constantly looping.
 * 
 * Tom Opgenorth
 * March 3rd, 2013
 */


const unsigned int LED_BIT0=2;
const unsigned int LED_BIT1=3;
const unsigned int LED_BIT2=4;
const unsigned int BUTTON_PIN=7;

const unsigned int PAUSE=2000;
const unsigned int BAUD_RATE = 9600;
const unsigned int DEBOUNCE_DELAY=50;


int current_value = 0;
int old_value = 0;

void setup ()
{
	Serial.begin(BAUD_RATE);    
	randomSeed(analogRead(A0));

	pinMode(LED_BIT0, OUTPUT);
	pinMode(LED_BIT1, OUTPUT);
	pinMode(LED_BIT2, OUTPUT);

	pinMode(BUTTON_PIN, INPUT);

	Serial.println("RandomNumber2 - setup complete.");
	turn_off_all_LED();
}

void loop()
{
	current_value = digitalRead(BUTTON_PIN);

	if ((current_value != old_value) && (current_value == HIGH))
	{
		turn_off_all_LED();
		output_result(random(1,4));
		delay(DEBOUNCE_DELAY);		
	}
	old_value = current_value;
}

/*
 * Light up the appropriate LED on the breadboard.
 */
void output_result(const long result) 
{
	Serial.print("showing number ");
	Serial.println(result);
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

void turn_off_all_LED() 
{
	digitalWrite(LED_BIT0, LOW);
	digitalWrite(LED_BIT1, LOW);
	digitalWrite(LED_BIT2, LOW);
	Serial.println("Turned off all the LEDs.");
}