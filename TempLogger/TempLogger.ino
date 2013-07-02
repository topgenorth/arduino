/**
 *
 */
#include <SD.h>
#include <SPI.h>
#include <Ethernet.h>

const unsigned int BAUD_RATE              = 9600;
const unsigned int DHCP_MAX_ATTEMPT_COUNT = 5;

// Millisecond delays, depending on the context
const unsigned long IP_DELAY              = 5000;     // The delay while trying to get the IP address via DHCP.
const unsigned short IP_DELAY_SECONDS     = IP_DELAY / 1000;

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
  Serial.println("********** SETUP     **********");

  // Always initialize the SDCard first.
  init_sdcard();  
  init_ethernet();

  Serial.println();
  Serial.println("********** SETUP COMPLETE *****");    
}

void loop() {
  Serial.println();
  read_temperature_value();
  delay(CONNECTION_INTERVAL);
}

void init_ethernet() {
  int tryToInitializeEthernet = 1;
  int initCount               = 0;
  int intializedEthernet      = 0;
  const unsigned short SECONDS = IP_DELAY / 1000;
  
  Serial.println("Initializing Ethernet.");

  while (tryToInitializeEthernet) {
    initCount++;
    Serial.print("Attempt #");
    Serial.print(initCount);
    Serial.print("...");
    
    if (Ethernet.begin(mac) == 1) {
      initializedEthernet = 1;
      tryToInitializeEthernet = 0;
      Serial.println("succeeded.");
    }
    else {
      Serial.print("failed");
      initializedEthernet = 0;
      if (initCount < DHCP_MAX_ATTEMPT_COUNT) {
        tryToInitializeEthernet = 1;
        Serial.print(", trying again in ");
        Serial.print(IP_DELAY_SECONDS);
        Serial.println(" seconds.");
        delay(IP_DELAY);
      }
      else {
        tryToInitializeEthernet = 0;
        Serial.print("Aborting.");
      }      
    }    
  }
  Serial.println();

  if (initializedEthernet) {
    print_ip_address(Ethernet.localIP());
  }
  else {
    Serial.print("Could not initialize ethernet card after ");
    Serial.print(initCount);
    Serial.print(" tries.");
  }

  Serial.println();
  return;
}

void print_ip_address(IPAddress ipAddress) {
  const unsigned int OCTETS = 4;
  Serial.print("Ethernet initialized, IP address:");
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

