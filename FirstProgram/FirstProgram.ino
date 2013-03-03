const unsigned int LED_PIN = 13;
const unsigned int PAUSE=10000;
const unsigned int BAUD_RATE=9600;

void setup() {
  Serial.begin(BAUD_RATE);
  
  Serial.println(""); 
  pinMode(LED_PIN, OUTPUT);
  Serial.println("\nInitialized");
}

void loop() {
  Serial.println("starting");
  
  digitalWrite(LED_PIN, HIGH);
  Serial.println("HIGH");
  delay(PAUSE);
  
  digitalWrite(LED_PIN, LOW);
  Serial.println("LOW");
  delay(PAUSE);
  
  Serial.println("done.");
}
