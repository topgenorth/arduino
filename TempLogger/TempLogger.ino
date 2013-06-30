/**
 *
 */
#include <SD.h>
#include <SPI.h>
#include <Ethernet.h>

const unsigned int BAUD_RATE              = 9600;
const unsigned int DHCP_MAX_ATTEMPT_COUNT = 15;

// Millisecond delays, depending on the context
const unsigned long IP_DELAY              = 5000;     // The delay while trying to get the IP address via DHCP.
//const unsigned long CONNECTION_INTERVAL   = 60000;    // Only poll the sensor every 10 minutes.
const unsigned long CONNECTION_INTERVAL   = 1000; 

// Pins in use by this sketch.
const unsigned int TEMP_SENSOR_PIN        = 3;        // The temperature sensor pin is A0
const unsigned int CS_PIN                 = 4;        // Required by the Ethernet shield
const unsigned int SDCARD_PIN             = 10;       // Required by the Ethernet shield

// Constants for converting the TMP36 readings into degress Celsius
const float TEMPERATURE_ADJUSTMENT        = -2.7;     // Use this value to correct the reading of the TMP_36 sensor.
const float SUPPLY_VOLTAGE                = 5000;     // milliVolts

// Ethernet variables.
EthernetClient client;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // MAC address for your Ethernet shield
unsigned int initializedEthernet = 0;

void setup() {
  delay(250);
  Serial.begin(BAUD_RATE);

  Serial.println();
  Serial.println("********** SETUP **********");

  // Always initialize the SDCard first.
  init_sdcard();  
  init_ethernet();

  Serial.println("****** SETUP COMPLETE *****");    
}

void loop() {
  Serial.println();
  read_temperature_value();
  delay(CONNECTION_INTERVAL);
}

void init_ethernet() {
  int initCount = 0;
  File logFile = SD.open("readtemp.txt", FILE_WRITE);
  if (!logFile) {
    Serial.println("The SD card is not initialized - exiting.");
    return;
  }
  
  logFile.print("Initializing Ethernet...");
  Serial.println("Initializing Ethernet...");
  initializedEthernet = 0;

  while (Ethernet.begin(mac) != 1) {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(IP_DELAY);
  }  
}

void print_ip_address(IPAddress ipAddress) {
  const unsigned int OCTETS = 4;
  Serial.print("Network initialized, IP address:");
  for (unsigned int i=0; i<OCTETS; i++) {
    Serial.print(ipAddress[i]);
    if (i != OCTETS - 1) {
      Serial.print(".");
    }
  }
  Serial.println();
}

void read_temperature_value() {
  const int sensorValue = analogRead(TEMP_SENSOR_PIN);
  const float milliVolts = sensorValue * SUPPLY_VOLTAGE / 1024.0;
  const float temperature = ((milliVolts - 500) / 10) + TEMPERATURE_ADJUSTMENT;  
  
  log_temperature(sensorValue, milliVolts, temperature);
}

void log_temperature(int sensorValue, float milliVolts, float temperature) {
  
  // Write to the serial port.
  Serial.print("TMP36 reading: ");
  Serial.print(sensorValue);
  Serial.print(", milliVolts: ");
  Serial.print(milliVolts);
  Serial.print("mV, ");
  Serial.print(temperature);
  Serial.print(" Centigrade");
}

void init_sdcard() {
  delay(150);
  Serial.print("Initializing SD card...");
  pinMode(SDCARD_PIN, OUTPUT);
  if(!SD.begin(CS_PIN)) {
    Serial.println("initialization failed!");
    return;
  }

  Serial.println("initialization succeded.");
  File logFile= SD.open("readtemp.txt", FILE_WRITE);
  if (logFile) {
    logFile.println("Starting.");
    logFile.close();
  }  
}

