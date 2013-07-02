/**
 *
 */
#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Xively.h>
#include "Secrets.h"  // Xively API key is defined in this file.

const unsigned int BAUD_RATE               = 9600;
const byte DHCP_MAX_ATTEMPT_COUNT          = 5;

// Millisecond delays, depending on the context
const unsigned int IP_DELAY                = 5000;     // The delay while trying to get the IP address via DHCP.
const unsigned int IP_DELAY_SECONDS        = IP_DELAY / 1000;

//const unsigned long CONNECTION_INTERVAL    = 60000;    // Only poll the sensor every 10 minutes.
const unsigned int CONNECTION_INTERVAL     = 1000; 

// Pins in use by this sketch.
const byte TEMP_SENSOR_PIN                 = 3;        // The temperature sensor pin is A0
const byte CS_PIN                          = 4;        // Required by the Ethernet shield
const byte SDCARD_PIN                      = 10;       // Required by the Ethernet shield

// Constants for converting the TMP36 readings into degress Celsius
const float DEFAULT_TEMPERATURE_ADJUSTMENT = -2.7;     // Use this value to correct the reading of the TMP_36 sensor.
const float SUPPLY_VOLTAGE                 = 5000;     // milliVolts
float temperatureAdjustment                = 0.0;

// Ethernet variables.
EthernetClient client;
int initializedEthernet                    = 0;
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // MAC address for your Ethernet shield

// Xively stuff
char sensorID[]                            = "tmp36_sensor_reading";
char temperatureID[]                       = "tmp36_temperature";
char temperatureAdjustmentID[]             = "tmp36_temperature_adjustment";

XivelyDatastream datastreams[] = {
  XivelyDatastream(sensorID, strlen(sensorID), DATASTREAM_FLOAT),
  XivelyDatastream(temperatureID, strlen(temperatureID), DATASTREAM_FLOAT),
  XivelyDatastream(temperatureAdjustmentID, strlen(temperatureAdjustmentID), DATASTREAM_FLOAT),
};
XivelyFeed feed(XIVELY_FEED_ID, datastreams, 3);
XivelyClient xivelyclient(client);

void setup() {
  delay(250);
  Serial.begin(BAUD_RATE);

  Serial.println();
  Serial.println("********** SETUP     **********");

  init_ethernet();
  init_temperature_adjustment() ;

  Serial.println();
  Serial.println("********** SETUP COMPLETE *****");    
}

void loop() {
  Serial.println();
  read_temperature_value();
  delay(CONNECTION_INTERVAL);
}

void init_temperature_adjustment() {  
  if (initializedEthernet) {
    Serial.print("Contacting Xively...");
    int getReturn = xivelyclient.get(feed, XIVELY_API_KEY);

    if(getReturn > 0){
      temperatureAdjustment = feed[2].getFloat();
      if (temperatureAdjustment == 0.0) {
        temperatureAdjustment = DEFAULT_TEMPERATURE_ADJUSTMENT;
        Serial.print("for some reason the Xively value is 0, using ");
        Serial.print(temperatureAdjustment);
        Serial.print(" instead");
      }
      else {
        Serial.print("got temperature adjustment : ");
        Serial.print(temperatureAdjustment);
      }
    }
    else {
      Serial.print("HTTP Error");
      Serial.print(getReturn);
    }
  }
  else {
    temperatureAdjustment = DEFAULT_TEMPERATURE_ADJUSTMENT;
    Serial.print("No internet connection, can't read temperature adjustment value");
  }
  
  Serial.println(".");

  return;
}

int init_ethernet() {
  int tryToInitializeEthernet = 1;
  int initCount               = 0;
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
  return initializedEthernet;
}

void print_ip_address(IPAddress ipAddress) {
  const unsigned int OCTETS = 4;
  Serial.print("Ethernet initialized, IP address: ");
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
  const float temperature = ((milliVolts - 500) / 10) + DEFAULT_TEMPERATURE_ADJUSTMENT;  

  log_temperature(sensorValue, milliVolts, temperature);
  send_to_xively(sensorValue, temperature);
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

void send_to_xively(int sensorValue, float temperature) {
  datastreams[0].setFloat(sensorValue);
  //  datastreams[1].setFloat(temperature);
  int ret = xivelyclient.put(feed, XIVELY_API_KEY);

  //  Serial.print("Uploaded data to Xively, ");
  //  Serial.println(ret);
}

