#include <Bounce.h> 
const unsigned int BAUD_RATE = 9600;
const unsigned int DEBOUNCE_DELAY = 20;
const unsigned int LED_BIT0 = 12;
const unsigned int LED_BIT1 = 11;
const unsigned int LED_BIT2 = 10;
const unsigned int START_BUTTON_PIN = 5;
int current_value = 0;
int old_value = 0;
Bounce start_button(START_BUTTON_PIN, DEBOUNCE_DELAY);
void setup() 
{
  pinMode(LED_BIT0, OUTPUT);
  pinMode(LED_BIT1, OUTPUT);
  pinMode(LED_BIT2, OUTPUT);
  Serial.begin(BAUD_RATE);
  randomSeed(analogRead(A0));
}

void loop() 
{
  handle_start_button();
}

void handle_start_button() 
{
  if(start_button.update()) 
  {
    if(start_button.read()) 
    {
      const int result = random(1,3);
      output_result(result);
      Serial.print("Result: ");
      Serial.println(result);
      delay(2000);
    }
  }
}

void output_result(const long result) 
{
  switch(result) 
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

